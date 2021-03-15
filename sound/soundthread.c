#include "../sound.h"
#include "../fileio.h"
#include <string.h>
#include <pspaudio.h>
#include "../general.h"
#include "../safealloc.h"

int dxpSoundThreadFunc_file(SceSize size,void* argp)
{
	int channel = -1;
	u32 *pcmBuf[2] = {NULL,NULL};
	u32 pcmBufSize[2] = {0,0};
	u8 pcm = 1;
	DXPSOUNDHANDLE *pHnd = dxpSoundArray + *(int*)argp;
	while(dxpSoundData.init)
	{
		sceKernelDelayThread(1000);
		pcm ^= 1;
		if(dxpGeneralData.exit_called)break;
		if(dxpGeneralData.homebutton_pushed)break;
		if(pHnd->cmd == DXP_SOUNDCMD_EXIT)break;
		switch(pHnd->cmd)
		{
		case DXP_SOUNDCMD_NONE:
			break;
		case DXP_SOUNDCMD_PLAY:
			pHnd->playing = 1;
			pHnd->cmd = DXP_SOUNDCMD_NONE;
			break;
		case DXP_SOUNDCMD_STOP:
			pHnd->playing = 0;
			pHnd->cmd = DXP_SOUNDCMD_NONE;
			break;
		}
		if(pHnd->playing)
		{
			if(channel < 0)
				channel = sceAudioChReserve(PSP_AUDIO_NEXT_CHANNEL,pHnd->avContext.outSampleNum,PSP_AUDIO_FORMAT_STEREO);
			if(channel < 0)
				continue;
			if(pcmBufSize[pcm] < pHnd->avContext.outSampleNum * 4)
			{
				dxpSafeFree(pcmBuf[pcm]);
				pcmBuf[pcm] = dxpSafeAlloc(pHnd->avContext.outSampleNum * 4);
				if(!pcmBuf[pcm])
				{
					pcmBufSize[pcm] = 0;
					pHnd->playing = 0;
					continue;
				}
				pcmBufSize[pcm] = pHnd->avContext.outSampleNum * 4;
			}
			pHnd->avContext.pcmOut = pcmBuf[pcm];
			if(pHnd->file.gotoPos >= 0)
			{
				dxpSoundCodecSeek(pHnd,pHnd->file.gotoPos);
				dxpSoundCodecDecode(pHnd);
				pHnd->file.gotoPos = -1;
			}
			if(dxpSoundCodecDecode(pHnd) < 0)
			{
				if(pHnd->file.loop)
				{
					dxpSoundCodecSeek(pHnd,pHnd->loopResumePos);
					continue;
				}
				dxpSoundCodecSeek(pHnd,0);
				pHnd->playing = 0;
				continue;
			}
			while(sceAudioGetChannelRestLen(channel) > 0)sceKernelDelayThread(1000);
			sceAudioSetChannelDataLen(channel,pHnd->avContext.outSampleNum);
			sceAudioOutputPanned(channel,
				PSP_AUDIO_VOLUME_MAX * (pHnd->pan > 0 ? 1.0f - pHnd->pan / 10000.0f : 1.0f) * pHnd->volume / 255.0f,
				PSP_AUDIO_VOLUME_MAX * (pHnd->pan < 0 ? 1.0f + pHnd->pan / 10000.0f : 1.0f) * pHnd->volume / 255.0f,
				pcmBuf[pcm]);
		}else
		{
			if(channel >= 0)
			{
				sceAudioChRelease(channel);
				channel = -1;
			}
		}
	}
	dxpSafeFree(pcmBuf[0]);
	dxpSafeFree(pcmBuf[1]);
	if(channel >= 0)sceAudioChRelease(channel);
	pHnd->file.threadId = -1;
	pHnd->cmd = DXP_SOUNDCMD_NONE;
	pHnd->playing = 0;
	sceKernelExitDeleteThread(0);
	return 0;
}


int dxpSoundThreadFunc_memnopress(SceSize len,void* ptr)
{
	int i,j;
	int handle[PSP_AUDIO_CHANNEL_MAX];
	int pos[PSP_AUDIO_CHANNEL_MAX];
	int playtype[PSP_AUDIO_CHANNEL_MAX];
	int channel[PSP_AUDIO_CHANNEL_MAX];
	for(i = 0;i < PSP_AUDIO_CHANNEL_MAX;++i)
	{
		handle[i] = -1;
		channel[i] = -1;
	}
	while(dxpSoundData.init)
	{
		sceKernelDelayThread(1000);
		if(dxpGeneralData.exit_called)break;
		if(dxpGeneralData.homebutton_pushed)break;
		//コマンド受け取り
		for(i = 0;i < DXP_BUILDOPTION_SOUNDHANDLE_MAX;++i)
		{
			if(!dxpSoundArray[i].used)continue;
			if(dxpSoundArray[i].soundDataType != DX_SOUNDDATATYPE_MEMNOPRESS)continue;
			switch(dxpSoundArray[i].cmd)
			{
			case DXP_SOUNDCMD_NONE:
				continue;
			case DXP_SOUNDCMD_PLAY:
				for(j = 0;j < PSP_AUDIO_CHANNEL_MAX;++j)
				{
					if(handle[j] < 0)
					{
						channel[j] = sceAudioChReserve(PSP_AUDIO_NEXT_CHANNEL,dxpSoundArray[i].avContext.outSampleNum,PSP_AUDIO_FORMAT_STEREO);
						if(channel[j] < 0)break;
						handle[j] = i;
						pos[j] = 0;
						playtype[j] = dxpSoundArray[i].memnopress.cmdplaytype;
						break;
					}
				}
				dxpSoundArray[i].cmd = DXP_SOUNDCMD_NONE;
				break;
			case DXP_SOUNDCMD_STOP:
			case DXP_SOUNDCMD_EXIT:
			default:
				for(j = 0;j < PSP_AUDIO_CHANNEL_MAX;++j)
				{
					if(handle[j] == i)
					{
						if(channel[j] >= 0)sceAudioChRelease(channel[j]);
						channel[j] = -1;
						--dxpSoundArray[handle[j]].playing;
						handle[j] = -1;
					}
				}
				dxpSoundArray[i].cmd = DXP_SOUNDCMD_NONE;
				break;			
			}
		}

		//ループ等制御
		for(i = 0;i < PSP_AUDIO_CHANNEL_MAX;++i)
		{
			if(handle[i] < 0)continue;

			if(pos[i] > dxpSoundArray[handle[i]].memnopress.length)
			{
				if(playtype[i] == DX_PLAYTYPE_LOOP)
				{
					pos[i] = dxpSoundArray[handle[i]].loopResumePos;
				}
				else
				{
					if(channel[i] >= 0)
						sceAudioChRelease(channel[i]);
					channel[i] = -1;
					handle[i] = -1;
					--dxpSoundArray[handle[i]].playing;
					continue;
				}
			}
		//再生バッファ監視
		//再生
			if(sceAudioGetChannelRestLen(channel[i]) <= 0)
			{
				sceAudioOutputPanned(channel[i],
					PSP_AUDIO_VOLUME_MAX * (dxpSoundArray[handle[i]].pan > 0 ? 1.0f - dxpSoundArray[handle[i]].pan / 10000.0f : 1.0f) * dxpSoundArray[handle[i]].volume / 255.0f,
					PSP_AUDIO_VOLUME_MAX * (dxpSoundArray[handle[i]].pan < 0 ? 1.0f + dxpSoundArray[handle[i]].pan / 10000.0f : 1.0f) * dxpSoundArray[handle[i]].volume / 255.0f,
					dxpSoundArray[handle[i]].memnopress.pcmBuf + pos[i]);
				pos[i] += dxpSoundArray[handle[i]].avContext.outSampleNum;
			}
		}
	}
	for(i = 0;i < PSP_AUDIO_CHANNEL_MAX;++i)
	{
		if(channel[i] >= 0)sceAudioChRelease(channel[i]);
		if(handle[i] >= 0)
		{
			dxpSoundArray[handle[i]].playing = 0;
		}
	}
	sceKernelExitDeleteThread(0);
	return 0;
}
