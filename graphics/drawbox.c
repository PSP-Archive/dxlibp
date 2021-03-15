#include "../graphics.h"

int DrawBox(int x1,int y1,int x2,int y2,int color,int fillflag)
{
	GUINITCHECK;
	if(fillflag)
	{
		dxpGraphicsSetup2D(color);
		DXP_FVF_2D *vertex = (DXP_FVF_2D*)dxpGuGetMemory(sizeof(DXP_FVF_2D) * 2);
		if(!vertex)return -1;
		vertex[0].x = x1;
		vertex[0].y = y1;
		vertex[0].z = dxpGraphicsData.z_2d;
		vertex[1].x = x2;
		vertex[1].y = y2;
		vertex[1].z = dxpGraphicsData.z_2d;
		sceGuDrawArray(GU_SPRITES,DXP_VTYPE_2D | GU_TRANSFORM_2D,2,0,vertex);
		return 0;
	}else
	{
		int dres = 0;
		x1 += 1;
		y1 += 1;
		dres += DrawLine(x1,y1,x2,y1,color);
		dres += DrawLine(x1,y1,x1,y2,color);
		dres += DrawLine(x2,y1,x2,y2,color);
		dres += DrawLine(x1,y2,x2,y2,color);
		return dres ? -1 : 0;
	}
}
