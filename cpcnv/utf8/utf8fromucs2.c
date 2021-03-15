#include "../../cpcnv.h"

int dxpCpUTF8_fromUcs2(dxpChar *dst,u32 count,const dxpUcs2 *src)
{
	int i, length;
	for(i = length = 0;src[i] && length < count - 1;++i)
	{
		if(src[i] <= 0x007F)
		{//1byte
			dst[length++] = src[i] & 0x7FFF;
		}else if(src[i] <= 0x07FF)
		{//2byte
			dst[length++] = 0xC0 | ((src[i] & 0x07C0) >> 6); 
			dst[length++] = 0x80 | (src[i] & 0x003F);
		}else
		{//3byte
			dst[length++] = 0xE0 | ((src[i] & 0xF000) >> 12);
			dst[length++] = 0x80 | ((src[i] & 0x0FC0) >> 6);
			dst[length++] = 0x80 | (src[i] & 0x003F);
		}
	}
	dst[length++] = 0;
	return length;
}
