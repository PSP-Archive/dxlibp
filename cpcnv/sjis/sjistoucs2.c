#include "../../cpcnv.h"
//èIí[ï∂éöÇä‹ÇﬂÇΩï∂éöêîÇéwíËÇµÇƒÇ≠ÇæÇµÇ†

int dxpCpSJIS_toUcs2(dxpUcs2 *dst,u32 count,const dxpChar *src)
{
	if(!src || !dst || !count)return 0;
	if(!dxpCpData.init)dxpCpInit();
	if(!dxpCpData.table[DXP_CP_SJIS][0].used)dxpCpLoadTable(DXPCPTABLE_FILENAME,DXP_CP_SJIS,DXPCP_TOUCS2);
	int i = 0,length = 0,j,code,id;
	if(!dxpCpData.table[DXP_CP_SJIS][0].ptr)
	{
		dst[0] = dxpCpData.errcode_ucs2;
		return 1;
	}
	u16 headerNum = *((u16*)(dxpCpData.table[DXP_CP_SJIS][0].ptr) + 2);
	DXPCPTABLE_HEADER *header = (DXPCPTABLE_HEADER*)((u16*)dxpCpData.table[DXP_CP_SJIS][0].ptr + 3);
	dxpUcs2 *SJIStoUCS2 = (dxpUcs2*)(dxpCpData.table[DXP_CP_SJIS][0].ptr) + headerNum * 3 + 3;
	while (src[i] && length < count)
	{
		code = src[i];
		id = -1;
		for (j = 0; (j < headerNum) && (id < 0); j++)
		{
			if ((code >= header[j].startcode) && (code <= header[j].endcode)) {
				id = header[j].offset + code - header[j].startcode; 
			} else {
				if (j == 1) code = 0x0200 * src[i] - 0xE100 - ((src[i] >= 0xE0) ? 0x8000 : 0) + src[i+1] + ((src[i+1] <= 0x7E) ? -0x1F : ((src[i+1] >= 0x9F) ? 0x82 : -0x20) );
			}
		}
		dst[length++] = (id < 0) ? dxpCpData.errcode_ucs2 : SJIStoUCS2[id];
		i += dxpCpSJIS_MultiByteCharCheck(src[i]) + 1;
	}
	dst[length] = 0;
	return length;
}
