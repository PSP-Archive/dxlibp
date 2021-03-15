#include "../fileio.h"
#include <pspsdk.h>

int FileRead_close(int filehandle)
{
	//int istate;
	DXPFILEIOHANDLE *pHnd;
	if(filehandle < 0 || filehandle >= DXP_BUILDOPTION_FILEHANDLE_MAX)return -1;
	pHnd = &dxpFileioData.handleArray[filehandle];
	//istate = pspSdkDisableInterrupts();
	if(!pHnd->used)return -1;
	if(!pHnd->onmemory)
	{
		sceIoClose(pHnd->fd);
	}
	pHnd->used = 0;
	//pspSdkEnableInterrupts(istate);
	return 0;
}