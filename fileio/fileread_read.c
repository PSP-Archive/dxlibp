#include "../fileio.h"

int	FileRead_read(void *buffer,int readsize,int filehandle)
{
	DXPFILEIOHANDLE *pHnd;
	if(filehandle < 0 || filehandle >= DXP_BUILDOPTION_FILEHANDLE_MAX)return -1;
	pHnd = &dxpFileioData.handleArray[filehandle];
	if(!pHnd->used)return -1;
	if(pHnd->onmemory)
	{
		int i;
		for(i = 0;i < readsize && pHnd->pos < pHnd->size;++i,++pHnd->pos)
			((u8*)buffer)[pHnd->pos] = ((u8*)pHnd->dat)[pHnd->pos];
		return i;
	}
	if(dxpFileioData.sleep)
		if(dxpFileioReopen(filehandle) < 0)return -1;
	int retread = sceIoRead(pHnd->fd,buffer,readsize);
	pHnd->pos += retread;
	return retread;
}