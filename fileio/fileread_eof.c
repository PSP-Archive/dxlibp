#include "../fileio.h"

int FileRead_eof(int filehandle)
{
	if(filehandle < 0 || filehandle >= DXP_BUILDOPTION_FILEHANDLE_MAX)return -1;
	DXPFILEIOHANDLE *pHnd;
	pHnd = &dxpFileioData.handleArray[filehandle];
	if(!pHnd->used)return 0;
	if(pHnd->pos >= pHnd->pos)
	{
		pHnd->pos = pHnd->size;
		return 1;
	}
	return 0;
}