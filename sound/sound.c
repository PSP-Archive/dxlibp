#include "../sound.h"
#include "../fileio.h"
#include <string.h>
#include <pspaudio.h>
#include "../general.h"
#include "../safealloc.h"

DXPSOUNDHANDLE dxpSoundArray[DXP_BUILDOPTION_SOUNDHANDLE_MAX];
DXPSOUNDDATA dxpSoundData = {0,DX_SOUNDDATATYPE_MEMNOPRESS};


int dxpSoundInit()
{
	if(dxpSoundData.init)return 0;
	int ret;
	ret = sceUtilityLoadAvModule(PSP_AV_MODULE_AVCODEC);
	if(ret < 0)return -1;
	memset(&dxpSoundArray,0,sizeof(dxpSoundArray));
	dxpSoundData.init = 1;
	sceKernelStartThread(sceKernelCreateThread("dxp mnp sound thread",dxpSoundThreadFunc_memnopress,0x11,0x4000,PSP_THREAD_ATTR_USER,0),0,0);
	return 0;
}

int dxpSoundTerm()
{
	if(!dxpSoundData.init)return 0;
	//‘S•”‚É‘Î‚µ‚ÄStop‚ð‚©‚¯‚é
	int i;
	for(i = 0;i < DXP_BUILDOPTION_SOUNDHANDLE_MAX;++i)
		DeleteSoundMem(i);
	sceUtilityUnloadAvModule(PSP_MODULE_AV_AVCODEC);
	dxpSoundData.init = 0;
	return 0;
}

int dxpSoundReserveHandle()
{
	if(!dxpSoundData.init)return -1;
	int i;
	DXPSOUNDHANDLE *pHnd;
	for(i = 0;i < DXP_BUILDOPTION_SOUNDHANDLE_MAX;++i)
	{
		if(dxpSoundArray[i].used)continue;
		pHnd = dxpSoundArray + i;
		memset(pHnd,0,sizeof(DXPSOUNDHANDLE));
		pHnd->volume = 255;
		pHnd->used = 1;
		return i;
	}
	return -1;
}

int dxpSoundReleaseHandle(int handle)
{
	if(!dxpSoundData.init)return -1;
	DXPSOUNDHANDLE *pHnd;
	if(handle < 0 || handle >= DXP_BUILDOPTION_SOUNDHANDLE_MAX)return -1;
	pHnd = dxpSoundArray + handle;
	pHnd->used = 0;
	return 0;
}


int LoadSoundMem(const char *filename)
{
	int stat;
	if(!dxpSoundData.init)return -1;
	int i;
	int fileSize = FileRead_size(filename);
	if(fileSize <= 0)return -1;
	int fileHandle = FileRead_open(filename,0);
	if(fileHandle < 0)return -1;
	int handle = dxpSoundReserveHandle();
	if(handle < 0)
	{
		FileRead_close(fileHandle);
		return -1;
	}
	DXPSOUNDHANDLE *pHnd = dxpSoundArray + handle;
	pHnd->avContext.fileHandle = fileHandle;
	pHnd->avContext.fileSize = fileSize;
	if(dxpSoundCodecInit(pHnd) < 0)
	{
		dxpSoundReleaseHandle(handle);
		FileRead_close(fileHandle);
		return -1;
	}
	pHnd->soundDataType = dxpSoundData.createSoundDataType;
	switch(dxpSoundData.createSoundDataType)
	{
	case DX_SOUNDDATATYPE_MEMNOPRESS:
		pHnd->memnopress.length = dxpSoundCodecGetSampleLength(pHnd);
		pHnd->memnopress.pcmBuf = memalign(64,pHnd->memnopress.length * 4);
		if(!pHnd->memnopress.pcmBuf)
		{
			free(pHnd->memnopress.pcmBuf);
			dxpSoundCodecEnd(pHnd);
			dxpSoundReleaseHandle(handle);
			FileRead_close(fileHandle);
			return -1;
		}
		memset(pHnd->memnopress.pcmBuf,0,pHnd->memnopress.length);
		pHnd->avContext.nextPos = 0;
		for(i = 0;1;++i)
		{
			if(pHnd->avContext.nextPos > pHnd->memnopress.length)break;
			pHnd->avContext.pcmOut = pHnd->memnopress.pcmBuf + pHnd->avContext.nextPos;
			if(dxpSoundCodecDecode(pHnd) < 0)break;
		}
		dxpSoundCodecEnd(pHnd);
		FileRead_close(fileHandle);
		break;
	case DX_SOUNDDATATYPE_FILE:
		pHnd->file.threadId = sceKernelCreateThread("dxp sound file thread",dxpSoundThreadFunc_file,0x11,0x4000,PSP_THREAD_ATTR_USER,0);
		if(pHnd->file.threadId < 0)
		{
			dxpSoundCodecEnd(pHnd);
			dxpSoundReleaseHandle(handle);
			FileRead_close(fileHandle);
			return -1;
		}
		stat = sceKernelStartThread(pHnd->file.threadId,4,&handle);
		if(stat < 0)
		{
			sceKernelDeleteThread(pHnd->file.threadId);
			dxpSoundCodecEnd(pHnd);
			dxpSoundReleaseHandle(handle);
			FileRead_close(fileHandle);
			return -1;
		}
		pHnd->file.gotoPos = -1;
		break;
	default:
		dxpSoundCodecEnd(pHnd);
		dxpSoundReleaseHandle(handle);
		FileRead_close(fileHandle);
		return -1;
	}
	return handle;
}

int PlaySoundMem(int handle,int playtype,int rewindflag)
{
	DXPSOUNDHANDLE *pHnd;
	SHND2PTR(handle,pHnd);
//	while(pHnd->cmd != DXP_SOUNDCMD_NONE)sceKernelDelayThread(100);
	switch(pHnd->soundDataType)
	{
	case DX_SOUNDDATATYPE_MEMNOPRESS:
		if(playtype == DX_PLAYTYPE_NORMAL)
		{
			int channel = sceAudioChReserve(PSP_AUDIO_NEXT_CHANNEL,pHnd->avContext.outSampleNum,PSP_AUDIO_FORMAT_STEREO);
			if(channel < 0)return 0;
			int pos = 0;
			while(pos < pHnd->memnopress.length)
			{
				sceAudioOutputPannedBlocking(channel,
					PSP_AUDIO_VOLUME_MAX * (pHnd->pan > 0 ? 1.0f - pHnd->pan / 10000.0f : 1.0f) * pHnd->volume / 255.0f,
					PSP_AUDIO_VOLUME_MAX * (pHnd->pan < 0 ? 1.0f + pHnd->pan / 10000.0f : 1.0f) * pHnd->volume / 255.0f,
					pHnd->memnopress.pcmBuf + pos);
				pos += pHnd->avContext.outSampleNum;
			}
			sceAudioChRelease(channel);
			return 0;
		}
		while(pHnd->cmd != DXP_SOUNDCMD_NONE)sceKernelDelayThread(100);
		pHnd->memnopress.cmdplaytype = playtype;
		pHnd->cmd = DXP_SOUNDCMD_PLAY;
		break;
	case DX_SOUNDDATATYPE_FILE:
		pHnd->file.loop = playtype == DX_PLAYTYPE_LOOP ? 1 : 0;
		if(rewindflag)pHnd->file.gotoPos = 0;
		while(pHnd->cmd != DXP_SOUNDCMD_NONE)sceKernelDelayThread(100);
		pHnd->cmd = DXP_SOUNDCMD_PLAY;
		if(playtype == DX_PLAYTYPE_NORMAL)
		{
			while(pHnd->cmd != DXP_SOUNDCMD_NONE)sceKernelDelayThread(100);
			while(pHnd->playing)sceKernelDelayThread(100);
		}
		break;
	default:
		return -1;
	}
	return 0;
}

int StopSoundMem(int handle)
{
	DXPSOUNDHANDLE *pHnd;
	SHND2PTR(handle,pHnd);
//	while(pHnd->cmd != DXP_SOUNDCMD_NONE)sceKernelDelayThread(100);
	pHnd->cmd = DXP_SOUNDCMD_STOP;
	return 0;
}

int DeleteSoundMem(int handle)
{
	DXPSOUNDHANDLE *pHnd;
	SHND2PTR(handle,pHnd);
	while(pHnd->cmd != DXP_SOUNDCMD_NONE)sceKernelDelayThread(100);
	pHnd->cmd = DXP_SOUNDCMD_EXIT;
	switch(pHnd->soundDataType)
	{
	case DX_SOUNDDATATYPE_MEMNOPRESS:
		while(pHnd->playing > 0)sceKernelDelayThread(100);
		free(pHnd->memnopress.pcmBuf);
		dxpSoundReleaseHandle(handle);
		return 0;
	case DX_SOUNDDATATYPE_FILE:
		while(pHnd->playing > 0)sceKernelDelayThread(100);
		dxpSoundCodecEnd(pHnd);
		FileRead_close(pHnd->avContext.fileHandle);
		dxpSoundReleaseHandle(handle);
		return 0;
	default:
		return -1;
	}
}

int CheckSoundMem(int handle)
{
	DXPSOUNDHANDLE *pHnd;
	SHND2PTR(handle,pHnd);
	return pHnd->playing ? 1 : 0;
}

int InitSoundMem()
{
	if(!dxpSoundData.init)return -1;
	int i;
	for(i = 0;i < DXP_BUILDOPTION_SOUNDHANDLE_MAX;++i)
		DeleteSoundMem(i);
	return 0;
}

int SetCreateSoundDataType(int type)
{
	switch(type)
	{
	case DX_SOUNDDATATYPE_MEMNOPRESS:
	case DX_SOUNDDATATYPE_FILE:
		dxpSoundData.createSoundDataType = type;
	default:
		return -1;
	}
}

int SetPanSoundMem(int pan,int handle)
{
	if(pan > 10000)pan = 10000;
	if(pan < -10000)pan = -10000;
	DXPSOUNDHANDLE *pHnd;
	SHND2PTR(handle,pHnd);
	pHnd->pan = pan;
	return 0;
}

int ChangeVolumeSoundMem(int volume,int handle)
{
	if(volume > 255)volume = 255;
	if(volume < 0)volume = 0;
	DXPSOUNDHANDLE *pHnd;
	SHND2PTR(handle,pHnd);
	pHnd->volume = volume;
	return 0;
}

int SetLoopPosSoundMem(int looppos_s,int handle)
{
	if(looppos_s < 0)return -1;
	DXPSOUNDHANDLE *pHnd;
	SHND2PTR(handle,pHnd);
	looppos_s = pHnd->avContext.sampleRate * looppos_s;
	return SetLoopSamplePosSoundMem(looppos_s,handle);
}

int SetLoopSamplePosSoundMem(int looppos,int handle)
{
	if(looppos < 0)return -1;
	DXPSOUNDHANDLE *pHnd;
	SHND2PTR(handle,pHnd);
	switch(pHnd->soundDataType)
	{
	case DX_SOUNDDATATYPE_MEMNOPRESS:
		if(looppos >= pHnd->memnopress.length)return -1;
	}
	pHnd->loopResumePos = looppos;
	return 0;
}