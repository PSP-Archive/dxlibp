#include "../sound.h"

int dxpSoundCodecInit(DXPSOUNDHANDLE *pHnd)
{
	int status;
	if(!dxpSoundData.init)return -1;
	status = dxpSoundMp3Init(&pHnd->avContext);
	if(!(status < 0))return 0;
	return -1;
}

int dxpSoundCodecGetSampleLength(DXPSOUNDHANDLE *pHnd)
{
	if(!dxpSoundData.init)return -1;
	switch(pHnd->avContext.format)
	{
	case DXP_SOUNDFMT_MP3:
		return dxpSoundMp3GetSampleLength(&pHnd->avContext);
	default:
		return -1;
	}
}

int dxpSoundCodecSeek(DXPSOUNDHANDLE *pHnd,int sample)
{
	if(!dxpSoundData.init)return -1;
	switch(pHnd->avContext.format)
	{
	case DXP_SOUNDFMT_MP3:
		return dxpSoundMp3Seek(&pHnd->avContext,sample);
	default:
		return -1;
	}
}

int dxpSoundCodecDecode(DXPSOUNDHANDLE *pHnd)
{
	if(!dxpSoundData.init)return -1;
	switch(pHnd->avContext.format)
	{
	case DXP_SOUNDFMT_MP3:
		return dxpSoundMp3Decode(&pHnd->avContext);
	default:
		return -1;
	}
}

int dxpSoundCodecEnd(DXPSOUNDHANDLE *pHnd)
{
	if(!dxpSoundData.init)return -1;
	switch(pHnd->avContext.format)
	{
	case DXP_SOUNDFMT_MP3:
		return dxpSoundMp3End(&pHnd->avContext);
	default:
		return -1;
	}
}
