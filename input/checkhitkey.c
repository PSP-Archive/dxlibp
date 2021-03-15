#include "../input.h"

int CheckHitKey(int keycode)
{
	if(keycode < 0 || keycode >= 256)return 0;
	return dxpInputData.key[keycode];
}