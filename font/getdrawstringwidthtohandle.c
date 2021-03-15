#include "../font.h"
#include <libc/stdio.h>
#include <string.h>

float GetDrawStringWidthToHandleF(const char *str,int len,int handle)
{
	if(handle < 0 || handle >= DXP_BUILDOPTION_FONTHANDLE_MAX)return -1;
	DXPFONTHANDLE *pHnd;
	pHnd = &dxpFontArray[handle];
	if(!pHnd->used)return -1;
	return intraFontMeasureTextEx(pHnd->pif,str,len);
}

float GetDrawStringWidthF(const char *str,int len)
{
	return GetDrawStringWidthToHandleF(str,len,0);
}

int GetDrawStringWidthToHandle(const char *str,int len,int handle)
{
	float ret = GetDrawStringWidthToHandleF(str,len,handle);
	return ret < 0 ? -1 : ret;
}

int GetDrawStringWidth(const char *str,int len)
{
	return GetDrawStringWidthToHandle(str,len,0);
}

float GetDrawFormatStringWidthToHandleF(int handle,const char *format, ... )
{
	char str[1024];
	int len;
	va_list arg;
	va_start(arg,format);
	vsnprintf(str,1024,format,arg);
	va_end(arg);
	len = strlen(str);
	return GetDrawStringWidthToHandleF(str,len,handle);
	
}

int GetDrawFormatStringWidthToHandle(int handle,const char *format, ... )
{
	char str[1024];
	int len;
	va_list arg;
	va_start(arg,format);
	vsnprintf(str,1024,format,arg);
	va_end(arg);
	len = strlen(str);
	return GetDrawStringWidthToHandle(str,len,handle);
}

float GetDrawFormatStringWidthF(const char *format, ... )
{
	char str[1024];
	int len;
	va_list arg;
	va_start(arg,format);
	vsnprintf(str,1024,format,arg);
	va_end(arg);
	len = strlen(str);
	return GetDrawStringWidthF(str,len);
}

int GetDrawFormatStringWidth(const char *format, ... )
{
	char str[1024];
	int len;
	va_list arg;
	va_start(arg,format);
	vsnprintf(str,1024,format,arg);
	va_end(arg);
	len = strlen(str);
	return GetDrawStringWidth(str,len);
}
