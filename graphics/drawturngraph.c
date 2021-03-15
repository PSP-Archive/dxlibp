#include "../graphics.h"

int DrawTurnGraph(int x,int y,int gh,int trans)
{
	GUINITCHECK;
	DXPGRAPHICSHANDLE *gptr;
	GHANDLE2GPTR(gptr,gh);
	return DrawExtendGraph(x + (gptr->u1 - gptr->u0),y,x,y + (gptr->v1 - gptr->v0),gh,trans);
}
