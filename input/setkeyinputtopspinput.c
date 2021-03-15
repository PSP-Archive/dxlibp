#include "../input.h"

int SetKeyInputToPspInput(int key,int pspbutton)
{
	if(key < 0 || key > 255)return -1;
	dxpInputData.psp_to_key[key] = pspbutton;
	return 0;
}