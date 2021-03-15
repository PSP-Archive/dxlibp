#include "../../cpcnv.h"

int dxpCpUTF8_Strlen(const dxpChar *str)
{
	int i,cl,length;
	for(i = length = 0;str[i];)
	{
		cl = dxpCpUTF8_Charlen(str + i);
		if(cl == -1)
			for(++i;(str[i] & 0xc0) == 0x80;++i);
		else
		{
			i += cl;
			++length;
		}
	}
	return length;
}