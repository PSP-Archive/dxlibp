#include "dxlibp.h"
#include <psputility.h>

//build option ----

//macros ----

//structures ----

typedef struct DXPGENERALDATA__
{
	unsigned initialized : 1;//�������t���O

	unsigned homebutton_callback_initialized : 1;//HOME�{�^���֘A
	unsigned homebutton_pushed : 1;
	unsigned exit_called : 1;
	SceUID homebutton_callback_threadid;

	int randmode;//�����֘A
	SceKernelUtilsMt19937Context *mt19937context;

	int charset;//�����Z�b�g�̎w��@OSK��printfDx�ׂ̈̕ϊ��Ɏg��
}DXPGENERALDATA;

//variables ----

extern DXPGENERALDATA dxpGeneralData;
