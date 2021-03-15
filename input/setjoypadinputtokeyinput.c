#include "../input.h"

int SetJoypadInputToKeyInput(int inputtype,int padinput,int keyinput1,int keyinput2,int keyinput3,int keyinput4)
{
	int bit;
	if(!(inputtype & DX_INPUT_PAD1))return 0;
	bit = dxpInputExp(padinput);
	if(bit == -1)return -1;
	if(keyinput1 < 0 || keyinput1 > 255)keyinput1 = -1;
	if(keyinput2 < 0 || keyinput2 > 255)keyinput2 = -1;
	if(keyinput3 < 0 || keyinput3 > 255)keyinput3 = -1;
	if(keyinput4 < 0 || keyinput4 > 255)keyinput4 = -1;
	dxpInputData.key_to_pad1[bit][0] = keyinput1;
	dxpInputData.key_to_pad1[bit][1] = keyinput2;
	dxpInputData.key_to_pad1[bit][2] = keyinput3;
	dxpInputData.key_to_pad1[bit][3] = keyinput4;
	return 0;
}
