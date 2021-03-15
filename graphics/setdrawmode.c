#include "../graphics.h"
int SetDrawMode(int Mode)
{
	GUINITCHECK;
	GUSTART;
	switch(Mode)
	{
	case DX_DRAWMODE_NEAREST:
		sceGuTexFilter(GU_NEAREST,GU_NEAREST);
		dxpGraphicsData.bilinear_enable = 0;
		return 0;
	case DX_DRAWMODE_BILINEAR:
		sceGuTexFilter(GU_LINEAR,GU_LINEAR);
		dxpGraphicsData.bilinear_enable = 1;
		return 0;
	default:
		return -1;
	}
}
