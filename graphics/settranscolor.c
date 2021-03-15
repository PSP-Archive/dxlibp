#include "../graphics.h"

int SetTransColor(int color)
{
	GUINITCHECK;
	dxpGraphicsData.colorkey = color;
	return 0;
}