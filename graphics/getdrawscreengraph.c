#include "../graphics.h"
int GetDrawScreenGraph(int x1,int y1,int x2,int y2,int gh)
{
	GUINITCHECK;
	GUSTART;
	DXPGRAPHICSHANDLE *gptr;
	GHANDLE2GPTR(gptr,gh);
	if(x2 - x1 != gptr->u1 - gptr->u0 || y2 - y1 != gptr->v1 - gptr->v0)return -1;

	DXPTEXTURE3 *dest,*src;
	dest = gptr->tex;
	if(dxpGraphicsData.rendertarget == NULL)
		src = dxpGraphicsData.displaybuffer_back;
	else
	{
		if(dxpGraphicsData.rendertarget == &dxpGraphicsData.displaybuffer[0] || dxpGraphicsData.rendertarget == &dxpGraphicsData.displaybuffer[1])
			src = dxpGraphicsData.displaybuffer_back;
		else
			src = dxpGraphicsData.rendertarget;
	}

	switch(dest->psm)
	{
	case GU_PSM_8888:
	case GU_PSM_4444:
	case GU_PSM_5551:
	case GU_PSM_5650:
		break;
	default:
		return -1;
	}
	if(dest->swizzledflag)return -1;
	if(!dest->texvram)return -1;

	DXP_FVF_2DTEX* vertex = (DXP_FVF_2DTEX*)dxpGuGetMemory(sizeof(DXP_FVF_2DTEX) * 2);
	if(!vertex)return -1;


	//レンダーターゲット一時変更
	sceGuTexFlush();
	sceGuDrawBufferList(dest->psm,dest->texvram,dest->pitch);
	sceGuOffset(2048 - (dest->width/2),2048 - (dest->height/2));
	sceGuViewport(2048,2048,dest->width,dest->height);

	//描画
	GUDISABLE(GU_BLEND);
	GUDISABLE(GU_DEPTH_TEST);
	GUENABLE(GU_TEXTURE_2D);
	GUDISABLE(GU_COLOR_TEST);
	sceGuTexMode(src->psm,0,0,src->swizzledflag);
	sceGuTexImage(0,src->width,src->height,src->pitch,src->texvram);
	sceGuColor(0xffffffff);

	sceGuTexFunc(GU_TFX_REPLACE,GU_TCC_RGB);

	vertex[0].u = x1;
	vertex[0].v = y1;
	vertex[0].x = 0;
	vertex[0].y = 0;
	vertex[0].z = 0;

	vertex[1].u = x2;
	vertex[1].v = y2;
	vertex[1].x = x2 - x1;
	vertex[1].y = y2 - y1;
	vertex[1].z = 0;

	sceGuDrawArray(GU_SPRITES,DXP_VTYPE_2DTEX | GU_TRANSFORM_2D,2,NULL,vertex);

	//設定を元に戻す
	sceGuTexFlush();
	sceGuDrawBufferList(dxpGraphicsData.rendertarget->psm,dxpGraphicsData.rendertarget->texvram,dxpGraphicsData.rendertarget->pitch);
	sceGuOffset(2048 - (dxpGraphicsData.rendertarget->width/2),2048 - (dxpGraphicsData.rendertarget->height/2));
	sceGuViewport(2048,2048,dxpGraphicsData.rendertarget->width,dxpGraphicsData.rendertarget->height);
	dxpGraphicsData.forceupdate = 1;
	dxpGraphicsData.drawstate = DXP_DRAWSTATE_EITHER;
	dxpGraphicsData.texture = NULL;

	return 0;
}
