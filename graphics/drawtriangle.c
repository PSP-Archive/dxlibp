#include "../graphics.h"

int DrawTriangle(int x1,int y1,int x2,int y2,int x3,int y3,int color,int fillflag)
{
	GUINITCHECK;
	if(!fillflag)
	{
		int res = 0;
		res += DrawLine(x1,y1,x2,y2,color);
		res += DrawLine(x2,y2,x3,y3,color);
		res += DrawLine(x1,y1,x3,y3,color);
		return res ? -1 : 0;;
	}
	dxpGraphicsSetup2D(color);
	DXP_FVF_2D *vertex = (DXP_FVF_2D*)dxpGuGetMemory(sizeof(DXP_FVF_2D) * 3);
	if(!vertex)return -1;
	vertex[0].x = x1;
	vertex[0].y = y1;
	vertex[0].z = dxpGraphicsData.z_2d;
	vertex[1].x = x2;
	vertex[1].y = y2;
	vertex[1].z = dxpGraphicsData.z_2d;
	vertex[2].x = x3;
	vertex[2].y = y3;
	vertex[2].z = dxpGraphicsData.z_2d;
	sceGuDrawArray(GU_TRIANGLES,DXP_VTYPE_2D | GU_TRANSFORM_2D,3,0,vertex);
	return 0;
}
