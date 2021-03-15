#include "../sound.h"
#include <string.h>
#include <stdio.h>
#include <psputility.h>
#include <pspaudiocodec.h>
#include <malloc.h>
#include "../safealloc.h"

int dxpSoundGetID3v1Size(int fh);
int dxpSoundGetID3v2Size(int fh);
static int dxpSoundMp3CheckFrameHeader(u8 *buf);
static int dxpSoundMp3GetSampleRate(u8 *buf);

int dxpSoundMp3Init(DXPAVCONTEXT *av);
int dxpSoundMp3GetSampleLength(DXPAVCONTEXT *av);
int dxpSoundMp3Seek(DXPAVCONTEXT *av,int sample);
int dxpSoundMp3Decode(DXPAVCONTEXT *av);
int dxpSoundMp3End(DXPAVCONTEXT *av);

int dxpSoundGetID3v1Size(int fh)
{
	int pos,ret = 0;
	char buf[3];
	if(fh < 0)return -1;
	pos = FileRead_tell(fh);
	FileRead_seek(fh,128,SEEK_END);
	FileRead_read(buf,3,fh);
	if(!strncmp(buf,"TAG",3))ret = 128;
	FileRead_seek(fh,pos,SEEK_SET);
	return ret;
}

int dxpSoundGetID3v2Size(int fh)
{
	char header[10];
	int pos,ret = 0;
	if(fh < 0)return -1;
	pos = FileRead_tell(fh);
	FileRead_seek(fh,0,SEEK_SET);
	FileRead_read(header,10,fh);
	if(!strncmp(header,"ID3",3) || !strncmp(header,"ea3",3))
	{
		u32 tagsize;
		tagsize = (u8)header[6];
		tagsize <<= 7;
		tagsize |= (u8)header[7];
		tagsize <<= 7;
		tagsize |= (u8)header[8];
		tagsize <<= 7;
		tagsize |= (u8)header[9];

		if(header[5] & 0x10)
			tagsize += 10;
		ret = tagsize + 10;
	}
	FileRead_seek(fh,pos,SEEK_SET);
	return ret;
}

int dxpSoundMp3CheckFrameHeader(u8 *buf)
{
	u32 header,version;
	const int bitrates[2][15]
	= {
		{0,	32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320 },	//MPEG1
		{0,	8,	16,	24,	32,	40,	48,	56,	64,	 80,  96,  112,	128, 144, 160 },	//MPEG2
	};
	const int samplerates[4] = {44100,48000,32000,2};

	if(buf[0] == 'T' && buf[1] == 'A' && buf[2] == 'G')return -1;

	header = buf[0];
	header = (header << 8) | buf[1];
	header = (header << 8) | buf[2];
	header = (header << 8) | buf[3];
	if((header & 0xFFE00000) != 0xFFE00000)return -1;

	version = (header & 0x180000) >> 19;
	switch(version)
	{
	case 3:
		version = 0;
		break;
	case 2:
		version = 1;
		break;
	default:
		return -1;
	}
	return 144000 * bitrates[version][(header & 0xf000) >> 12] / (samplerates[(header & 0xc00) >> 10] / (version + 1)) + ((header & 0x200) >> 9);
}

int dxpSoundMp3GetSampleRate(u8 *buf)
{
	u32 header;
	const int samplerates[4] = {44100,48000,32000,2};

	header = buf[0];
	header = (header << 8) | buf[1];
	header = (header << 8) | buf[2];
	header = (header << 8) | buf[3];
	return samplerates[(header & 0xc00) >> 10];
}

int dxpSoundMp3Init(DXPAVCONTEXT *av)
{
	u8 buf[4];
	int status;
	av->mp3.id3v1Pos = av->fileSize - dxpSoundGetID3v1Size(av->fileHandle);
	av->mp3.id3v2Pos = dxpSoundGetID3v2Size(av->fileHandle);
	FileRead_seek(av->fileHandle,av->mp3.id3v2Pos,SEEK_SET);
	if(FileRead_read(buf,4,av->fileHandle) != 4)return -1;
	status = dxpSoundMp3CheckFrameHeader(buf);
	if(status == -1)return -1;
	av->format = DXP_SOUNDFMT_MP3;
	av->sampleRate = dxpSoundMp3GetSampleRate(buf);

	FileRead_seek(av->fileHandle,av->mp3.id3v2Pos,SEEK_SET);
	av->mp3.avBuf = dxpSafeAlloc(sizeof(DXPAVCODEC_BUFFER));
	if(!av->mp3.avBuf)return -1;
	memset(av->mp3.avBuf,0,sizeof(DXPAVCODEC_BUFFER));
	status = sceAudiocodecCheckNeedMem((unsigned long*)av->mp3.avBuf,PSP_CODEC_MP3);
	if(status < 0)
	{
		dxpSafeFree(av->mp3.avBuf);
		return -1;
	}
	status = sceAudiocodecGetEDRAM((unsigned long*)av->mp3.avBuf,PSP_CODEC_MP3);
	if(status < 0)
	{
		dxpSafeFree(av->mp3.avBuf);
		return -1;
	}
	status = sceAudiocodecInit((unsigned long*)av->mp3.avBuf,PSP_CODEC_MP3);
	if(status < 0)
	{
		sceAudiocodecReleaseEDRAM((unsigned long*)av->mp3.avBuf);
		dxpSafeFree(av->mp3.avBuf);
		return -1;
	}
	av->mp3.mp3Buf = NULL;
	av->mp3.mp3BufSize = 0;
	av->nextPos = 0;
	av->outSampleNum = 1152;
	return 0;
}

int dxpSoundMp3Seek(DXPAVCONTEXT *av,int sample)
{
	int frame = sample / 1152,i;
	int frameLen;
	u8 buf[4];
	FileRead_seek(av->fileHandle,av->mp3.id3v2Pos,SEEK_SET);
	for(i = 0;i < frame;++i)
	{
		FileRead_read(buf,4,av->fileHandle);
		frameLen = dxpSoundMp3CheckFrameHeader(buf);
		if(frameLen == -1)return -1;
		FileRead_seek(av->fileHandle,frameLen - 4,SEEK_CUR);
	}
	av->nextPos = frame * 1152;
	return 0;
}

int dxpSoundMp3Decode(DXPAVCONTEXT *av)
{
	int frameLen;
	u8 headerBuf[4] = {0,0,0,0};
	int status;
	if(av->format != DXP_SOUNDFMT_MP3)return -1;
	FileRead_read(headerBuf,4,av->fileHandle);
	if(headerBuf[0] == 'T' && headerBuf[1] == 'A' && headerBuf[2] == 'G')return -1;
	frameLen = dxpSoundMp3CheckFrameHeader(headerBuf);
	if(frameLen < 0)return -1;
	if(av->mp3.mp3BufSize < frameLen)
	{
		dxpSafeFree(av->mp3.mp3Buf);
		av->mp3.mp3Buf = dxpSafeAlloc(frameLen);
		if(!av->mp3.mp3Buf)
		{
			av->mp3.mp3BufSize = 0;
			return -1;
		}
		av->mp3.mp3BufSize = frameLen;
	}
	if(FileRead_read(av->mp3.mp3Buf + 4,frameLen - 4,av->fileHandle) != frameLen - 4)return -1;
	memcpy(av->mp3.mp3Buf,headerBuf,4);
	av->mp3.avBuf->datIn = av->mp3.mp3Buf;
	av->mp3.avBuf->decodeByte = 1152 * 2 * 2;
	av->mp3.avBuf->frameSize0 =
		av->mp3.avBuf->frameSize1 = frameLen;
	av->mp3.avBuf->pcmOut = av->pcmOut;
	status = sceAudiocodecDecode((unsigned long*)av->mp3.avBuf,PSP_CODEC_MP3);
	if(status < 0)return -1;
	av->nextPos += 1152;
	return 0;
}

int dxpSoundMp3End(DXPAVCONTEXT *av)
{
	if(av->format != DXP_SOUNDFMT_MP3)return -1;
	sceAudiocodecReleaseEDRAM((unsigned long*)av->mp3.avBuf);
	dxpSafeFree(av->mp3.avBuf);
	dxpSafeFree(av->mp3.mp3Buf);
	return 0;
}

int dxpSoundMp3GetSampleLength(DXPAVCONTEXT *av)
{
	u8 buf[4];
	if(av->format != DXP_SOUNDFMT_MP3)return -1;
	int pos = FileRead_tell(av->fileHandle);
	FileRead_seek(av->fileHandle,av->mp3.id3v2Pos,SEEK_SET);
	int length,status;
	for(length = 0;1;++length)
	{
		if(FileRead_read(buf,4,av->fileHandle) != 4)break;
		status = dxpSoundMp3CheckFrameHeader(buf);
		if(status < 0)break;
		FileRead_seek(av->fileHandle,status - 4,SEEK_CUR);
	}
	FileRead_seek(av->fileHandle,pos,SEEK_SET);
	return length * 1152;
}