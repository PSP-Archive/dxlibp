#include "../graphics.h"
#include <malloc.h>
#include <string.h>

int ConvertGraphFormat(int gh,int psm)
{
	GUINITCHECK;
	GUSYNC;
	u32 size,i;
	int swizzleflag = 0;
	DXPTEXTURE3 *texptr = dxpGraphHandle2TexPtr(gh);
	if(!texptr)return -1;
	if(texptr->psm == psm)return 0;
	if(texptr->swizzledflag)
	{
		swizzleflag = 1;
		UnswizzleGraph(gh);
		if(texptr->swizzledflag)return -1;
	}
	void *src,*dest;
	u16 *src16,*dest16;//‚±‚Ì‚ ‚½‚è‚¿‚å‚¢–³‘Ê
	u32 *src32,*dest32;
	if(psm != GU_PSM_4444 && psm != GU_PSM_5551 && psm != GU_PSM_5650 && psm != GU_PSM_8888)return -1; 
	if(texptr->psm != GU_PSM_4444 && texptr->psm != GU_PSM_5551 && texptr->psm != GU_PSM_5650 && texptr->psm != GU_PSM_8888)return -1; 
	if(texptr->texvram)
	{
		if(!texptr->texdata)return -1;
		src = texptr->texvram;
		dest = texptr->texdata;
	}
	else
	{
		src = texptr->texdata;
		dest = malloc(dxpGraphicsCalcTexSize(texptr->pitch,texptr->height,texptr->psm));
		if(!dest)return -1;
	}
	src16 = src;
	dest16 = dest;
	src32 = src;
	dest32 = dest;

	size = texptr->width * texptr->height;

	switch(texptr->psm)
	{
	case GU_PSM_4444:
		switch(psm)
		{
		case GU_PSM_5551:
			for(i = 0;i < size;++i)*dest16++ = dxpGraphics8888_5551(dxpGraphics4444_8888(*src16++));
			break;
		case GU_PSM_5650:
			for(i = 0;i < size;++i)*dest16++ = dxpGraphics8888_5650(dxpGraphics4444_8888(*src16++));
			break;
		case GU_PSM_8888:
			for(i = 0;i < size;++i)*dest32++ = dxpGraphics4444_8888(*src16++);
			break;
		}
		break;
	case GU_PSM_5650:
		switch(psm)
		{
		case GU_PSM_4444:
			for(i = 0;i < size;++i)*dest16++ = dxpGraphics8888_4444(dxpGraphics5650_8888(*src16++));
			break;
		case GU_PSM_5551:
			for(i = 0;i < size;++i)*dest16++ = dxpGraphics8888_5551(dxpGraphics5650_8888(*src16++));
			break;
		case GU_PSM_8888:
			for(i = 0;i < size;++i)*dest32++ = dxpGraphics5650_8888(*src16++);
			break;
		}
		break;
	case GU_PSM_5551:
		switch(psm)
		{
		case GU_PSM_4444:
			for(i = 0;i < size;++i)*dest16++ = dxpGraphics8888_4444(dxpGraphics5551_8888(*src16++));
			break;
		case GU_PSM_5650:
			for(i = 0;i < size;++i)*dest16++ = dxpGraphics8888_5650(dxpGraphics5551_8888(*src16++));
			break;
		case GU_PSM_8888:
			for(i = 0;i < size;++i)*dest32++ = dxpGraphics8888_5551(*src16++);
			break;
		}
		break;
	case GU_PSM_8888:
		switch(psm)
		{
		case GU_PSM_4444:
			for(i = 0;i < size;++i)*dest16++ = dxpGraphics8888_4444(*src32++);
			break;
		case GU_PSM_5551:
			for(i = 0;i < size;++i)*dest16++ = dxpGraphics8888_5551(*src32++);
			break;
		case GU_PSM_5650:
			for(i = 0;i < size;++i)*dest16++ = dxpGraphics8888_5650(*src32++);
			break;
		}
		break;
	}

	size = dxpGraphicsCalcTexSize(texptr->pitch,texptr->height,texptr->psm);
	memcpy(src,dest,size);	
	if(!texptr->texvram)free(dest);
	sceKernelDcacheWritebackAll();
	texptr->psm = psm;
	texptr->reloadflag = 1;
	if(swizzleflag)SwizzleGraph(gh);
	return 0;
}
