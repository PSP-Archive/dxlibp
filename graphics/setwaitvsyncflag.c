#include "../graphics.h"

int SetWaitVSyncFlag(int flag)
{
	dxpGraphicsData.waitvsinc = flag ? 1 : 0;
	return 0;
}
