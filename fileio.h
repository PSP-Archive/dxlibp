#include "dxlibp.h"
#include <malloc.h>
#include <stdio.h>

//macros ---

//stream
#define STREAM_SEEKTYPE_SET							(PSP_SEEK_SET)
#define STREAM_SEEKTYPE_END							(PSP_SEEK_END)
#define STREAM_SEEKTYPE_CUR							(PSP_SEEK_CUR)

//structures ----

typedef struct DXPFILEIOHANDLE__
{
	unsigned used : 1;
	unsigned onmemory : 1;

	char filename[DXP_BUILDOPTION_FILENAMELENGTH_MAX];

	u32 pos;
	u32 size;

	union
	{
		SceUID fd;
		void *dat;
	};
}DXPFILEIOHANDLE;

typedef struct DXPFILEIODATA__
{
	unsigned init : 1;
	unsigned sleep : 1;
	DXPFILEIOHANDLE handleArray[DXP_BUILDOPTION_FILEHANDLE_MAX];
}DXPFILEIODATA;
//variables ----

extern DXPFILEIODATA dxpFileioData;

//local functions ----



void dxpFileioInit();
int dxpFileioReopen(int handle);