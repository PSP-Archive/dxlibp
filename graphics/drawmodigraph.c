#include "../graphics.h"

int DrawModiGraph( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int gh, int trans )
{
	register int u0,u1,v0,v1;
	DXPGRAPHICSHANDLE* gptr;
	GHANDLE2GPTR(gptr,gh);
	if(dxpGraphicsSetup2DTex(gptr->tex,trans) < 0)return -1;
	int sw = dxpPsm2SliceSize[gptr->tex->psm][0];
	int sh = dxpPsm2SliceSize[gptr->tex->psm][1];
	u0 = gptr->u0;
	v0 = gptr->v0;
	u1 = gptr->u1 - u0;
	v1 = gptr->v1 - v0;
	int swn = (u1 + sw - 1) / sw;
	int shn = (v1 + sh - 1) / sh;
	DXP_FVF_2DTEX *buf = (DXP_FVF_2DTEX*)dxpGuGetMemory(sizeof(DXP_FVF_2DTEX) * (swn * 2 + 2) * shn);
	DXP_FVF_2DTEX *vtxbuf = buf;
	register int x,y;
	register float z = dxpGraphicsData.z_2d;
	__asm__ volatile(

		"lv.s	S030,	%0\n"
		"lv.s	S031,	%4\n"
		"lv.s	S002,	%1\n"
		"lv.s	S003,	%5\n"
		"lv.s	S012,	%2\n"
		"lv.s	S013,	%6\n"
		"lv.s	S022,	%3\n"
		"lv.s	S023,	%7\n"
		"vi2f.p	C030,	C030,	1/2\n"
		"vi2f.t	R002,	R002,	1/2\n"
		"vi2f.t	R003,	R003,	1/2\n"
		"vsub.p	C000,	C002,	C030\n"
		"vsub.p	C010,	C022,	C030\n"
		"vsub.p	C020,	C012,	C022\n"
		"vsub.p	C020,	C020,	C000\n"
		"mtv	%8,	S002\n"
		"mtv	%9,	S012\n"
		"vi2f.p	R002,	R002,	1/2\n"
		"vone.s	S033\n"
		"vrcp.p	R002,	R002\n"
		"mtv	%10,	S100\n"
		"mtv	%11,	S110\n"
		"vi2f.p	R100,	R100,	1/2\n"
		:
		:"m"(x1),"m"(x2),"m"(x3),"m"(x4),
			"m"(y1),"m"(y2),"m"(y3),"m"(y4),
			"r"(u1),"r"(v1),"r"(u0),"r"(v0)
	);

	v0 = gptr->v0;
	while(v0 < gptr->v1)
	{
		u0 = gptr->u0;
		v1 = MIN(v0 + sh,gptr->v1);
		buf = vtxbuf;

		vtxbuf->u = u0;
		vtxbuf->v = v0;
		__asm__ volatile(
			"mtv	%2,	S003\n"
			"mtv	%3,	S013\n"
			"vi2f.p	R003,	R003,	1/2\n"
			"vsub.p	R003,	R003,	R100\n"
			"vmul.p	R003,	R003,	R002\n"
			"vmul.s	S023,	S013,	S003\n"
			"vdot.q	S022,	R003,	R000\n"
			"vdot.q	S032,	R003,	R001\n"
			"vf2in.p	R022,	R022,	1/2\n"
			"mfv	%0,	S022\n"
			"mfv	%1,	S032\n"
			:"=r"(x),"=r"(y)
			:"r"(u0),"r"(v0)
		);
		vtxbuf->x = x;
		vtxbuf->y = y;
		vtxbuf->z = z;
		++vtxbuf;

		vtxbuf->u = u0;
		vtxbuf->v = v1;
		__asm__ volatile(
			"mtv	%2,	S003\n"
			"mtv	%3,	S013\n"
			"vi2f.p	R003,	R003,	1/2\n"
			"vsub.p	R003,	R003,	R100\n"
			"vmul.p	R003,	R003,	R002\n"
			"vmul.s	S023,	S013,	S003\n"
			"vdot.q	S022,	R003,	R000\n"
			"vdot.q	S032,	R003,	R001\n"
			"vf2in.p	R022,	R022,	1/2\n"
			"mfv	%0,	S022\n"
			"mfv	%1,	S032\n"
			:"=r"(x),"=r"(y)
			:"r"(u0),"r"(v1)
		);
		vtxbuf->x = x;
		vtxbuf->y = y;
		vtxbuf->z = z;
		++vtxbuf;
		while(u0 < gptr->u1)
		{
			u1 = MIN(u0 + sw,gptr->u1);			

			vtxbuf->u = u1;
			vtxbuf->v = v0;
			__asm__ volatile(
				"mtv	%2,	S003\n"
				"mtv	%3,	S013\n"
				"vi2f.p	R003,	R003,	1/2\n"
				"vsub.p	R003,	R003,	R100\n"
				"vmul.p	R003,	R003,	R002\n"
				"vmul.s	S023,	S013,	S003\n"
				"vdot.q	S022,	R003,	R000\n"
				"vdot.q	S032,	R003,	R001\n"
				"vf2in.p	R022,	R022,	1/2\n"
				"mfv	%0,	S022\n"
				"mfv	%1,	S032\n"
				:"=r"(x),"=r"(y)
				:"r"(u1),"r"(v0)
			);
			vtxbuf->x = x;
			vtxbuf->y = y;
			vtxbuf->z = z;
			++vtxbuf;

			vtxbuf->u = u1;
			vtxbuf->v = v1;
			__asm__ volatile(
				"mtv	%2,	S003\n"
				"mtv	%3,	S013\n"
				"vi2f.p	R003,	R003,	1/2\n"
				"vsub.p	R003,	R003,	R100\n"
				"vmul.p	R003,	R003,	R002\n"
				"vmul.s	S023,	S013,	S003\n"
				"vdot.q	S022,	R003,	R000\n"
				"vdot.q	S032,	R003,	R001\n"
				"vf2in.p	R022,	R022,	1/2\n"
				"mfv	%0,	S022\n"
				"mfv	%1,	S032\n"
				:"=r"(x),"=r"(y)
				:"r"(u1),"r"(v1)
			);
			vtxbuf->x = x;
			vtxbuf->y = y;
			vtxbuf->z = z;
			++vtxbuf;

			u0 += sw;
		}
		sceGuDrawArray(GU_TRIANGLE_STRIP,DXP_VTYPE_2DTEX | GU_TRANSFORM_2D,2 * swn + 2,0,buf);
		v0 += sh;
	}
	return 0;
}
