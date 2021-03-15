#include "../font.h"

int DeleteFontToHandle(int handle)
{
	if(handle <= 0 || handle >= DXP_BUILDOPTION_FONTHANDLE_MAX)return -1;//0î‘ÇÕïKÇ∏égÇ§ÇÃÇ≈è¡Ç≥Ç»Ç¢
	if(!dxpFontArray[handle].used)return -1;
	dxpFontReleaseHandle(handle);
	return 0;
}
