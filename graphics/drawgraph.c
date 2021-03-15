#include "../graphics.h"

int DrawGraph(int x,int y,int gh,int trans)
{
	GUINITCHECK;
	DXPGRAPHICSHANDLE *gptr;
	GHANDLE2GPTR(gptr,gh);
	return DrawExtendGraph(x,y,x + (gptr->u1 - gptr->u0),y + (gptr->v1 - gptr->v0),gh,trans);
}
