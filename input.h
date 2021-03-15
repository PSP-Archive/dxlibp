#include "dxlibp.h"
#include <pspctrl.h>

//macros ----

#define DXP_INPUT_UP_BIT		(4)
#define DXP_INPUT_RIGHT_BIT		(5)
#define DXP_INPUT_DOWN_BIT		(6)
#define DXP_INPUT_LEFT_BIT		(7)
#define DXP_INPUT_TRIANGLE_BIT	(12)
#define DXP_INPUT_CIRCLE_BIT	(13)
#define DXP_INPUT_CROSS_BIT		(14)
#define DXP_INPUT_SQUARE_BIT	(15)
#define DXP_INPUT_LTRIGGER_BIT	(8)
#define DXP_INPUT_RTRIGGER_BIT	(9)
#define DXP_INPUT_START_BIT		(3)
#define DXP_INPUT_SELECT_BIT	(0)

#define PAD_INPUT_DOWN_BIT		(0)
#define PAD_INPUT_LEFT_BIT		(1)
#define PAD_INPUT_RIGHT_BIT		(2)
#define PAD_INPUT_UP_BIT		(3)
#define PAD_INPUT_A_BIT			(4)
#define PAD_INPUT_B_BIT			(5)
#define PAD_INPUT_C_BIT			(6)
#define PAD_INPUT_X_BIT			(7)
#define PAD_INPUT_Y_BIT			(8)
#define PAD_INPUT_Z_BIT			(9)
#define PAD_INPUT_L_BIT			(10)
#define PAD_INPUT_R_BIT			(11)
#define PAD_INPUT_START_BIT		(12)
#define PAD_INPUT_M_BIT			(13)
#define PAD_INPUT_D_BIT			(14)
#define PAD_INPUT_F_BIT			(15)
#define PAD_INPUT_G_BIT			(16)
#define PAD_INPUT_H_BIT			(17)
#define PAD_INPUT_I_BIT			(18)
#define PAD_INPUT_J_BIT			(19)
#define PAD_INPUT_K_BIT			(20)
#define PAD_INPUT_LL_BIT		(21)
#define PAD_INPUT_N_BIT			(22)
#define PAD_INPUT_O_BIT			(23)
#define PAD_INPUT_P_BIT			(24)
#define PAD_INPUT_RR_BIT		(25)
#define PAD_INPUT_S_BIT			(26)
#define PAD_INPUT_T_BIT			(27)
#define PAD_INPUT_U_BIT			(28)
#define PAD_INPUT_V_BIT			(29)
#define PAD_INPUT_W_BIT			(30)
#define PAD_INPUT_XX_BIT		(31)

//structures ----

typedef struct DXPINPUTDATA__
{
	SceCtrlData ctrldata;	//生の入力データ

	int	psp_to_key[256];	//PSP→キーボード変換テーブル
	int	key_to_pad1[32][4];	//キーボード→PAD1変換テーブル

	char key[256];			//キーボードとしての入力状態
	int	pad1;				//PAD1としての入力状態
}DXPINPUTDATA;


//functions ----

void dxpInputInit();
void dxpInputRenew();
int dxpInputExp(int pspinput);

//variables ----

extern DXPINPUTDATA dxpInputData;