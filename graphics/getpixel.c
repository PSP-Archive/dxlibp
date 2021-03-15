#include "../graphics.h"

int GetPixel(int x,int y)
{
	if(!dxpGraphicsData.init)return 0;
	u32 c,psm;
	DXPTEXTURE3 *texptr;
	if(dxpGraphicsData.rendertarget)
	{
		texptr = dxpGraphicsData.rendertarget;
		psm = texptr->psm;
	}
	else
	{
		texptr = dxpGraphicsData.displaybuffer_back;
		psm = dxpGraphicsData.display_psm;
	}
	if(psm == GU_PSM_8888)
		return ((u32*)texptr->texvram)[y * 512 + x];
	else
	{
		c = ((u16*)texptr->texvram)[y * 512 + x];
		switch(psm)
		{
		case GU_PSM_4444:
			return dxpGraphics4444_8888(c);
			break;
		case GU_PSM_5551:
			return dxpGraphics5551_8888(c);
			break;
		case GU_PSM_5650:
			return dxpGraphics5650_8888(c);
			break;
		}
	}
	return 0;
}
