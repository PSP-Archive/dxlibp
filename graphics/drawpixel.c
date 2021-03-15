#include "../graphics.h"

int DrawPixel(int x,int y,int color)
{
	GUINITCHECK;
	dxpGraphicsSetup2D(color);
	DXP_FVF_2D *vertex = (DXP_FVF_2D*)dxpGuGetMemory(sizeof(DXP_FVF_2D));
	if(!vertex)return -1;
	vertex->x = x;
	vertex->y = y;
	vertex->z = dxpGraphicsData.z_2d;
	sceGuDrawArray(GU_POINTS,DXP_VTYPE_2D | GU_TRANSFORM_2D,1,0,vertex);
	return 0;
}
