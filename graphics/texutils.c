#include <valloc.h>
#include <malloc.h>
#include <string.h>
#include <pspgu.h>
#include"../graphics.h"



DXPTEXTURE3* dxpGraphicsCreateTexture()
{
	u32 i;
	DXPTEXTURE3 *texptr;
	for(i = 0;i < DXP_BUILDOPTION_TEXTURE_MAXNUM;++i)
		if(dxpGraphicsData.texarray[i] == 0)break;
	if(i == DXP_BUILDOPTION_TEXTURE_MAXNUM)return NULL;
	texptr = (DXPTEXTURE3*)malloc(sizeof(DXPTEXTURE3));
	if(!texptr)return NULL;
	memset(texptr,0,sizeof(DXPTEXTURE3));
	texptr->thisptrptr = &dxpGraphicsData.texarray[i];
	dxpGraphicsData.texarray[i] = texptr;
	return texptr;
}

int dxpGraphicsReleseTexture(DXPTEXTURE3 *texptr)
{
	if(!texptr)return -1;
	if(texptr->refcount)return -1;
	free(texptr->ppalette);
	free(texptr->texdata);
	vfree(texptr->texvram);
	*texptr->thisptrptr = NULL;
	free(texptr);
	return 0;
}

DXPGRAPHICSHANDLE* dxpGraphicsCreateGraphicHandle()
{
	u32 i;
	DXPGRAPHICSHANDLE *gptr;
	for(i = 0;i < DXP_BUILDOPTION_GHANDLE_MAXNUM;++i)
		if(!dxpGraphicsData.grapharray[i])break;
	if(i == DXP_BUILDOPTION_GHANDLE_MAXNUM)return NULL;
	gptr = (DXPGRAPHICSHANDLE*)malloc(sizeof(DXPGRAPHICSHANDLE));
	if(!gptr)return NULL;
	memset(gptr,0,sizeof(DXPGRAPHICSHANDLE));
	gptr->handle = (int)i;
	dxpGraphicsData.grapharray[i] = gptr;
	return gptr;
}

int dxpGraphicsReleseGraphicHandle(DXPGRAPHICSHANDLE* gptr)
{
	if(!gptr)return -1;
	if(gptr->tex)
	{
		--gptr->tex->refcount;
		dxpGraphicsReleseTexture(gptr->tex);
	}
	dxpGraphicsData.grapharray[gptr->handle] = NULL;
	free(gptr);
	return 0;
}

int dxpGraphicsCalcTexSize(int width,int height,int psm)
{
	height = dxpN_2(height);
	switch(psm)
	{
	case GU_PSM_4444:
	case GU_PSM_5551:
	case GU_PSM_5650:
	case GU_PSM_T16:
		width = ((width + 7) >> 3) << 3;
		return width * height * 2;
	case GU_PSM_8888:
	case GU_PSM_T32:
		width = ((width + 3) >> 2) << 2;
		return width * height * 4;
	case GU_PSM_DXT1:
	case GU_PSM_T4:
		width = ((width + 31) >> 5) << 5;
		return width * height / 2;
	case GU_PSM_DXT3:
	case GU_PSM_DXT5:
	case GU_PSM_T8:
		width = ((width + 15) >> 4) << 4;
		return width * height;
	}
	return 0;
}

static void dxpGraphicsSwizzleFast(u8* out, const u8* in, unsigned int width, unsigned int height)
{
   unsigned int blockx, blocky;
   unsigned int j;
 
   unsigned int width_blocks = (width / 16);
   unsigned int height_blocks = (height / 8);
 
   unsigned int src_pitch = (width-16)/4;
   unsigned int src_row = width * 8;
 
   const u8* ysrc = in;
   u32* dst = (u32*)out;
 
   for (blocky = 0; blocky < height_blocks; ++blocky)
   {
      const u8* xsrc = ysrc;
      for (blockx = 0; blockx < width_blocks; ++blockx)
      {
         const u32* src = (u32*)xsrc;
         for (j = 0; j < 8; ++j)//16byte幅で高さ8の情報を線形に転送
         {
            *(dst++) = *(src++);
            *(dst++) = *(src++);
            *(dst++) = *(src++);
            *(dst++) = *(src++);
            src += src_pitch;
         }
         xsrc += 16;
     }
     ysrc += src_row;
   }
}

static void dxpGraphicsUnswizzleFast(u8* out, const u8* in, unsigned int width, unsigned int height)
{
   unsigned int blockx, blocky;
   unsigned int j;
 
   unsigned int width_blocks = (width / 16);
   unsigned int height_blocks = (height / 8);
 
   unsigned int src_pitch = (width-16)/4;
   unsigned int src_row = width * 8;
 
   u8* ysrc = out;
   u32* dst = (u32*)in;
 
   for (blocky = 0; blocky < height_blocks; ++blocky)
   {
      u8* xsrc = ysrc;
      for (blockx = 0; blockx < width_blocks; ++blockx)
      {
         u32* src = (u32*)xsrc;
         for (j = 0; j < 8; ++j)
         {
            *(src++) = *(dst++);
            *(src++) = *(dst++);
            *(src++) = *(dst++);
            *(src++) = *(dst++);
            src += src_pitch;
         }
         xsrc += 16;
     }
     ysrc += src_row;
   }
}

int SwizzleGraph(int gh)
{
	GUINITCHECK;
	GUSYNC;
	u32 size;
	DXPTEXTURE3 *texptr = dxpGraphHandle2TexPtr(gh);
	if(!texptr)return -1;
	if(texptr->swizzledflag)return 0;
	size = dxpGraphicsCalcTexSize(texptr->pitch,texptr->height,texptr->psm);
	if(texptr->texvram)
	{
		if(!texptr->texdata)return -1;
		dxpGraphicsSwizzleFast(texptr->texdata,texptr->texvram,PSM2BYTEX2(texptr->psm) * texptr->pitch / 2,texptr->height);
		memcpy(texptr->texvram,texptr->texdata,size);
	}
	else
	{
		void *buf = malloc(size);
		if(!buf)return -1;
		dxpGraphicsSwizzleFast(buf,texptr->texdata,PSM2BYTEX2(texptr->psm) * texptr->pitch / 2,texptr->height);
		memcpy(texptr->texdata,buf,size);
		free(buf);
	}
	sceKernelDcacheWritebackAll();
	texptr->swizzledflag = 1;
	texptr->reloadflag = 1;
	return 0;
}

int UnswizzleGraph(int gh)
{
	GUINITCHECK;
	GUSYNC;
	u32 size;
	DXPTEXTURE3 *texptr = dxpGraphHandle2TexPtr(gh);
	if(!texptr)return -1;
	if(!texptr->swizzledflag)return 0;
	size = dxpGraphicsCalcTexSize(texptr->pitch,texptr->height,texptr->psm);
	if(texptr->texvram)
	{
		if(!texptr->texdata)return -1;
		dxpGraphicsUnswizzleFast(texptr->texdata,texptr->texvram,PSM2BYTEX2(texptr->psm) * texptr->pitch / 2,texptr->height);
		memcpy(texptr->texvram,texptr->texdata,size);
	}
	else
	{
		void *buf = malloc(size);
		if(!buf)return -1;
		dxpGraphicsUnswizzleFast(buf,texptr->texdata,PSM2BYTEX2(texptr->psm) * texptr->pitch / 2,texptr->height);
		memcpy(texptr->texdata,buf,size);
		free(buf);
	}
	sceKernelDcacheWritebackAll();
	texptr->swizzledflag = 0;
	texptr->reloadflag = 1;
	return 0;
}

int MoveGraphToVRAM(int gh)
{
	GUINITCHECK;
	GUSYNC;
	DXPTEXTURE3 *texptr;
	int size;
	texptr = dxpGraphHandle2TexPtr(gh);
	if(!texptr)return -1;
	if(texptr->texvram)return 0;
	size = dxpGraphicsCalcTexSize(texptr->pitch,texptr->height,texptr->psm);
	texptr->texvram = valloc(size);
	if(!texptr->texvram)return -1;
	memcpy(texptr->texvram,texptr->texdata,size);
	texptr->reloadflag = 1;
	sceKernelDcacheWritebackAll();
	return 0;
}

int MoveGraphToDDR(int gh)
{
	GUINITCHECK;
	GUSYNC;
//vramあり→vram上のデータを消す
//vramなし→なにもしない
	DXPTEXTURE3 *texptr;
	int size;
	texptr = dxpGraphHandle2TexPtr(gh);
	if(!texptr)return -1;
	if(!texptr->texvram)return 0;
	size = dxpGraphicsCalcTexSize(texptr->pitch,texptr->height,texptr->psm);
	memcpy(texptr->texdata,vCPUPointer(texptr->texvram),size);
	vfree(texptr->texvram);
	texptr->texvram = NULL;
	texptr->reloadflag = 1;
	sceKernelDcacheWritebackAll();
	return 0;
}

//int UpdateGraphToDDR(int gh)
//{
////vramなし→なにもしない
////vramあり→vramのデータをDDRにコピー
//	return -1;
//}
//
//int UpdateGraphToVRAM(int gh)
//{
////vramなし→なにもしない
////vramあり→DDRのデータをvramにコピー
//	return -1;
//}
