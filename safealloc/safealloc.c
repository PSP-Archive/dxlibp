#include "../safealloc.h"
#include <stdio.h>
#include <malloc.h>
//
//
//
//
//
//
//
//
//typedef struct DXPSAFEALLOCDATA
//{
//	unsigned init : 1;
//	SceUID msgbox;
//}DXPSAFEALLOCDATA;
//
//typedef struct DXPSAFEALLOC_MSG
//{
//	SceKernelMsgPacket packet;
//	u32 *size;
//	void **address;
//}DXPSAFEALLOC_MSG;
//
//DXPSAFEALLOCDATA dxpSafeAllocData = {0,-1};
//
//void dxpSafeAllocInit()
//{
//	if(dxpSafeAllocData.init)return;
//	dxpSafeAllocData.msgbox = sceKernelCreateMbx("dxp safe alloc msgbox",0,0);
//	if(dxpSafeAllocData.msgbox >= 0)dxpSafeAllocData.init = 1;
//}
//
//void dxpSafeAllocEnd()
//{
//	if(!dxpSafeAllocData.init)return;
//	sceKernelDeleteMbx(dxpSafeAllocData.msgbox);
//	dxpSafeAllocData.msgbox = -1;
//	dxpSafeAllocData.init = 0;
//}
//
//void* dxpSafeAlloc(u32 size)
//{
//	if(!dxpSafeAllocData.init || !size)return NULL;
//	void *address = 0;
//	DXPSAFEALLOC_MSG msg;
//	msg.size = &size;
//	msg.address = &address;
//	sceKernelSendMbx(dxpSafeAllocData.msgbox,&msg);
//	while(size)
//		sceKernelDelayThread(100);
//	return address;
//}
//
//void dxpSafeFree(void *ptr)
//{
//	if(!dxpSafeAllocData.init || !ptr)return;
//	u32 size = 0;
//	DXPSAFEALLOC_MSG msg;
//	msg.address = &ptr;
//	msg.size = &size;
//	sceKernelSendMbx(dxpSafeAllocData.msgbox,&msg);
//	while(!ptr)
//		sceKernelDelayThread(100);
//}
//
//void dxpSafeAllocMain()
//{
//	if(!dxpSafeAllocData.init)return;
//	DXPSAFEALLOC_MSG *pmsg;
//
//	while(sceKernelPollMbx(dxpSafeAllocData.msgbox,(void**)&pmsg) >= 0)
//	{
//		if(!pmsg)continue;
//		if(pmsg->size)
//		{
//			*pmsg->address = memalign(64,*pmsg->size);
//			*pmsg->size = 0;
//		}else
//		{
//			free(*pmsg->address);
//			*pmsg->address = NULL;
//		}
//	}
//}
//

void* dxpSafeAlloc(u32 size)
{
	if(!size)return NULL;
	int blockId = sceKernelAllocPartitionMemory(PSP_MEMORY_PARTITION_USER,"dxp threadsafe alloc",PSP_SMEM_Low,size + 64,0);
	if(blockId < 0)return NULL;
	u32 addr = (u32)sceKernelGetBlockHeadAddr(blockId),addr2;
	if(!addr)
	{
		sceKernelFreePartitionMemory(blockId);
		return NULL;
	}
	addr2 = addr = (addr + 64) & ~63;
	addr2 -= 4;
	*(int*)addr2 = blockId;
	return (void*)addr;
}

void dxpSafeFree(void *addr)
{
	if(!addr)return;
	int *blockIdPtr = (int*)addr;
	sceKernelFreePartitionMemory(*(--blockIdPtr));
}