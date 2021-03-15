#include "../graphics.h"

int SetDrawScreen(int ghandle)
{
	GUINITCHECK;
	GUSTART;
	DXPTEXTURE3 *old = dxpGraphicsData.rendertarget;
	if(ghandle == -1)return -1;
	if(ghandle == DX_SCREEN_BACK)
	{
		dxpGraphicsData.rendertarget = dxpGraphicsData.displaybuffer_back;
	}
	else
	{
		DXPTEXTURE3 *tmp;
		DXPGRAPHICSHANDLE *gptr;
		GHANDLE2GPTR(gptr,ghandle);
		tmp = gptr->tex;
		if(!tmp->texvram || tmp->swizzledflag)return -1;
		if(!(tmp->psm == GU_PSM_4444 || tmp->psm == GU_PSM_5551 || tmp->psm == GU_PSM_5650 || tmp->psm == GU_PSM_8888))return -1;
		dxpGraphicsData.rendertarget = tmp;
		++tmp->refcount;
	}
	sceGuTexFlush();
	sceGuDrawBufferList(dxpGraphicsData.rendertarget->psm,dxpGraphicsData.rendertarget->texvram,dxpGraphicsData.rendertarget->pitch);
	sceGuOffset(2048 - (dxpGraphicsData.rendertarget->width/2),2048 - (dxpGraphicsData.rendertarget->height/2));
	sceGuViewport(2048,2048,dxpGraphicsData.rendertarget->width,dxpGraphicsData.rendertarget->height);
	dxpGraphicsData.forceupdate = 1;
	if(old != NULL && old != &dxpGraphicsData.displaybuffer[0] && old != &dxpGraphicsData.displaybuffer[1])
		--old->refcount;
	return 0;
}
