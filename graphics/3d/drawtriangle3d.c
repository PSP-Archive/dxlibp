#include "../../graphics.h"

#ifndef DXP_BUILDOPTION_NO3D



int DrawTriangle3D(VECTOR pos1,VECTOR pos2,VECTOR pos3,int color,int fillflag)
{
	GUSTART;
	dxpGraphicsSetup3D(color);
	if(!fillflag)
	{
		DrawLine3D(pos1,pos2,color);
		DrawLine3D(pos2,pos3,color);
		DrawLine3D(pos3,pos1,color);
		return 0;
	}
	DXP_FVF_3D *vtx = dxpGuGetMemory(sizeof(DXP_FVF_3D) * 3);
	if(!vtx)return -1;
	vtx[0].x = pos1.x;
	vtx[0].y = pos1.y;
	vtx[0].z = pos1.z;
	vtx[1].x = pos2.x;
	vtx[1].y = pos2.y;
	vtx[1].z = pos2.z;
	vtx[2].x = pos3.x;
	vtx[2].y = pos3.y;
	vtx[2].z = pos3.z;
	sceGumDrawArray(GU_TRIANGLES,DXP_VTYPE_3D | GU_TRANSFORM_3D,3,0,vtx);
	return 0;
}

#endif
