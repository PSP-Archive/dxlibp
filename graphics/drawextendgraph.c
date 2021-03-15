#include "../graphics.h"
int DrawExtendGraph(int x1,int y1,int x2,int y2,int gh,int trans)
{
	if((x2 < x1 || y2 < y1) && !(x2 < x1 && y2 < y1))return DrawModiGraph(x1,y1,x2,y1,x2,y2,x1,y2,gh,trans);//X方向反転画像がうまく動作しないので（GU_SPRITESを使ったのが原因と思われる）強引に回避
	DXPGRAPHICSHANDLE* gptr;
	GHANDLE2GPTR(gptr,gh)
	if(dxpGraphicsSetup2DTex(gptr->tex,trans) < 0)return -1;
	int sw = dxpPsm2SliceSize[gptr->tex->psm][0];
	int sh = dxpPsm2SliceSize[gptr->tex->psm][1];
	int u[2],v[2];
	int count = ((gptr->u1 - gptr->u0 + sw - 1) / sw) * ((gptr->v1 - gptr->v0 + sh - 1) / sh);
	DXP_FVF_2DTEX *vtxbuf = (DXP_FVF_2DTEX*)dxpGuGetMemory(sizeof(DXP_FVF_2DTEX) * 2 * count);
	float uscale = (float)(x2 - x1) / (float)(gptr->u1 - gptr->u0);
	float vscale = (float)(y2 - y1) / (float)(gptr->v1 - gptr->v0);
	u[0] = gptr->u0;
	while(u[0] < gptr->u1)
	{
		u[1] = MIN(u[0] + sw,gptr->u1);
		v[0] = gptr->v0;
		while(v[0] < gptr->v1)
		{
			v[1] = MIN(v[0] + sh,gptr->v1);
			vtxbuf->u = u[0];
			vtxbuf->v = v[0];
			vtxbuf->x = x1 + uscale * (u[0] - gptr->u0);
			vtxbuf->y = y1 + vscale * (v[0] - gptr->v0);
			vtxbuf->z = dxpGraphicsData.z_2d;
			++vtxbuf;
			vtxbuf->u = u[1];
			vtxbuf->v = v[1];
			vtxbuf->x = x1 + uscale * (u[1] - gptr->u0);
			vtxbuf->y = y1 + vscale * (v[1] - gptr->v0);
			vtxbuf->z = dxpGraphicsData.z_2d;
			++vtxbuf;
			v[0] += sh;
		}
		u[0] += sw;
	}
	vtxbuf -= count * 2;
	sceGuDrawArray(GU_SPRITES,DXP_VTYPE_2DTEX | GU_TRANSFORM_2D,2 * count,NULL,vtxbuf);
	return 0;
}
