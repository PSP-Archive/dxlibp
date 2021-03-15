#include "../font.h"

int CreateFontToHandle(const char *fontname,int size,int thick,int fonttype,int charset)//thick‚Í–³Ž‹
{
	if(fonttype < 0 || fonttype > 3)return -1;
	if(size == -1)size = DXP_FONT_DEFAULT_SIZE;
	DXPFONTHANDLE *pHnd;
	intraFont *pif;
	if(fontname == NULL)
		fontname = DXP_FONTNAME_DEFAULT;
	pif = intraFontLoad(fontname,0x00010000 * charset);
	if(!pif)return -1;
	pHnd = dxpFontReserveHandle();
	if(!pHnd)
	{
		intraFontUnload(pif);
		return -1;
	}
	pHnd->pif = pif;
	pHnd->edgeEnable = fonttype & 1;
	pHnd->scale = (float)size / DXP_FONT_DEFAULT_SIZE;
	return pHnd - dxpFontArray;
}
