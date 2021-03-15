#include "../input.h"

int GetJoypadAnalogInput(int *xbuf,int *ybuf,int inputtype)
{
	if(inputtype & DX_INPUT_PAD1)
	{
		if(xbuf)*xbuf = dxpInputData.ctrldata.Lx * 7.8125f - 1000;
		if(ybuf)*ybuf = dxpInputData.ctrldata.Ly * 7.8125f - 1000;
		return 0;
	}
	else
	{
		if(xbuf)*xbuf = 0;
		if(ybuf)*ybuf = 0;
		return 0;
	}
}
