#include "../../cpcnv.h"


int dxpCpSJIS_fromUcs2(dxpChar *dst,u32 count,const dxpUcs2 *src)
{
	if(!src || !dst || !count)return 0;
	if (!dxpCpData.init) dxpCpInit();
	if(!dxpCpData.table[DXP_CP_SJIS][1].used)dxpCpLoadTable(DXPCPTABLE_FILENAME,DXP_CP_SJIS,DXPCP_FROMUCS2);
	
	int i = 0, dstpos = 0,length = 0, j, code, id;
	unsigned char tmp[2];
	unsigned short *ptmp = (unsigned short*)tmp;
	if (!dxpCpData.table[DXP_CP_SJIS][1].used)
	{
		dst[0] = '\0';
		return 0;
	}
	u16 headerNum = *((u16*)(dxpCpData.table[DXP_CP_SJIS][1].ptr) + 2);
	DXPCPTABLE_HEADER *header = (DXPCPTABLE_HEADER*)((u16*)dxpCpData.table[DXP_CP_SJIS][1].ptr + 3);
	dxpUcs2 *UCS2toSJIS = (dxpUcs2*)(dxpCpData.table[DXP_CP_SJIS][1].ptr) + headerNum * 3 + 3;
	while (src[i] && length < count) {
		code = src[i];
		id = -1;
		if(!(code >> 8))
		{
			dst[dstpos++] = code & 0xff;
		}else
		{
			//2byte
			for (j = 1; (j <= headerNum) && (id < 0); j++) {
				if ((code >= header[j].startcode) && (code <= header[j].endcode))
				{
					id = header[j].offset + code - header[j].startcode;
				}
			}
			*ptmp = (id < 0) ? 0 : UCS2toSJIS[id];
			//JIS->SJIS
			tmp[0] ^= tmp[1];
			tmp[1] ^= tmp[0];
			tmp[0] ^= tmp[1];
			if (tmp[0] & 1) {
				tmp[0] = ((tmp[0] + 1) >> 1) + 0x70;
				tmp[1] += 0x1f;
			} else {
				tmp[0] = (tmp[0] >> 1) + 0x70;
				tmp[1] += 0x7d;
			}
			if (tmp[0] >= 0xa0) { tmp[0] += 0x40; }
			if (tmp[1] >= 0x7f) { tmp[1] += 1; }

			dst[dstpos++] = tmp[0];
			dst[dstpos++] = tmp[1];
		}
		++length;
		++i;
	}
	dst[dstpos] = '\0';
	return length;
}
