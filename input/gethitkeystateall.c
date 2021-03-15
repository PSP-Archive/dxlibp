#include "../input.h"
#include <string.h>

int GetHitKeyStateAll(char* keystatebuf)
{
	if(!keystatebuf)return -1;
	memcpy(keystatebuf,dxpInputData.key,sizeof(char) * 256);
	return 0;
}
