#include "../fileio.h"
#include <psppower.h>
#include <unistd.h>
#include <string.h>
//variables ----

DXPFILEIODATA dxpFileioData = 
{
	0,
	0,
};

//local functions ----


static int dxpPowerCallback(int unk0,int flag,void* arg)
{
	if(flag & PSP_POWER_CB_SUSPENDING)
	{
		dxpFileioData.sleep = 1;
	}
	if(flag & PSP_POWER_CB_RESUME_COMPLETE)
	{
		dxpFileioData.sleep = 0;
	}
	return 0;
}

static int dxpPowerCallbackThread(SceSize args, void *argp)
{
    int cbid;
	cbid = sceKernelCreateCallback("dxp power callback", dxpPowerCallback, NULL);
    scePowerRegisterCallback(-1, cbid);
    sceKernelSleepThreadCB();
	return 0;
}

static int dxpPowerSetupCallback(void)
{
    int thid = 0;
	thid = sceKernelCreateThread("update_thread", dxpPowerCallbackThread, 0x11, 0xFA0, 0, 0);
    if (thid >= 0)
	sceKernelStartThread(thid, 0, 0);
    return thid;
}

void dxpFileioInit()
{
	int i;
	if(dxpFileioData.init)return;
	for(i = 0;i < DXP_BUILDOPTION_FILEHANDLE_MAX;++i)
	{
		dxpFileioData.handleArray[i].used = 0;
	}
	dxpPowerSetupCallback();
	dxpFileioData.init = 1;
}

int dxpFileioReopen(int handle)
{
	char name[DXP_BUILDOPTION_FILENAMELENGTH_MAX];
	if(handle < 0 || handle >= DXP_BUILDOPTION_FILEHANDLE_MAX)return -1;
	DXPFILEIOHANDLE *pHnd;
	pHnd = &dxpFileioData.handleArray[handle];
	if(!pHnd->used || pHnd->onmemory)return 0;
	while(dxpFileioData.sleep)
	{
		sceKernelDelayThread(100);
	}
	pHnd->fd = sceIoOpen(pHnd->filename,PSP_O_RDONLY,0777);
	if(pHnd->fd == SCE_KERNEL_ERROR_NOCWD)
	{
		getcwd(name,DXP_BUILDOPTION_FILENAMELENGTH_MAX);
		int len = strlen(name);
		if(len >= DXP_BUILDOPTION_FILENAMELENGTH_MAX)return -1;
		name[len] = '/';
		strncpy(name + len + 1,pHnd->filename,DXP_BUILDOPTION_FILENAMELENGTH_MAX - len - 1);
		pHnd->fd = sceIoOpen(name,PSP_O_RDONLY,0777);
		if(pHnd->fd >= 0)strncpy(pHnd->filename,name,DXP_BUILDOPTION_FILENAMELENGTH_MAX);
	}
	if(pHnd->fd < 0)return -1;
	sceIoLseek32(pHnd->fd,pHnd->pos,SEEK_SET);
	return 0;
}

