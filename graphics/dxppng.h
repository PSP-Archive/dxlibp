/*
png decoder for dx library portable.
written by yreeen
ver1.0.3
last update 2009/10/27

change log
1.0.0 release
1.0.1 bug fix
1.0.2 bug fix
1.0.3 support alpha info exist check

require::zlib

PNG Specification, Version 1.2

mode:DXPPNG_MODE_GPU
sizeLimit:	512x512
			2_nSize
			16bitPitchAlignment

clut1,2,4 -> T4
clut8 -> T8
gray1,2,4 -> T4
gray8,16 -> T8
grayalpha8,16 -> 8888
rgb8,16 -> 8888
rgba8,16 -> 8888

mode:DXPPNG_MODE_RAW
sizeLimit:	depend for memory capacity
allFormat -> 8888
*/


/*
sample code

#include <stdio.h>
#include "dxppng.h"
int main()
{
	DXPPNG png;
	DXPPNG_PARAMS param;
	FILE *fp;

	fp = fopen("image.png","rb");
	params.srcLength = fseek(fp,0,SEEK_END);
	fseek(fp,0,SEEK_SET);
	params.src = malloc(params.srcLength);
	fread(params.src,1,params.srcLength,fp);
	fclose(fp);

	params.mode = DXPPNG_MODE_RAW;
	params.funcs.pfree = 0;//use default malloc/free
	dxppng_decode(&params,&png);

	return 0;
}
*/
#ifndef DXPPNG_H__
#define DXPPNG_H__
#ifdef __cplusplus
extern "C" {
#endif

#define PSM_8888	(3)
#define PSM_T4		(4)
#define PSM_T8		(5)

#define DXPPNG_MODE_GPU	1	/*Minimam psm,Pitch,Alignment,Size limit,etc... optimized for psp's gpu*/
#define DXPPNG_MODE_RAW	0	/*for programmer's immidiate access.*/

typedef struct DXPPNG_
{
	unsigned int width,height;
	unsigned int widthN2,heightN2;
	unsigned int pitch;
	unsigned int psm;
	unsigned int *clut;
	unsigned int clutnum;
	unsigned int alpha;
	void *raw;
}DXPPNG;

typedef struct DXPPNG_FUNCS_
{
	void*(*pmalloc)(unsigned int);
	void*(*pmemalign)(unsigned int,unsigned int);
	void (*pfree)(void*);
}DXPPNG_FUNCS;

typedef struct DXPPNG_PARAMS_
{
	void *src;
	unsigned int srcLength;
	unsigned int mode;
	DXPPNG_FUNCS funcs;
}DXPPNG_PARAMS;


int dxppng_decode(DXPPNG_PARAMS *params,DXPPNG *png);

#ifdef __cplusplus
};
#endif

#endif

