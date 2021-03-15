#include "../graphics.h"

int DerivationGraph(int srcx,int srcy,int width,int height,int gh)
{
	DXPGRAPHICSHANDLE *gptr,*resptr;
	GUINITCHECK;
	GUSYNC;
	GHANDLE2GPTR(gptr,gh);
	if(srcx < 0 || srcy < 0)return -1;
	if(srcx + width > gptr->u1 - gptr->u0 || srcy + height > gptr->v1 - gptr->v0)return -1;
	resptr = dxpGraphicsCreateGraphicHandle();
	if(!resptr)return -1;
	resptr->u0 = gptr->u0 + srcx;
	resptr->v0 = gptr->v0 + srcy;
	resptr->u1 = resptr->u0 + width;
	resptr->v1 = resptr->v0 + height;
	resptr->tex = gptr->tex;
	++gptr->tex->refcount;
	return resptr->handle;
}

