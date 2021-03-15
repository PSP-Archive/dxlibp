#include "../graphics.h"

int DrawRectGraph(int destx,int desty,int srcx,int srcy,int width,int height,int gh,int trans,int turn)
{
	DXPGRAPHICSHANDLE* gptr;
	GHANDLE2GPTR(gptr,gh);
	if(srcx < 0 || srcy < 0 || srcx + width > gptr->u1 - gptr->u0 || srcy + height > gptr->v1 - gptr->v0)return -1;
	if(dxpGraphicsSetup2DTex(gptr->tex,trans) < 0)return -1;
	int x1 = destx,x2 = destx + width,y1 = desty,y3 = desty + height;
	int x3 = x2,x4 = x1,y2 = y1,y4 = y3;
	int sw = dxpPsm2SliceSize[gptr->tex->psm][0];
	int sh = dxpPsm2SliceSize[gptr->tex->psm][1];
	int swn = (width + sw - 1) / sw;
	int shn = (height + sh - 1) / sh;
	DXP_FVF_2DTEX *buf = (DXP_FVF_2DTEX*)dxpGuGetMemory(sizeof(DXP_FVF_2DTEX) * (swn * 2 + 2) * shn);
	DXP_FVF_2DTEX *vtxbuf = buf;
	int u[2],v[2];
	float u1_0 = (width),v1_0 = (height);
	u1_0 = 1.0f / u1_0;
	v1_0 = 1.0f / v1_0;

	v[0] = gptr->v0 + srcy;
	int u0 = gptr->u0 + srcx,v0 = gptr->v0 + srcy;
	int u1 = u0 + width,v1 = v0 + height;
	while(v[0] < gptr->v0 + srcy + height)
	{
		u[0] = gptr->u0 + srcx;
		v[1] = MIN(v[0] + sh,gptr->v0 + srcy + height);
		buf = vtxbuf;

		vtxbuf->u = u[0];
		vtxbuf->v = v[0];
		vtxbuf->x = (x1 + (u[0] - u0) * (x2 - x1) * u1_0) * ((v1 - v[0]) * v1_0) +
					(x4 + (u[0] - u0) * (x3 - x4) * u1_0) * ((v[0] - v0) * v1_0);
		vtxbuf->y = (y1 + (v[0] - v0) * (y4 - y1) * v1_0) * ((u1 - u[0]) * u1_0) +
					(y2 + (v[0] - v0) * (y3 - y2) * v1_0) * ((u[0] - u0) * u1_0);
		vtxbuf->z = dxpGraphicsData.z_2d;
		++vtxbuf;

		vtxbuf->u = u[0];
		vtxbuf->v = v[1];
		vtxbuf->x = (x1 + (u[0] - u0) * (x2 - x1) * u1_0) * ((v1 - v[1]) * v1_0) +
					(x4 + (u[0] - u0) * (x3 - x4) * u1_0) * ((v[1] - v0) * v1_0);
		vtxbuf->y = (y1 + (v[1] - v0) * (y4 - y1) * v1_0) * ((u1 - u[0]) * u1_0) +
					(y2 + (v[1] - v0) * (y3 - y2) * v1_0) * ((u[0] - u0) * u1_0);
		vtxbuf->z = dxpGraphicsData.z_2d;
		++vtxbuf;
		while(u[0] < gptr->u0 + srcx + width)
		{
			u[1] = MIN(u[0] + sw,gptr->u0 + srcx + width);			

			vtxbuf->u = u[1];
			vtxbuf->v = v[0];
			vtxbuf->x = (x1 + (u[1] - u0) * (x2 - x1) * u1_0) * ((v1 - v[0]) * v1_0) +
						(x4 + (u[1] - u0) * (x3 - x4) * u1_0) * ((v[0] - v0) * v1_0);
			vtxbuf->y = (y1 + (v[0] - v0) * (y4 - y1) * v1_0) * ((u1 - u[1]) * u1_0) +
						(y2 + (v[0] - v0) * (y3 - y2) * v1_0) * ((u[1] - u0) * u1_0);
			vtxbuf->z = dxpGraphicsData.z_2d;
			++vtxbuf;

			vtxbuf->u = u[1];
			vtxbuf->v = v[1];
			vtxbuf->x = (x1 + (u[1] - u0) * (x2 - x1) * u1_0) * ((v1 - v[1]) * v1_0) +
						(x4 + (u[1] - u0) * (x3 - x4) * u1_0) * ((v[1] - v0) * v1_0);
			vtxbuf->y = (y1 + (v[1] - v0) * (y4 - y1) * v1_0) * ((u1 - u[1]) * u1_0) +
						(y2 + (v[1] - v0) * (y3 - y2) * v1_0) * ((u[1] - u0) * u1_0);
			vtxbuf->z = dxpGraphicsData.z_2d;
			++vtxbuf;

			u[0] += sw;
		}
		sceGuDrawArray(GU_TRIANGLE_STRIP,DXP_VTYPE_2DTEX | GU_TRANSFORM_2D,2 * swn + 2,0,buf);
		v[0] += sh;
	}
	return 0;
}
