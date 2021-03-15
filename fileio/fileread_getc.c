#include "../fileio.h"
#include <stdio.h>
int FileRead_getc(int filehandle)
{
	if(filehandle < 0 || filehandle >= DXP_BUILDOPTION_FILEHANDLE_MAX)return -1;
	DXPFILEIOHANDLE *pHnd;
	pHnd = &dxpFileioData.handleArray[filehandle];
	if(!pHnd->used)return 0;
	if(pHnd->onmemory)
	{
		if(pHnd->pos >= pHnd->size)
		{
			pHnd->pos = pHnd->size;
			return -1;
		}
		return ((u8*)pHnd->dat)[pHnd->pos++];
	}
	if(dxpFileioData.sleep)
		if(dxpFileioReopen(filehandle) < 0)return -1;

	char c;
	int status;
	status = sceIoRead(dxpFileioData.handleArray[filehandle].fd,&c,1);
	if(status != 1)return -1;
	pHnd->pos += 1;
	return c;
}