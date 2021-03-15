#include "../../cpcnv.h"

int dxpCpUcs2_Strlen(const dxpUcs2 *str)
{
	int i;
	for(i = 0;str[i];++i);
	return i;
}