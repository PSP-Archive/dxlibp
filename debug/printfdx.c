#include "../debug.h"
#include "../general.h"
#include <libc/stdio.h>
#include "../cpcnv.h"


static void br()
{
	dxpDebugBuf.strbuf[dxpDebugData.cy][dxpDebugData.cx] = '\0';
	dxpDebugData.cx = 0;
	dxpDebugData.cy += 1;
	dxpDebugData.cy %= dxpDebugData.strbufsize[1];

	if(dxpDebugData.cy == 0)dxpDebugData.l1 = dxpDebugData.strbufsize[1];
	if(dxpDebugData.l1)dxpDebugData.l1 = (dxpDebugData.l1 + 1) % dxpDebugData.strbufsize[1] + dxpDebugData.strbufsize[1];	
	dxpDebugBuf.strbuf[(dxpDebugData.l1 - 1) % dxpDebugData.strbufsize[1]][0] = '\0';
}

void dxpDrawDebugScreen()
{
	int i;
	for(i = 0;i < dxpDebugData.strbufsize[1];++i)
	{
		dxpDebugDrawString(0,dxpDebugData.fontsize[1] * i,0xffffffff,0xff000000,dxpDebugBuf.strbuf[(i + dxpDebugData.l1) % dxpDebugData.strbufsize[1]]);
	}
}


int printfDx(const char *format,...)
{
	if(!dxpDebugData.init)dxpDebugInit();



	va_list vlist;
	va_start(vlist,format);
	vsnprintf(dxpDebugBuf.vfsbuf,2048,format,vlist);
	va_end(vlist);
	dxpDebugBuf.vfsbuf[2046] = '\0';
	dxpDebugBuf.vfsbuf[2047] = '\0';
	//vfsbuf‚É•\Ž¦“à—eŠi”[
	int i;
	for(i = 0;dxpDebugBuf.vfsbuf[i] != '\0' && i < 2048;++i)
	{
		if(dxpCpSJIS_MultiByteCharCheck(dxpDebugBuf.vfsbuf[i]))
		{
			if(dxpDebugData.cx >= dxpDebugData.strbufsize[0])br();
			dxpDebugBuf.strbuf[dxpDebugData.cy][dxpDebugData.cx] = dxpDebugBuf.vfsbuf[i];
			dxpDebugBuf.strbuf[dxpDebugData.cy][dxpDebugData.cx + 1] = dxpDebugBuf.vfsbuf[i + 1];
			++i;
			dxpDebugData.cx += 2;
		}
		else
		{
			if(dxpDebugBuf.vfsbuf[i] == '\n')
			{
				br();
				continue;
			}
			if(dxpDebugData.cx >= dxpDebugData.strbufsize[0])br();
			dxpDebugBuf.strbuf[dxpDebugData.cy][dxpDebugData.cx] = dxpDebugBuf.vfsbuf[i];
			dxpDebugData.cx += 1;
		}
	}
	dxpDebugBuf.strbuf[dxpDebugData.cy][dxpDebugData.cx] = '\0';
	return 0;
}
