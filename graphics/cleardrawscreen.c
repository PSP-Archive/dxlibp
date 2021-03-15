#include "../graphics.h"

int ClearDrawScreen()
{
	GUINITCHECK;
	GUSTART;
	sceGuClear(
		GU_COLOR_BUFFER_BIT | 
		(dxpGraphicsData.usedepth || dxpGraphicsData.usedepth3d ? GU_DEPTH_BUFFER_BIT : 0) | 
		(dxpGraphicsData.clear_stencil ? GU_STENCIL_BUFFER_BIT : 0)
	);
	return 0;
}