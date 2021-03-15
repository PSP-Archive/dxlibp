#include "../fileio.h"

int FileRead_size(const char *filename)
{
	SceIoStat stat;
	if(sceIoGetstat(filename,&stat) < 0)return -1;
	return stat.st_size;
}
