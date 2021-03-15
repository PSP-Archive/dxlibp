#include "../general.h"

int GetRand(int max)
{
	u32 result;
	switch(dxpGeneralData.randmode)
	{
	case DXP_RANDMODE_MT:
		if(!dxpGeneralData.mt19937context)
		{
			ChangeRandMode(DXP_RANDMODE_MT);
			return GetRand(max);
		}
		return sceKernelUtilsMt19937UInt(dxpGeneralData.mt19937context) % (max + 1);
	case DXP_RANDMODE_HW:
		__asm__ volatile
		(
			"vrndi.s S000\n"
			"mfv      %0, S000\n"
			: "=r"(result)
		);
		return (int)(result % (max + 1));
	default:
		ChangeRandMode(DXP_RANDMODE_MT);
		return GetRand(max);
	}
}