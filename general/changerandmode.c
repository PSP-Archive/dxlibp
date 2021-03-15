#include "../general.h"
#include <malloc.h>
int ChangeRandMode(int mode)
{
	switch(mode)
	{
	case DXP_RANDMODE_MT:
		dxpGeneralData.randmode = DXP_RANDMODE_MT;
		if(!dxpGeneralData.mt19937context)
			dxpGeneralData.mt19937context = (SceKernelUtilsMt19937Context*) malloc(sizeof(SceKernelUtilsMt19937Context));
		if(dxpGeneralData.mt19937context)return 0;
	case DXP_RANDMODE_HW:
		dxpGeneralData.randmode = DXP_RANDMODE_HW;
		if(dxpGeneralData.mt19937context)
		{
			free(dxpGeneralData.mt19937context);
			dxpGeneralData.mt19937context = NULL;
		}
		return 0;
	default:
		return ChangeRandMode(DXP_RANDMODE_MT);
	}
}