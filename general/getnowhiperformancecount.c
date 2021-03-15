#include "../general.h"
#include <psprtc.h>

u64 GetNowHiPerformanceCount()
{
	u64 tick;
	if(sceRtcGetCurrentTick(&tick) != 0)return 0;
	return tick;
}