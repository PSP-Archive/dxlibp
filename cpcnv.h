#include "dxlibp.h"

#define DXPCPTABLE_MAX 20

#define DXPCPTABLE_FILENAME "flash0:/vsh/etc/cptbl.dat"

#define DXPCP_TOUCS2	0
#define DXPCP_FROMUCS2	1

typedef u16 dxpUcs2;
typedef u8 dxpChar;

typedef struct DXPCPTABLE_HEADER__ //PSP内臓の文字コード変換テーブルで使うヘッダ
{
	u16 startcode;
	u16 endcode;
	u16 offset;
}DXPCPTABLE_HEADER;


typedef struct DXPCPTABLE__
{
	u8 used;
	void* ptr;
	void* end;
}DXPCPTABLE;

typedef struct DXPCPDATA__
{
	unsigned init:1;
	dxpUcs2 errcode_ucs2;
	DXPCPTABLE table[DXPCPTABLE_MAX][2];
}DXPCPDATA;

extern DXPCPDATA dxpCpData;

void dxpCpInit();
int dxpCpLoadTable(const char *filename,u8 cp,u8 direction);

int MultiByteCharCheck( const char *Buf, int CharSet /* DX_CHARSET_SHFTJIS */ );;

int dxpCpSJIS_MultiByteCharCheck(dxpChar code);
int dxpCpSJIS_Strlen(const dxpChar *str);
int dxpCpSJIS_toUcs2(dxpUcs2 *dst,u32 count,const dxpChar *src);
int dxpCpSJIS_fromUcs2(dxpChar *dst,u32 count,const dxpUcs2 *src);

int dxpCpUTF8_toUcs2(dxpUcs2 *dst,u32 count,const dxpChar *src);
int dxpCpUTF8_fromUcs2(dxpChar *dst,u32 count,const dxpUcs2 *src);
int dxpCpUTF8_Strlen(const dxpChar *str);
static inline int dxpCpUTF8_Charlen(const dxpChar *code)
{
	if(!code)return -1;
	if(*code <= 0x7f)return 1;
	if(*code <= 0xc1)return -1;
	if(*code <= 0xdf)return 2;
	if(*code <= 0xef)return 3;
	if(*code <= 0xf7)return 4;
	if(*code <= 0xfb)return 5;
	if(*code <= 0xfd)return 6;
	return -1;
}

int dxpCpCode_Strlen(const dxpChar *str,u8 cp);
int dxpCpCode_toUcs2(dxpUcs2 *dst,u32 count,const dxpChar *src,u8 cp);
int dxpCpCode_fromUcs2(dxpChar *dst,u32 count,const dxpUcs2 *src,u8 cp);
int dxpCpUCS2_Strlen(const dxpUcs2 *str);