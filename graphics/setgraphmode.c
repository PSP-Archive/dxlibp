#include "../graphics.h"
#include "../general.h"

int SetGraphMode(int xsize,int ysize,int bpp)
{
	GUINITCHECK;
	GUSYNC;
	if(xsize != 480 || ysize != 272)return DX_CHANGESCREEN_RETURN;
	if(bpp == 16)bpp = DXP_FMT_5551;
	if(bpp == 32)bpp = DXP_FMT_8888;
	if(bpp < 0 || bpp > 3)return DX_CHANGESCREEN_RETURN;
	if(bpp == dxpGraphicsData.display_psm)return DX_CHANGESCREEN_OK;

//	sceGuDisplay(GU_FALSE);
	GUSTART;
	if(bpp == DXP_FMT_8888 || dxpGraphicsData.display_psm == DXP_FMT_8888)
	{//16->32,32->16
		int size = vgetMemorySize(512,272,bpp);
		vfree(dxpGraphicsData.displaybuffer[0].texvram);
		vfree(dxpGraphicsData.displaybuffer[1].texvram);
		dxpGraphicsData.displaybuffer[0].texvram = valloc(size);
		dxpGraphicsData.displaybuffer[1].texvram = valloc(size);
		if(!dxpGraphicsData.displaybuffer[0].texvram || !dxpGraphicsData.displaybuffer[1].texvram)
		{
			vfree(dxpGraphicsData.displaybuffer[0].texvram);
			vfree(dxpGraphicsData.displaybuffer[1].texvram);
			size = vgetMemorySize(512,272,dxpGraphicsData.display_psm);
			dxpGraphicsData.displaybuffer[0].texvram = valloc(size);
			dxpGraphicsData.displaybuffer[1].texvram = valloc(size);
			sceGuDrawBuffer(bpp,vGuPointer(GetFramebufferAddress()),512);
			sceGuDispBuffer(480,272,vGuPointer(GetDisplaybufferAddress()),512);
			GUSYNC;
//			sceDisplayWaitVblankStart();
//			GUSTART;
//			sceGuDisplay(GU_TRUE);
			return DX_CHANGESCREEN_RETURN;
		}
	}

	dxpGraphicsData.display_psm = bpp;
	dxpGraphicsData.displaybuffer[0].psm = bpp;
	dxpGraphicsData.displaybuffer[1].psm = bpp;
	sceGuDrawBuffer(bpp,vGuPointer(GetFramebufferAddress()),512);
	sceGuDispBuffer(480,272,vGuPointer(GetDisplaybufferAddress()),512);
	GUSYNC;
//	sceDisplayWaitVblankStart();
//	GUSTART;
//	sceGuDisplay(GU_TRUE);
	return DX_CHANGESCREEN_OK;
}