#include "../graphics.h"

int SetDrawArea(int x1,int y1,int x2,int y2)
{
	GUINITCHECK;
	GUSTART;
	sceGuScissor(x1,y1,x2,y2);
	dxpGraphicsData.intrafont_scissor[0] = x1;
	dxpGraphicsData.intrafont_scissor[1] = y1;
	dxpGraphicsData.intrafont_scissor[2] = x2;
	dxpGraphicsData.intrafont_scissor[3] = y2;
	return 0;
}