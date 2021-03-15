#include "../graphics.h"
#include "../debug.h"
#include <pspdisplay.h>

int ScreenFlip()
{
	GUINITCHECK;
	GUSYNC;
	if(dxpGraphicsData.debugScreenCallback)dxpGraphicsData.debugScreenCallback();
	sceGuSwapBuffers();
	dxpGraphicsWaitVSync();
	dxpGraphicsData.displaybuffer_back = &dxpGraphicsData.displaybuffer[0] == dxpGraphicsData.displaybuffer_back ? &dxpGraphicsData.displaybuffer[1] : &dxpGraphicsData.displaybuffer[0];
	if(dxpGraphicsData.rendertarget == &dxpGraphicsData.displaybuffer[0] || dxpGraphicsData.rendertarget == &dxpGraphicsData.displaybuffer[1])
		dxpGraphicsData.rendertarget = dxpGraphicsData.displaybuffer_back;
	return 0;
}
