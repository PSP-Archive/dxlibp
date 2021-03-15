#include "../general.h"

int SRand(int seed)
{
	switch(dxpGeneralData.randmode)
	{
	case DXP_RANDMODE_MT:
		if(!dxpGeneralData.mt19937context)
		{
			ChangeRandMode(DXP_RANDMODE_MT);
			return SRand(seed);
		}
		sceKernelUtilsMt19937Init(dxpGeneralData.mt19937context,(u32)seed);
		return 0;
	case DXP_RANDMODE_HW:
		__asm__ volatile
		(
			"mtv		%0,	S000\n"
			"vrnds.s	S000	\n"
			:: "r"(seed)
		);
		return 0;
	default:
		ChangeRandMode(DXP_RANDMODE_MT);
		return SRand(seed);
	}
}

