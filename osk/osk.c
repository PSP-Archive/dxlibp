#include "../dxlibp.h"
#include "../general.h"
#include "../cpcnv.h"
#include <psputility.h>
#include <pspgu.h>
#include "../graphics.h"
#include <malloc.h>
#include <string.h>

int GetTextOSK(char *buf,int buflen,int mode,const char *title,const char *init,int lllang)
{
	if(buf == NULL)return -1;

	u16 *winit,*wtitle,*wresult;
	char nulstr[] = "";
	if(init == NULL)init = nulstr;
	if(title == NULL)title = nulstr;
	winit = malloc((strlen(init) + 1) * 2);
	wtitle = malloc((strlen(title) + 1) * 2);
	wresult = malloc((buflen + 1) * 2);
	if(winit == NULL || wtitle == NULL || wresult == NULL)
	{
		free(winit);
		free(wtitle);
		free(wresult);
		return -1;
	}


	dxpCpCode_toUcs2(winit,strlen(init) + 1,(u8*)init,dxpGeneralData.charset);
	dxpCpCode_toUcs2(wtitle,strlen(title) + 1,(u8*)title,dxpGeneralData.charset);

	SceUtilityOskData data;
	memset(&data, 0, sizeof(SceUtilityOskData));
	sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &data.language);
	data.unk_00 = 1;	//ATOK
	data.lines = 1;
	data.unk_24 = 1;
	data.inputtype = mode;
	data.desc = wtitle;
	data.intext = winit;
	data.outtextlength = buflen;
	data.outtextlimit = buflen;
	data.outtext = wresult;

	SceUtilityOskParams params;
	memset(&params,0x00,sizeof(params));
	params.base.size = sizeof(params);
	sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE,&params.base.language);
	params.base.language = lllang;
	sceUtilitySetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE,lllang);
	sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_UNKNOWN,&params.base.buttonSwap);
	params.base.graphicsThread = 0x11;
	params.base.accessThread = 0x13;
	params.base.fontThread = 0x12;
	params.base.soundThread = 0x10;
	params.datacount = 1;
	params.data = &data;

	sceUtilityOskInitStart(&params);

	int done = 0;
	while(ProcessMessage() != -1 && !done)
	{
		ClearDrawScreen();
		GUFINISH
		switch(sceUtilityOskGetStatus())
		{
			case PSP_UTILITY_DIALOG_INIT:
				break;
			
			case PSP_UTILITY_DIALOG_VISIBLE:
				sceUtilityOskUpdate(1);
				break;
			
			case PSP_UTILITY_DIALOG_QUIT:
				sceUtilityOskShutdownStart();
				break;
			
			case PSP_UTILITY_DIALOG_FINISHED:
				break;
				
			case PSP_UTILITY_DIALOG_NONE:
				done = 1;
				
			default :
				break;
		}
		ScreenFlip();
	}

//	dxpCpSJIS_fromUcs2((u8*)buf,buflen,wresult);
	dxpCpCode_fromUcs2((u8*)buf,buflen,wresult,dxpGeneralData.charset);
	free(winit);
	free(wtitle);
	free(wresult);
	return 0;
}
