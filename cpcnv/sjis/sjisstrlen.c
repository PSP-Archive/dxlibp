#include "../../cpcnv.h"


int dxpCpSJIS_Strlen(const dxpChar *str)
{
	if(!str)return 0;
	int length = 0;
	while(*str)
	{
		str += dxpCpSJIS_MultiByteCharCheck(*str) + 1;
		++length;
	}
	return length;
}
