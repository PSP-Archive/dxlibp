#include "../graphics.h"
#include <pspdisplay.h>
#include <string.h>
int ScreenCopy()
{
	GUINITCHECK;
	GUSYNC;
	DXPTEXTURE3 *tf,*tb;
	register void *front,*back;
	tf = &dxpGraphicsData.displaybuffer[&dxpGraphicsData.displaybuffer[0] == dxpGraphicsData.displaybuffer_back ? 1 : 0];
	tb = dxpGraphicsData.displaybuffer_back;
	front = tf->texvram;
	back = tb->texvram;
	memcpy(front,back,dxpGraphicsData.display_psm == GU_PSM_8888 ? 557056 : 278528);
	sceKernelDcacheWritebackAll();
	if(dxpGraphicsData.debugScreenCallback)dxpGraphicsData.debugScreenCallback();
	sceGuSwapBuffers();
	dxpGraphicsWaitVSync();
	dxpGraphicsData.displaybuffer_back = tf;
	if(dxpGraphicsData.rendertarget == &dxpGraphicsData.displaybuffer[0] || dxpGraphicsData.rendertarget == &dxpGraphicsData.displaybuffer[1])
		dxpGraphicsData.rendertarget = tf;
	return 0;
}
