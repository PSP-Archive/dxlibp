#include "../fileio.h"

int FileRead_tell(int filehandle)
{
	if(filehandle < 0 || filehandle >= DXP_BUILDOPTION_FILEHANDLE_MAX)return -1;
	if(!dxpFileioData.handleArray[filehandle].used)return -1;
	return dxpFileioData.handleArray[filehandle].pos;
}