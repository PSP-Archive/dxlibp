#include "../general.h"
#include <psprtc.h>

int GetNowCount()
{
	u64 tick;
	if(sceRtcGetCurrentTick(&tick) != 0)return 0;
	return (int)(tick / 1000);
}