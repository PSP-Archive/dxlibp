#include "dxlibp.h"
#include "font/intraFont.h"


#define DXP_FONTNAME_JPN "flash0:/font/jpn0.pgf"
#define DXP_FONTNAME_DEFAULT DXP_FONTNAME_JPN

typedef struct DXPFONTHANDLE
{
	unsigned used : 1;
	unsigned edgeEnable : 1;
	intraFont *pif;
	float	scale;
	u32		edgeColor;
	int		fontAlign;
}DXPFONTHANDLE;

typedef struct DXPFONTDATA
{
	unsigned init : 1;
}DXPFONTDATA;

extern DXPFONTHANDLE dxpFontArray[];
extern DXPFONTDATA dxpFontData;

u32 dxpFontIntrafontStart(int color);
void dxpFontIntrafontFinish(void);
DXPFONTHANDLE* dxpFontReserveHandle();
void dxpFontReleaseHandle(int handle);

/*
InitFontToHandle	　フォントデータを全て初期化する
*/
