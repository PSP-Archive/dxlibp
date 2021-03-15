#include <libc/stdio.h>

#define LOGFILENAME "dxplog.txt"

int AppLogAdd(const char *format,...)
{
	va_list args;
	va_start(args,format);
	if(!format)return -1;
	FILE *fp = fopen(LOGFILENAME,"a");
	if(!fp)return -1;
	vfprintf(fp,format,args);
	fprintf(fp,"\n");
	fclose(fp);
	va_end(args);
	return 0;
}
