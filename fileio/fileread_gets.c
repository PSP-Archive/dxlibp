#include "../fileio.h"

int FileRead_gets(char *buffer,int buffersize,int filehandle)
{
	int i,c;
	for(i = 0;i < buffersize;++i)
	{
		c = FileRead_getc(filehandle);
		if(c == '\0' || c == '\n' || c == -1)break;
		buffer[i] = c;
	}
	buffer[i] = '\0';
	return i;
}