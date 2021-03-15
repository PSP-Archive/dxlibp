#include "dxlibp.h"
//スレッドセーフなメモリ確保、開放。６４バイトアラインメントで行われる
//void dxpSafeAllocInit();
//void dxpSafeAllocEnd();
void* dxpSafeAlloc(u32 size);//メモリ確保を要求する。メインスレッドから呼んではいけない。
void dxpSafeFree(void *ptr);//メモリ開放を要求する。メインスレッドから呼んではいけない。
//void dxpSafeAllocMain();//メインスレッドが呼ぶ。mallocやfreeはここで処理される

