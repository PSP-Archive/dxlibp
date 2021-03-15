#include "../graphics.h"
int SetDrawBright(int Red,int Green,int Blue)
{
	GUINITCHECK;
	u32 color = (dxpGraphicsData.color & 0xff000000)
				| (0x00ffffff & (u32)GetColor(Red,Green,Blue));
	if(color == dxpGraphicsData.color)return 0;
	dxpGraphicsData.color = color;
	dxpGraphicsData.forceupdate = 1;
	return 0;
}
