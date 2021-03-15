#include "../graphics.h"
int SetDrawBlendMode(int Blendmode,int Param)
{
	GUINITCHECK;
	if(Blendmode == DX_BLENDMODE_NOBLEND)Param = 255;
	Param &= 0xff;
	if(dxpGraphicsData.blendmode == Blendmode 
		&& Param == (dxpGraphicsData.color >> 24))
	{
//		dxpGraphicsData.forceupdate = 1;
		return 0;
	}
	dxpGraphicsData.blendmode = Blendmode;
	dxpGraphicsData.color = (dxpGraphicsData.color & 0x00ffffff) | ((u32)Param << 24);
	dxpGraphicsData.forceupdate = 1;
	return 0;
}
