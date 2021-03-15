#include "../general.h"

void Sleep(int ms)
{
	sceKernelDelayThread(ms * 1000);
}
