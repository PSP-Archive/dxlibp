#include "../debug.h"
#include "../graphics.h"
DXPDEBUGDATA dxpDebugData =
{
	0,//init
	0,
	0,
	0,
	{6,12},
};
DXP_DEBUG_BUF dxpDebugBuf;

void dxpDebugInit()
{
	if(dxpDebugData.init)return;
	if(dxpDebugDrawStringInit(&dxpDebugData.fontsize[0],&dxpDebugData.fontsize[1]) < 0)return;
	if(dxpDebugData.fontsize[0] < 6 || dxpDebugData.fontsize[1] < 12)return;
	dxpDebugData.strbufsize[0] = (480 + dxpDebugData.fontsize[0] - 1) / dxpDebugData.fontsize[0];
	dxpDebugData.strbufsize[1] = (272 + dxpDebugData.fontsize[1] - 1) / dxpDebugData.fontsize[1];
	dxpDebugData.init = 1;
	dxpGraphicsData.debugScreenCallback = dxpDrawDebugScreen;
	return;
}