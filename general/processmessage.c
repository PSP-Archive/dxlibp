#include "../general.h"
#include "../input.h"
#include "../safealloc.h"
int ProcessMessage()
{
	//dxpSafeAllocMain();
	if(dxpGeneralData.homebutton_pushed)return -1;
	
	dxpInputRenew();
	return 0;
}