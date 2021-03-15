#include "../general.h"

int SetCodepoint(int codepoint)
{
	switch(codepoint)
	{
	case DXP_CP_SJIS:
		break;
	case DXP_CP_UTF8:
		break;
	default:
		return -1;
	}
	dxpGeneralData.charset = codepoint;
	return 0;
}