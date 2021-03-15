#include "dxlibp.h"
#include <psputility.h>

//build option ----

//macros ----

//structures ----

typedef struct DXPGENERALDATA__
{
	unsigned initialized : 1;//初期化フラグ

	unsigned homebutton_callback_initialized : 1;//HOMEボタン関連
	unsigned homebutton_pushed : 1;
	unsigned exit_called : 1;
	SceUID homebutton_callback_threadid;

	int randmode;//乱数関連
	SceKernelUtilsMt19937Context *mt19937context;

	int charset;//文字セットの指定　OSKやprintfDxの為の変換に使う
}DXPGENERALDATA;

//variables ----

extern DXPGENERALDATA dxpGeneralData;
