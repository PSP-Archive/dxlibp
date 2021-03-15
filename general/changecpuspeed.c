#include <psppower.h>

int ChangeCpuSpeed(int freq)
{
	int res = scePowerGetCpuClockFrequencyInt();
	if(1 <= freq && freq <= 333)scePowerSetClockFrequency(freq,freq,freq / 2);
	return res;
}