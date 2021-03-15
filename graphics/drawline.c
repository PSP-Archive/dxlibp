#include "../graphics.h"

int DrawLine(int x1,int y1,int x2,int y2,int color)
{
	GUINITCHECK;
	dxpGraphicsSetup2D(color);
	DXP_FVF_2D *vertex = (DXP_FVF_2D*)dxpGuGetMemory(sizeof(DXP_FVF_2D) * 2);
	if(!vertex)return -1;
	vertex[0].x = x1;
	vertex[0].y = y1;
	vertex[0].z = dxpGraphicsData.z_2d;
	vertex[1].x = x2;
	vertex[1].y = y2;
	vertex[1].z = dxpGraphicsData.z_2d;
	sceGuDrawArray(GU_LINES,DXP_VTYPE_2D | GU_TRANSFORM_2D,2,0,vertex);
	return 0;
}