#include "../graphics.h"
int WaitGPUSync()
{
	GUINITCHECK;
	GUSYNC;
	GUSTART;
	return 0;
}