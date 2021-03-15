#include "../../graphics.h"
#ifndef DXP_BUILDOPTION_NO3D

int DrawLine3D(VECTOR pos1,VECTOR pos2,int color)
{
	GUSTART;
	dxpGraphicsSetup3D(color);
	DXP_FVF_3D *vtx = dxpGuGetMemory(sizeof(DXP_FVF_3D) * 2);
	if(!vtx)return -1;
	vtx[0].x = pos1.x;
	vtx[0].y = pos1.y;
	vtx[0].z = pos1.z;
	vtx[1].x = pos2.x;
	vtx[1].y = pos2.y;
	vtx[1].z = pos2.z;
	sceGumDrawArray(GU_LINES,DXP_VTYPE_3D | GU_TRANSFORM_3D,2,0,vtx);
	return 0;
}

#endif