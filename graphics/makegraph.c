#include "../graphics.h"
#include <malloc.h>
int MakeGraph(int xsize,int ysize)
{
	GUINITCHECK;
	if(xsize <= 0 || ysize <= 0 || xsize > 512 || ysize > 512)return -1;
	DXPGRAPHICSHANDLE *gptr = dxpGraphicsCreateGraphicHandle();
	DXPTEXTURE3 *texptr = dxpGraphicsCreateTexture();
	int size = dxpGraphicsCalcTexSize(xsize,ysize,GU_PSM_8888);
	void *texraw = memalign(16,size);
	if(!gptr || !texptr || !texraw)
	{
		dxpGraphicsReleseGraphicHandle(gptr);
		dxpGraphicsReleseTexture(texptr);
		free(texraw);
		return -1;
	}
	gptr->tex = texptr;
	texptr->texdata = texraw;
	gptr->u0 = 0;
	gptr->v0 = 0;
	gptr->u1 = xsize;
	gptr->v1 = ysize;
	texptr->colorkey = dxpGraphicsData.colorkey;
	texptr->height = dxpN_2(ysize);
	if(texptr->height < 8)texptr->height = 8;
	texptr->width = dxpN_2(xsize);
	texptr->pitch = ((texptr->width + 3) >> 2) << 2;
	texptr->psm = GU_PSM_8888;
	++texptr->refcount;
	texptr->reloadflag = 1;
	texptr->size2_nflag = texptr->width == xsize && texptr->height == ysize ? 1 : 0;
	texptr->umax = xsize;
	texptr->vmax = ysize;
	return gptr->handle;
}