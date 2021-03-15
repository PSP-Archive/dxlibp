#include "../../graphics.h"
#ifndef DXP_BUILDOPTION_NO3D


int dxpGraphicsSetup3DTex(DXPGRAPHICSHANDLE *gptr,int flag)
{
	if(!gptr)return -1;
	DXPTEXTURE3 *texptr = gptr->tex;
	if(!texptr)return -1;
	//設定が同じなら変更処理をしない
	if(dxpGraphicsData.drawstate == DXP_DRAWSTATE_TEX3D
		&& flag == dxpGraphicsData.flag
		&& texptr == dxpGraphicsData.texture
		&& !texptr->reloadflag
		&& !dxpGraphicsData.forceupdate
		)return 0;
	GUSTART;
	if(dxpGraphicsData.drawstate != DXP_DRAWSTATE_TEX3D)
	{//2Dテクスチャ用の描画設定を行う。
		dxpGraphicsData.drawstate = DXP_DRAWSTATE_TEX3D;
		dxpGraphicsData.forceupdate = 1;
		GUENABLE(GU_TEXTURE_2D);
		if(dxpGraphicsData.bilinear_enable)
			sceGuTexFilter(GU_LINEAR,GU_LINEAR);
		else
			sceGuTexFilter(GU_NEAREST,GU_NEAREST);
		if(dxpGraphicsData.usedepth)
		{
			GUENABLE(GU_DEPTH_TEST);
			sceGuDepthFunc(dxpGraphicsData.depthfunc);
			sceGuDepthBuffer(dxpGraphicsData.depthbuffer.texvram,512);	//深度バッファを有効にするときでいい
			if(dxpGraphicsData.writedepth)sceGuDepthMask(0);
			else sceGuDepthMask(1);
		}
		else GUDISABLE(GU_DEPTH_TEST);
	}
	//テクスチャの設定開始
	if(texptr != dxpGraphicsData.texture || texptr->reloadflag || dxpGraphicsData.forceupdate)
	{//GUにテクスチャの指定をする
		if(texptr->ppalette && (texptr->psm == GU_PSM_T8 || texptr->psm == GU_PSM_T4))
		{//パレットの設定
			sceGuClutMode(GU_PSM_8888,0,0xff,0);
			sceGuClutLoad(texptr->psm == GU_PSM_T4 ? 2 : 32,texptr->ppalette);
		}
		sceGuTexMode(texptr->psm,0,0,texptr->swizzledflag);
		sceGuTexImage(0,texptr->width,texptr->height,texptr->pitch,texptr->texvram ? texptr->texvram : texptr->texdata);
		dxpGraphicsData.texture = texptr;
		if(texptr != &dxpGraphicsData.displaybuffer[0] && texptr != &dxpGraphicsData.displaybuffer[1])texptr->reloadflag = 0;
	}
	sceGuTexOffset((float)gptr->u0 / (float)texptr->width,(float)gptr->v0 / (float)texptr->height);
	sceGuTexScale((float)(gptr->u1 - gptr->u0) / (float)texptr->width,(float)(gptr->v1 - gptr->v0) / (float)texptr->height);

	//sceGuEnable(GU_CULL_FACE);
	//sceGuFrontFace(GU_CCW);

	u8 colorKey = 0,alphaEnable = 0;
	
	if(flag)
	{
		if(texptr->alphabit)
			alphaEnable = 1;
		else colorKey = 1;
	}
	if(colorKey)
	{
		GUENABLE(GU_COLOR_TEST);
		sceGuColorFunc(GU_NOTEQUAL,texptr->colorkey,0x00fefefe);
	}
	else
	{
		GUDISABLE(GU_COLOR_TEST);
	}
	int op;
	int src,dest;
	unsigned int srcfix;
	unsigned int destfix;	 
	switch(dxpGraphicsData.blendmode)
	{
	case DX_BLENDMODE_NOBLEND:
		op = GU_ADD;
		src = GU_FIX;
		dest = GU_FIX;
		srcfix = 0xffffffff;
		destfix = 0;
		if(!alphaEnable)break;
	case DX_BLENDMODE_ALPHA:
		op = GU_ADD;
		src = GU_SRC_ALPHA;
		dest = GU_ONE_MINUS_SRC_ALPHA;
		srcfix = 0;
		destfix = 0;
		break;
	case DX_BLENDMODE_ADD:
		op = GU_ADD;
		src = GU_SRC_ALPHA;
		dest = GU_FIX;
		srcfix = 0xffffffff;
		destfix = 0xffffffff;
		break;
	case DX_BLENDMODE_SUB:
		op = GU_REVERSE_SUBTRACT;
		src = GU_SRC_ALPHA;
		dest = GU_FIX;
		srcfix = 0xffffffff;
		destfix = 0xffffffff;
		break;
	case DX_BLENDMODE_MUL:
		op = GU_ADD;
		src = GU_DST_COLOR;
		dest = GU_FIX;
		srcfix = 0xffffffff;
		destfix = 0;
		break;
	case DX_BLENDMODE_DESTCOLOR:
		op = GU_ADD;
		src = GU_FIX;
		dest = GU_FIX;
		srcfix = 0;
		destfix = 0xffffffff;
		break;
	case DX_BLENDMODE_INVDESTCOLOR:
		op = GU_ADD;
		src = GU_ONE_MINUS_DST_COLOR;
		dest = GU_FIX;
		srcfix = 0;
		destfix = 0;
		break;
	case DX_BLENDMODE_INVSRC:
		op = GU_ADD;
		src = GU_SRC_ALPHA;
		dest = GU_ONE_MINUS_SRC_ALPHA;
		srcfix = 0;
		destfix = 0;
		break;
	default:
		return -1;
	}

	if(dxpGraphicsData.blendmode == DX_BLENDMODE_NOBLEND && !alphaEnable)
	{
		GUDISABLE(GU_BLEND);
	}
	else
	{
		GUENABLE(GU_BLEND);
		if(dxpGraphicsData.forceupdate
			|| dxpGraphicsData.op != op
			|| dxpGraphicsData.src != src
			|| dxpGraphicsData.dest != dest
			|| dxpGraphicsData.srcfix != srcfix
			|| dxpGraphicsData.destfix != destfix
		){
			sceGuBlendFunc(op,src,dest,srcfix,destfix);
			dxpGraphicsData.op = op;
			dxpGraphicsData.src = src;
			dxpGraphicsData.dest = dest;
			dxpGraphicsData.srcfix = srcfix;
			dxpGraphicsData.destfix = destfix;
		}
	}

	if(dxpGraphicsData.forceupdate || dxpGraphicsData.color != dxpGraphicsData.gucolor)
	{//色を設定
		sceGuColor(dxpGraphicsData.color);
		dxpGraphicsData.gucolor = dxpGraphicsData.color;
	}

	int tfx,tcc;
	switch(dxpGraphicsData.blendmode)
	{
	case DX_BLENDMODE_NOBLEND:
	case DX_BLENDMODE_MUL:
	case DX_BLENDMODE_DESTCOLOR:
		tcc = GU_TCC_RGB;
		tfx = GU_TFX_MODULATE;
		if(!alphaEnable)
		{
			GUDISABLE(GU_ALPHA_TEST);
			break;
		}
	case DX_BLENDMODE_ALPHA:
	case DX_BLENDMODE_ADD:
	case DX_BLENDMODE_SUB:
	case DX_BLENDMODE_INVDESTCOLOR:
		tcc = GU_TCC_RGBA;
		tfx = GU_TFX_MODULATE;
		GUENABLE(GU_ALPHA_TEST);
		sceGuAlphaFunc(GU_NOTEQUAL,0x00,0xff);
		break;
	case DX_BLENDMODE_INVSRC:
		sceGuTexEnvColor(0x00000000);
		tcc = GU_TCC_RGBA;
		tfx = GU_TFX_BLEND;
		GUDISABLE(GU_ALPHA_TEST);
		break;
	default:
		return -1;
	}
	if(dxpGraphicsData.forceupdate || dxpGraphicsData.tfx != tfx || dxpGraphicsData.tcc != tcc)
	{
		sceGuTexFunc(tfx,tcc);
		dxpGraphicsData.tfx = tfx;
		dxpGraphicsData.tcc = tcc;
	}
	dxpGraphicsData.forceupdate = 0;
	dxpGraphicsData.flag = flag;
	return 0;
}


int dxpGraphicsSetup3D(u32 color)
{
	GUSTART;
	if(dxpGraphicsData.drawstate != DXP_DRAWSTATE_NONTEX3D)
	{
		GUDISABLE(GU_TEXTURE_2D);
		dxpGraphicsData.texture = NULL;
		dxpGraphicsData.drawstate = DXP_DRAWSTATE_NONTEX3D;
		if(dxpGraphicsData.usedepth3d)
		{
			GUENABLE(GU_DEPTH_TEST);
			sceGuDepthFunc(dxpGraphicsData.depthfunc);
			sceGuDepthBuffer(dxpGraphicsData.depthbuffer.texvram,512);	//深度バッファを有効にするときでいい
			if(dxpGraphicsData.writedepth3d)sceGuDepthMask(0);
			else sceGuDepthMask(1);
		}
		else GUDISABLE(GU_DEPTH_TEST);
		dxpGraphicsData.forceupdate = 1;
	}


	register u32 r,g,b,a,t;
	r = color & 0x000000ff;
	t = dxpGraphicsData.color & 0x000000ff;
	r *= t;
	r /= 255;
	g = (color & 0x0000ff00) >> 8;
	t = (dxpGraphicsData.color & 0x0000ff00) >> 8;
	g *= t;
	g /= 255;
	b = (color & 0x00ff0000) >> 16;
	t = (dxpGraphicsData.color & 0x00ff0000) >> 16;
	b *= t;
	b /= 255;
	//a = (color & 0xff000000) >> 24;
	//t = (dxpGraphicsData.color & 0xff000000) >> 24;
	//a *= t;
	//a /= 255;
	a = (dxpGraphicsData.color & 0xff000000) >> 24;
	color = (a << 24) | (b << 16) | (g << 8) | r;

	int op;
	int src,dest;
	unsigned int srcfix;
	unsigned int destfix;	 

	switch(dxpGraphicsData.blendmode)
	{
	case DX_BLENDMODE_NOBLEND:
		GUDISABLE(GU_BLEND);
		sceGuColor(dxpGraphicsData.gucolor = color);
		dxpGraphicsData.forceupdate = 0;
		return 0;
	case DX_BLENDMODE_INVSRC:
		op = GU_ADD;
		src = GU_SRC_ALPHA;
		dest = GU_ONE_MINUS_SRC_ALPHA;
		srcfix = 0;
		destfix = 0;
		color = (color & 0xff000000) | ~(color & 0x00ffffff);
		break;
	case DX_BLENDMODE_ALPHA:
		op = GU_ADD;
		src = GU_SRC_ALPHA;
		dest = GU_ONE_MINUS_SRC_ALPHA;
		srcfix = 0;
		destfix = 0;
		break;
	case DX_BLENDMODE_ADD:
		op = GU_ADD;
		src = GU_SRC_ALPHA;
		dest = GU_FIX;
		srcfix = 0xffffffff;
		destfix = 0xffffffff;
		break;
	case DX_BLENDMODE_SUB:
		op = GU_REVERSE_SUBTRACT;
		src = GU_SRC_ALPHA;
		dest = GU_FIX;
		srcfix = 0xffffffff;
		destfix = 0xffffffff;
		break;
	case DX_BLENDMODE_MUL:
		op = GU_ADD;
		src = GU_DST_COLOR;
		dest = GU_FIX;
		srcfix = 0;
		destfix = 0;
		break;
	case DX_BLENDMODE_DESTCOLOR:
		op = GU_ADD;
		src = GU_FIX;
		dest = GU_FIX;
		srcfix = 0;
		destfix = 0xffffffff;
		break;
	case DX_BLENDMODE_INVDESTCOLOR:
		op = GU_ADD;
		src = GU_ONE_MINUS_DST_COLOR;
		dest = GU_FIX;
		srcfix = 0;
		destfix = 0;
		break;
	default:
		return -1;
	}
	sceGuColor(dxpGraphicsData.gucolor = color);

	GUENABLE(GU_BLEND);
	if(dxpGraphicsData.forceupdate
		|| dxpGraphicsData.op != op
		|| dxpGraphicsData.src != src
		|| dxpGraphicsData.dest != dest
		|| dxpGraphicsData.srcfix != srcfix
		|| dxpGraphicsData.destfix != destfix
	){
		sceGuBlendFunc(op,src,dest,srcfix,destfix);
		dxpGraphicsData.op = op;
		dxpGraphicsData.src = src;
		dxpGraphicsData.dest = dest;
		dxpGraphicsData.srcfix = srcfix;
		dxpGraphicsData.destfix = destfix;
	}

	dxpGraphicsData.forceupdate = 0;
	return 0;
}

int SetTransformToWorld( MATRIX *Matrix )
{
	sceGumMatrixMode(GU_MODEL);
	sceGumLoadMatrix(Matrix);
	return 0;
}

#endif