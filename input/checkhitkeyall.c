#include "../input.h"

int CheckHitKeyAll()
{
	int i;
	for(i = 0;i < 256;++i)
		if(dxpInputData.key[i])return 1;
	return 0;
}