#include "dxppng.h"
#include "../graphics.h"
#include <malloc.h>
#include <pspdebug.h>

static DXPTEXTURE3* LoadPngImage(const char *FileName);

int LoadGraph(const char *FileName)
{
	GUINITCHECK;
	DXPGRAPHICSHANDLE *gptr = NULL;
	DXPTEXTURE3 *texptr = NULL;
	gptr = dxpGraphicsCreateGraphicHandle();
	if(!gptr)return -1;

	texptr = LoadPngImage(FileName);
	if(!texptr)
	{
		dxpGraphicsReleseGraphicHandle(gptr);
		return -1;
	}
	
	gptr->tex = texptr;
	++texptr->refcount;
	gptr->u0 = gptr->v0 = 0;
	gptr->u1 = texptr->umax;
	gptr->v1 = texptr->vmax;
	if(dxpGraphicsData.create_vram_graph)MoveGraphToVRAM(gptr->handle);
	if(dxpGraphicsData.create_swizzled_graph)SwizzleGraph(gptr->handle);
//	AppLogAdd("fh=%d;uv::%d,%d,%d,%d",gptr->handle,gptr->u0,gptr->u1,gptr->v0,gptr->v1);
	sceKernelDcacheWritebackAll();
	return gptr->handle;
}

static DXPTEXTURE3* LoadPngImage(const char *FileName)
{
	u32 filesize;
	int fp = -1;
	DXPPNG png;
	DXPPNG_PARAMS params;
	void *buf = NULL;
	DXPTEXTURE3 *texptr = NULL;
	filesize = FileRead_size(FileName);
	if(!filesize)goto err;
	buf = malloc(filesize);
	if(!buf)goto err;
	texptr = dxpGraphicsCreateTexture();
	if(!texptr)goto err;
	fp = FileRead_open(FileName,0);
	if(fp == -1)goto err;
	FileRead_read(buf,filesize,fp);
	FileRead_close(fp);
	params.funcs.pmalloc = malloc;
	params.funcs.pmemalign = memalign;
	params.funcs.pfree = free;
	params.mode = DXPPNG_MODE_GPU;
	params.src = buf;
	params.srcLength = filesize;
	if(dxppng_decode(&params,&png) == -1)goto err;
	texptr->alphabit = png.alpha ? 1 : 0;
	texptr->colorkey = dxpGraphicsData.colorkey;
	texptr->width = png.widthN2;
	texptr->height = png.heightN2;
	texptr->pitch = png.pitch;
	texptr->ppalette = png.clut;
	texptr->psm = png.psm;
	texptr->reloadflag = 1;
	texptr->size2_nflag = (png.height == png.heightN2 && png.width == png.widthN2 ? 1 : 0);
	texptr->swizzledflag = 0;
	texptr->texdata = png.raw;
	texptr->texvram = 0;
	texptr->umax = png.width;
	texptr->vmax = png.height;
	return texptr;
err:
	FileRead_close(fp);
	free(buf);
	dxpGraphicsReleseTexture(texptr);
	return NULL;
}
