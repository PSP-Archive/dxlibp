#include "../../cpcnv.h"


int dxpCpSJIS_MultiByteCharCheck(dxpChar code)
{
	return ((code >= 0x81) && (code < 0xa0)) || ((code >= 0xe0) && (code <= 0xfd)) ? 1 : 0;
}
