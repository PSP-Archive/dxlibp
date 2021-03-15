#include "../general.h"
#include "../sound.h"
#include "../input.h"
#include "../graphics.h"
#include "../safealloc.h"
#include <stdlib.h>
#include <time.h>

/*HOME button check*/
static void exitfunc()
{
	dxpGeneralData.exit_called = 1;
	sceKernelExitGame();
}// __attribute__((destructor))//PSPのGCCはこのオプション使えないみたい

static int exit_callback(int arg1, int arg2, void *common)
{
	dxpGeneralData.homebutton_pushed = 1;
	return 0;
}

static int ExitCallbackThread(SceSize args, void *argp)
{
	sceKernelRegisterExitCallback(sceKernelCreateCallback("HOME button Callback", exit_callback, NULL));
	sceKernelSleepThreadCB();
	return 0;
}

static SceUID SetExitCallback()
{
	SceUID thid = sceKernelCreateThread("dxp HOME button callback thread", ExitCallbackThread, 0x11, 0xFA0, 0, 0);
	if (thid >= 0) {
		sceKernelStartThread(thid, 0, 0);
	}
	atexit(exitfunc);
	return thid;
}

int DxLib_Init()
{
	if(dxpGeneralData.initialized)return 0;

	//dxpSafeAllocInit();
	if(!dxpGeneralData.homebutton_callback_initialized)
	{
		dxpGeneralData.homebutton_callback_threadid = SetExitCallback();
		dxpGeneralData.homebutton_callback_initialized = 1;
	}
	dxpInputInit();
	dxpGraphicsInit();
	dxpSoundInit();

	SRand(time(NULL));
	
	dxpGeneralData.initialized = 1;
	return 0;
	/*
err:
	return -1;
	*/
}

