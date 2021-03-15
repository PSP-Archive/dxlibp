#include "../graphics.h"

int GetScreenState(int *xsize,int *ysize,int *bpp)
{
	GUINITCHECK;
	if(xsize)*xsize = 480;
	if(ysize)*ysize = 272;
	if(bpp)*bpp = dxpGraphicsData.display_psm == DXP_FMT_8888 ? 32 : 16;
	return 0;
}