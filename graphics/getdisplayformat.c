#include "../graphics.h"

int GetDisplayFormat()
{
	GUINITCHECK;
	return dxpGraphicsData.display_psm;
}