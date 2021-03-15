#include "../input.h"
#include <string.h>

DXPINPUTDATA dxpInputData;

void dxpInputInit()
{
	int i,j;
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
	memset(&dxpInputData.ctrldata,0,sizeof(SceCtrlData));

	for(i = 0;i < 32;++i)
		for(j = 0;j < 4;++j)dxpInputData.key_to_pad1[i][j] = -1;
	memset(&dxpInputData.psp_to_key[0],0,sizeof(int) * 256);


	dxpInputData.key_to_pad1[PAD_INPUT_DOWN_BIT][0] = KEY_INPUT_DOWN;
	dxpInputData.key_to_pad1[PAD_INPUT_DOWN_BIT][1] = KEY_INPUT_NUMPAD2;
	dxpInputData.key_to_pad1[PAD_INPUT_LEFT_BIT][0] = KEY_INPUT_LEFT;
	dxpInputData.key_to_pad1[PAD_INPUT_LEFT_BIT][1] = KEY_INPUT_NUMPAD4;
	dxpInputData.key_to_pad1[PAD_INPUT_RIGHT_BIT][0] = KEY_INPUT_RIGHT;
	dxpInputData.key_to_pad1[PAD_INPUT_RIGHT_BIT][1] = KEY_INPUT_NUMPAD6;
	dxpInputData.key_to_pad1[PAD_INPUT_UP_BIT][0] = KEY_INPUT_UP;
	dxpInputData.key_to_pad1[PAD_INPUT_UP_BIT][1] = KEY_INPUT_NUMPAD8;
	dxpInputData.key_to_pad1[PAD_INPUT_A_BIT][0] = KEY_INPUT_Z;
	dxpInputData.key_to_pad1[PAD_INPUT_B_BIT][0] = KEY_INPUT_X;
	dxpInputData.key_to_pad1[PAD_INPUT_C_BIT][0] = KEY_INPUT_C;
	dxpInputData.key_to_pad1[PAD_INPUT_X_BIT][0] = KEY_INPUT_A;
	dxpInputData.key_to_pad1[PAD_INPUT_Y_BIT][0] = KEY_INPUT_S;
	dxpInputData.key_to_pad1[PAD_INPUT_Z_BIT][0] = KEY_INPUT_D;
	dxpInputData.key_to_pad1[PAD_INPUT_L_BIT][0] = KEY_INPUT_Q;
	dxpInputData.key_to_pad1[PAD_INPUT_R_BIT][0] = KEY_INPUT_W;
	dxpInputData.key_to_pad1[PAD_INPUT_START_BIT][0] = KEY_INPUT_ESCAPE;
	dxpInputData.key_to_pad1[PAD_INPUT_M_BIT][0] = KEY_INPUT_SPACE;

	dxpInputData.psp_to_key[KEY_INPUT_DOWN] = DXP_INPUT_DOWN;
	dxpInputData.psp_to_key[KEY_INPUT_LEFT] = DXP_INPUT_LEFT;
	dxpInputData.psp_to_key[KEY_INPUT_RIGHT] = DXP_INPUT_RIGHT;
	dxpInputData.psp_to_key[KEY_INPUT_UP] = DXP_INPUT_UP;
	dxpInputData.psp_to_key[KEY_INPUT_Z] = DXP_INPUT_CIRCLE;
	dxpInputData.psp_to_key[KEY_INPUT_X] = DXP_INPUT_CROSS;
	dxpInputData.psp_to_key[KEY_INPUT_A] = DXP_INPUT_TRIANGLE;
	dxpInputData.psp_to_key[KEY_INPUT_S] = DXP_INPUT_SQUARE;
	dxpInputData.psp_to_key[KEY_INPUT_Q] = DXP_INPUT_LTRIGGER;
	dxpInputData.psp_to_key[KEY_INPUT_W] = DXP_INPUT_RTRIGGER;
	dxpInputData.psp_to_key[KEY_INPUT_ESCAPE] = DXP_INPUT_START;
	dxpInputData.psp_to_key[KEY_INPUT_SPACE] = DXP_INPUT_SELECT;



	dxpInputRenew();
	dxpInputRenew();
}

void dxpInputRenew()
{
	int i;

	sceCtrlReadBufferPositive(&dxpInputData.ctrldata,1);

	dxpInputData.pad1 = 0;
	memset(dxpInputData.key,0,sizeof(char) * 256);

	//PSP→キーボード
	for(i = 0;i < 256;++i)
	{
		dxpInputData.key[i] = dxpInputData.psp_to_key[i] & dxpInputData.ctrldata.Buttons ? 1 : 0;
	}

	//キーボード→PAD1
	for(i = 0;i < 32;++i)
	{
		if(dxpInputData.key_to_pad1[i][0] != -1)
			dxpInputData.pad1 |= (dxpInputData.key[dxpInputData.key_to_pad1[i][0]] << i);
		if(dxpInputData.key_to_pad1[i][1] != -1)
			dxpInputData.pad1 |= (dxpInputData.key[dxpInputData.key_to_pad1[i][1]] << i);
		if(dxpInputData.key_to_pad1[i][2] != -1)
			dxpInputData.pad1 |= (dxpInputData.key[dxpInputData.key_to_pad1[i][2]] << i);
		if(dxpInputData.key_to_pad1[i][3] != -1)
			dxpInputData.pad1 |= (dxpInputData.key[dxpInputData.key_to_pad1[i][3]] << i);
	}

}

int dxpInputExp(int pspinput)
{
	int i;
	if(!pspinput)return -1;
	for(i = 0;i < 32;++i)
		if(pspinput & (1 << i))return i;
	return -1;
}