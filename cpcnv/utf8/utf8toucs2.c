#include "../../cpcnv.h"

int dxpCpUTF8_toUcs2(dxpUcs2 * dst, u32 count,const dxpChar * src)
{
	if (!src || !dst || !count) return 0;

    int i = 0, length = 0,cl;
    while (src[i] && length < count)
	{
		cl = dxpCpUTF8_Charlen(src + i);
		switch(cl)
		{
		case -1:
			for(++i;(src[i] & 0xc0) == 0x80;++i);
			continue;
		case 1:
			dst[length++] = src[i];
			break;
		case 2:
			dst[length++] = ((src[i] & 0x001f) << 6) | (src[i + 1] & 0x003f); 
			break;
		case 3:
			dst[length++] = ((src[i] & 0x001f) << 12) | ((src[i + 1] & 0x003f) << 6) | (src[i + 2] & 0x003f); 
			break;
		}
		i += cl;
	}
    return length;
}

