#include "dxlibp.h"
#define MAXSTRBUF_X	80
#define MAXSTRBUF_Y	23

typedef struct DXPDEBUGDATA__
{
	unsigned init:1;
	unsigned char cx;
	unsigned char cy;
	unsigned char l1;
	int fontsize[2];
	int strbufsize[2];
}DXPDEBUGDATA;

typedef struct DXP_DEBUG_BUF__
{
	char strbuf[MAXSTRBUF_Y][MAXSTRBUF_X + 1];
	char vfsbuf[2048];
	char cpbuf[2048];
}DXP_DEBUG_BUF;

int AppLogAdd(const char *format,...);
int printfDx(const char *format,...);
int clsDx();

void dxpDebugInit();

void dxpDebugDrawString(int x,int y,u32 color,u32 bgcolor,const char *str);//�f�o�b�O������`��Ɏg���`��֐��Q��������ׂ��֐��̖��O�ƌ^
int dxpDebugDrawStringInit(int *fwidth,int *fheight);//�f�o�b�O������`��Ɏg���`��֐��Q�̏������֐�

extern DXPDEBUGDATA dxpDebugData;
extern DXP_DEBUG_BUF dxpDebugBuf;

void dxpDrawDebugScreen();