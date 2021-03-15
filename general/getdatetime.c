#include "../general.h"
#include <psprtc.h>

int GetDateTime(DATEDATA *datebuf)
{
	pspTime time;
	if(!datebuf)return -1;
	if(sceRtcGetCurrentClockLocalTime(&time) < 0)return -1;
	datebuf->Year = time.year;
	datebuf->Mon = time.month;
	datebuf->Day = time.day;
	datebuf->Hour = time.hour;
	datebuf->Min = time.minutes;
	datebuf->Sec = time.seconds;
	return 0;
}
