#include "../input.h"

int GetJoypadInputState(int inputtype)
{
	if(inputtype)
		return dxpInputData.pad1;
	return 0;
}

