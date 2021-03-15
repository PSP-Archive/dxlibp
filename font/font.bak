#include "../font.h"
#include "../graphics.h"
#include "libc/stdio.h"
#include "string.h"
static int gustatus;

DXPFONTDATA dxpFontData;
DXPFONTHANDLE dxpFontArray[DXP_BUILDOPTION_FONTHANDLE_MAX];

void dxpFontInit()
{
	if(dxpFontData.init)return;
	intraFontInit();
	int i;
	for(i = 0;i < DXP_BUILDOPTION_FONTHANDLE_MAX;++i)
		dxpFontArray[i].used = 0;
	dxpFontArray[0].used = 1;
	dxpFontArray[0].pif = intraFontLoad(DXP_FONTNAME_DEFAULT,INTRAFONT_STRING_SJIS);
	dxpFontArray[0].scale = 1.0f;
	dxpFontArray[0].edgeColor = DXP_COLOR_DARKGRAY;
	dxpFontArray[0].fontAlign = DXP_FONT_ALIGN_DEFAULT;
	dxpFontArray[0].edgeEnable = 1;
	dxpFontData.init = 1;
}

void dxpFontEnd()
{
	if(!dxpFontData.init)return;
	InitFontToHandle();
	intraFontUnload(dxpFontArray[0].pif);
	intraFontShutdown();
	dxpFontData.init = 0;
}

u32 dxpFontIntrafontStart(int color)
{
	GUSTART;
	if(dxpGraphicsData.drawstate != DXP_DRAWSTATE_INTRAFONT)
	{
		gustatus = sceGuGetAllStatus();
		dxpGraphicsData.drawstate = DXP_DRAWSTATE_INTRAFONT;
		dxpGraphicsData.forceupdate = 1;
	}
	dxpGraphicsData.texture = NULL;
	int op;
	int src,dest;
	unsigned int srcfix;
	unsigned int destfix;	 
	switch(dxpGraphicsData.blendmode)
	{
	case DX_BLENDMODE_NOBLEND:
	case DX_BLENDMODE_ALPHA:
		op = GU_ADD;
		src = GU_SRC_ALPHA;
		dest = GU_ONE_MINUS_SRC_ALPHA;
		srcfix = 0;
		destfix = 0;
		break;
	case DX_BLENDMODE_ADD:
		op = GU_ADD;
		src = GU_SRC_ALPHA;
		dest = GU_FIX;
		srcfix = 0xffffffff;
		destfix = 0xffffffff;
		break;
	case DX_BLENDMODE_SUB:
		op = GU_REVERSE_SUBTRACT;
		src = GU_SRC_ALPHA;
		dest = GU_FIX;
		srcfix = 0xffffffff;
		destfix = 0xffffffff;
		break;
	case DX_BLENDMODE_MUL:
		op = GU_ADD;
		src = GU_DST_COLOR;
		dest = GU_FIX;
		srcfix = 0xffffffff;
		destfix = 0xffffffff;
		break;
	case DX_BLENDMODE_DESTCOLOR:
		op = GU_ADD;
		src = GU_FIX;
		dest = GU_FIX;
		srcfix = 0;
		destfix = 0xffffffff;
		break;
	case DX_BLENDMODE_INVDESTCOLOR:
		op = GU_ADD;
		src = GU_ONE_MINUS_DST_COLOR;
		dest = GU_FIX;
		srcfix = 0;
		destfix = 0;
		break;
	case DX_BLENDMODE_INVSRC:
		op = GU_ADD;
		src = GU_SRC_ALPHA;
		dest = GU_ONE_MINUS_SRC_ALPHA;
		srcfix = 0;
		destfix = 0;
		break;
	default:
		return 0;
	}
	GUENABLE(GU_BLEND);
	sceGuBlendFunc(op,src,dest,srcfix,destfix);
	u32 r,g,b,a,t;
	r = color & 0x000000ff;
	t = dxpGraphicsData.color & 0x000000ff;
	r *= t;
	r /= 255;
	g = (color >> 8) & 0x000000ff;
	t = (dxpGraphicsData.color >> 8) & 0x000000ff;
	g *= t;
	g /= 255;
	b = (color >> 16) & 0x000000ff;
	t = (dxpGraphicsData.color >> 16) & 0x000000ff;
	b *= t;
	b /= 255;
	a = (color >> 24) & 0x000000ff;
	t = (dxpGraphicsData.color >> 24) & 0x000000ff;
	a *= t;
	a /= 255;
	t = (r & 0x000000ff) |
		((g & 0x000000ff) << 8) |
		((b & 0x000000ff) << 16) |
		((a & 0x000000ff) << 24);

	switch(dxpGraphicsData.blendmode)
	{
	case DX_BLENDMODE_NOBLEND:
	case DX_BLENDMODE_MUL:
	case DX_BLENDMODE_DESTCOLOR:
		t |= 0xff000000;
		break;
	case DX_BLENDMODE_INVSRC:
		t = (t & 0xff000000) | (~t & 0x00ffffff);
		break;
	}
	return t;
}																					

void dxpFontIntrafontFinish(void)
{
	sceGuSetAllStatus(gustatus);
	sceGuScissor(
		dxpGraphicsData.intrafont_scissor[0],
		dxpGraphicsData.intrafont_scissor[1],
		dxpGraphicsData.intrafont_scissor[2],
		dxpGraphicsData.intrafont_scissor[3]
	);
}

DXPFONTHANDLE* dxpFontReserveHandle()
{
	int i;
	if(!dxpFontData.init)dxpFontInit();
	for(i = 0;i < DXP_BUILDOPTION_FONTHANDLE_MAX;++i)
		if(!dxpFontArray[i].used)break;
	if(i >= DXP_BUILDOPTION_FONTHANDLE_MAX)return NULL;
	dxpFontArray[i].used = 1;
	dxpFontArray[i].edgeEnable = 1;
	dxpFontArray[i].pif = NULL;
	dxpFontArray[i].scale = 1.0f;
	dxpFontArray[i].edgeColor = DXP_COLOR_DARKGRAY;
	dxpFontArray[i].fontAlign = DXP_FONT_ALIGN_DEFAULT;
	return dxpFontArray + i;
}

void dxpFontReleaseHandle(int handle)
{
	if(handle < 0 || handle >= DXP_BUILDOPTION_FONTHANDLE_MAX)return;
	if(!dxpFontArray[handle].used)return;
	intraFontUnload(dxpFontArray[handle].pif);
	dxpFontArray[handle].used = 0;
}


int DrawStringToHandle(int x,int y,const char *str,int color,int handle,int edgecolor)
{
	if(!dxpFontData.init)dxpFontInit();
	if(handle < 0 || handle >= DXP_BUILDOPTION_FONTHANDLE_MAX)return -1;
	DXPFONTHANDLE *pHnd;
	pHnd = &dxpFontArray[handle];
	if(!pHnd->used)return -1;
	if(!edgecolor)edgecolor = pHnd->edgeColor;
	if(!pHnd->edgeEnable)edgecolor = 0;
	u32 fcolor = dxpFontIntrafontStart(color);
	intraFontSetStyle(pHnd->pif,pHnd->scale,fcolor,edgecolor,pHnd->fontAlign);
	intraFontPrint(pHnd->pif,x,y + DXP_FONT_DEFAULT_SIZE * pHnd->scale,str);
	dxpFontIntrafontFinish();
	return 0;
}

int DrawString(int x,int y,const char *str,int color,int edgecolor)
{
	if(!dxpFontData.init)dxpFontInit();
	return DrawStringToHandle(x,y,str,color,0,edgecolor);
}

int DrawFormatString(int x,int y,int color,const char *format,...)
{
	char strbuf[1024];
	va_list arg;
	va_start(arg,format);
	vsnprintf(strbuf,1024,format,arg);
	va_end(arg);
	return DrawString(x,y,strbuf,color,0);
}

int	DrawFormatStringToHandle(int x,int y,int color,int handle,const char *format,...)
{
	char str[1024];
	va_list arg;
	va_start(arg,format);
	vsnprintf(str,1024,format,arg);
	va_end(arg);
	return DrawStringToHandle(x,y,str,color,handle,0);
}


int SetFontSizeF(float size)
{
	if(!dxpFontData.init)dxpFontInit();
	dxpFontArray[0].scale = size / 16;
	return 0;
}

int SetFontSize(int size)
{
	return SetFontSizeF(size);
}

int SetFontThickness(int thickness)
{
	return 0;
}

int ChangeFont(const char *fontname,int charset)
{
	if(!dxpFontData.init)dxpFontInit();
	intraFont *pif;
	pif = intraFontLoad(fontname,charset);
	if(!pif)return -1;
	intraFontUnload(dxpFontArray[0].pif);
	dxpFontArray[0].pif = pif;
	return 0;
}

int ChangeFontType(int type)
{
	if(type == -1)type = 0;
	if(!dxpFontData.init)dxpFontInit();
	if(type < 0 || type > 3)return -1;
	dxpFontArray[0].edgeEnable = type & 1;
	return 0;
}

int InitFontToHandle()
{
	if(!dxpFontData.init)return -1;
	int i;
	for(i = 1;i < DXP_BUILDOPTION_FONTHANDLE_MAX;++i)
		DeleteFontToHandle(i);
	return 0;
}

