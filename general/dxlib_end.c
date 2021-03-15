#include "../general.h"
#include "../graphics.h"
#include "../safealloc.h"
int DxLib_End()
{
	if(!dxpGeneralData.initialized)return 0;
//	InitSoundMem();
	dxpGraphicsEnd();
	//dxpSafeAllocEnd();
	return 0;
}