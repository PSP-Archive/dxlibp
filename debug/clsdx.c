#include "../debug.h"

int clsDx()
{
	int i;
	dxpDebugData.cx = 0;
	dxpDebugData.cy = 0;
	dxpDebugData.l1 = 0;
	for(i = 0;i < 23;++i)dxpDebugBuf.strbuf[i][0] = '\0';
	return 0;
}