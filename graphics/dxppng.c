#include "dxppng.h"

#include <string.h>
#include <malloc.h>
#include <zlib.h>
#ifdef WIN32
typedef unsigned __int8		u8;
typedef unsigned __int16	u16;
typedef unsigned __int32	u32;
typedef unsigned __int64	u64;

static void* memalign(unsigned int align,unsigned int size)
{
	return malloc(size);
}

#else
#include <psptypes.h>
#endif

typedef struct CHUNK_
{
	u8 *pos;
	u32	length;
	u8	name[4];
	u8 *data;
	u32 crc;
	u8	valid;
}CHUNK;

static u32 crctable[256];
static int crcinit = 0;
static u32 bytearray2dword(const u8 dat[])
{
	if(!dat)return 0;
	return (((u32)dat[0]) << 24) | (((u32)dat[1]) << 16) | (((u32)dat[2]) << 8) | (((u32)dat[3]) & 0xff);
}

static u16 bytearray2word(const u8 dat[])
{
	if(!dat)return 0;
	return (((u16)dat[0]) << 8) | (((u16)dat[1]) & 0xff);
}

static int signaturecheck(const u8 dat[])
{
	if(!dat)return -1;
	if(dat[0] == 137 && dat[1] == 80 && dat[2] == 78 && dat[3] == 71
		&& dat[4] == 13 && dat[5] == 10 && dat[6] == 26 && dat[7] == 10)return 0;
	return -1;
}

static int	getchunk(u8 dat[],CHUNK *cnk)
{
	if(!dat || !cnk)return -1;
	cnk->pos = dat;
	cnk->length = bytearray2dword(dat);
	cnk->name[0] = dat[4];
	cnk->name[1] = dat[5];
	cnk->name[2] = dat[6];
	cnk->name[3] = dat[7];
	cnk->data = dat + 8;
	cnk->crc = bytearray2dword(dat + 8 + cnk->length);
	cnk->valid = 1;
	return 0;
}

static u32	getcrc(const u8 dat[],u32 length)
{
	u32 result;
	int i;
	if(!dat)return 0;
	if(!crcinit)
	{
		u32 c;
		int n, k;
		for (n = 0;n < 256;++n)
		{
			c = (u32)n;
			for (k = 0;k < 8;++k)
			{
				if (c & 1)c = 0xEDB88320 ^ (c >> 1);
				else c >>= 1;
			}
			crctable[n] = c;
		}
		crcinit = 1;
	}
	result = 0xFFFFFFFF;
	for (i = 0;i < length;++i)result = crctable[(result ^ dat[i]) & 0xFF] ^ (result >> 8);
	result ^= 0xFFFFFFFF;
	return result;
}

static int	comparechunkname(const u8 dat1[],const u8 dat2[])
{
	if(!dat1 || !dat2)return -1;
	if(dat1[0] == dat2[0] && dat1[1] == dat2[1] && dat1[2] == dat2[2] && dat1[3] == dat2[3])return 0;
	return 1;
}
int dxppng_decode(DXPPNG_PARAMS *params,DXPPNG *png)
{
	DXPPNG_FUNCS *funcs;
	CHUNK IHDR,PLTE,tRNS,IDAT;
	u32 idatnum = 0;
	u32	width,height;
	u8	bitDepth,colorType,interlace;
	u8 epp;//elements per pixel
	u16 trnscolor[3];//rgb:rgb gray:g--

	memset(&IHDR,0,sizeof(CHUNK));
	memset(&PLTE,0,sizeof(CHUNK));
	memset(&tRNS,0,sizeof(CHUNK));
	memset(&IDAT,0,sizeof(CHUNK));
	trnscolor[0] = trnscolor[1] = trnscolor[2] = 0;

	//arguments check and setup
	if(!params || !png)return -1;
	funcs = &params->funcs;
	if(!funcs->pmalloc || !funcs->pfree || !funcs->pmemalign)
	{
		funcs->pmalloc = malloc;
		funcs->pfree = free;
		funcs->pmemalign = memalign;
	}

	png->alpha = 0;

	//signature check
	if(signaturecheck((u8*)params->src) < 0)return -1;

	//get chunk
	{
		u8 *ptr = (u8*)params->src + 8,endexist = 0;
		CHUNK tmpcnk;
		tmpcnk.valid = 1;
		while(1)
		{
			if(getchunk(ptr,&tmpcnk) < 0)return -1;
			if(getcrc(tmpcnk.data - 4,tmpcnk.length + 4) != tmpcnk.crc)return -1;
			if(tmpcnk.pos + 12 + tmpcnk.length > (u8*)params->src + params->srcLength)return -1;
			if(!comparechunkname(tmpcnk.name,(u8*)"IHDR"))
			{
				if(IHDR.valid)return -1;
				IHDR = tmpcnk;
				ptr += 12 + tmpcnk.length;
				continue;
			}
			if(!comparechunkname(tmpcnk.name,(u8*)"PLTE"))
			{
				if(PLTE.valid)return -1;
				PLTE = tmpcnk;
				ptr += 12 + tmpcnk.length;
				continue;
			}
			if(!comparechunkname(tmpcnk.name,(u8*)"tRNS"))
			{
				if(tRNS.valid)return -1;
				tRNS = tmpcnk;
				ptr += 12 + tmpcnk.length;
				continue;
			}
			if(!comparechunkname(tmpcnk.name,(u8*)"IDAT"))
			{
				if(!IDAT.valid)IDAT = tmpcnk;
				ptr += 12 + tmpcnk.length;
				idatnum += 1;
				continue;
			}
			if(!comparechunkname(tmpcnk.name,(u8*)"IEND"))
			{
				endexist = 1;
				break;
			}
			ptr += 12 + tmpcnk.length;
		}
		if(!endexist)return -1;
	}

	//IHDR
	{
		u64 cs;
		if(!IHDR.valid)return -1;
		width = bytearray2dword(IHDR.data);
		height = bytearray2dword(IHDR.data + 4);
		cs = width;
		cs *= height;
		if(cs >> 30)return -1;
		bitDepth = IHDR.data[8];
		colorType = IHDR.data[9];
		if(IHDR.data[10])return -1;
		if(IHDR.data[11])return -1;
		interlace = IHDR.data[12];
		if(bitDepth != 1 && bitDepth != 2 && bitDepth != 4 && bitDepth != 8 && bitDepth != 16)return -1; 
		if(colorType & 1 && bitDepth > 8)return -1;
		if(colorType == 2 && bitDepth < 8)return -1;
		if(colorType & 4 && bitDepth < 8)return -1;
		switch(colorType)
		{
			case 0:
			case 3:epp = 1;break;
			case 2:epp = 3;break;
			case 4:epp = 2;png->alpha = 1;break;
			case 6:epp = 4;png->alpha = 1;break;
			default:return -1;
		}
		png->width = width;
		png->height = height;
		if(params->mode)
		{
			switch(colorType)
			{
				case 0://g
				case 3://p
					if(bitDepth <= 4)png->psm = PSM_T4;else png->psm = PSM_T8;
					break;
				case 2://rgb
				case 4://g+a
				case 6://rgb+a
					png->psm = PSM_8888;
					break;
			}
			if(png->width > 512 || png->height > 512)return -1;
			for(png->widthN2 = 1;png->widthN2 < png->width;png->widthN2 <<= 1);
			for(png->heightN2 = 1;png->heightN2 < png->height;png->heightN2 <<= 1);
			if(png->heightN2 < 8)png->heightN2 = 8;
			if(png->psm == PSM_8888)
			{
				png->pitch = ((png->width + 3) >> 2) << 2;
			}else if(png->psm == PSM_T8)
			{
				png->pitch = ((png->width + 15) >> 4) << 4;
			}else if(png->psm == PSM_T4)
			{
				png->pitch = ((png->width + 31) >> 5) << 5;
			}else return -1;
		}
		else
		{
			png->psm = PSM_8888;
			png->heightN2 = png->height;
			png->widthN2 = png->width;
			png->pitch = png->width;
		}
	}

	//PLTE
	if(colorType == 3)
	{
		int i;
		if(!PLTE.valid)return -1;
		if(PLTE.length % 3 || (png->clutnum = PLTE.length / 3) > 256)return -1;
		if(!(png->clut = funcs->pmemalign(16,png->clutnum * 4)))return -1;
		for(i = 0;i < png->clutnum;++i)png->clut[i] = 0xff000000 | (((u32)PLTE.data[i * 3 + 2]) << 16) | (((u32)PLTE.data[i * 3 + 1]) << 8) | ((u32)PLTE.data[i * 3]);
	}else if(colorType == 0)
	{
		int i;
		u32 c;
		png->clutnum = 1 << (bitDepth > 8 ? 8 : bitDepth);
		if(!(png->clut = funcs->pmemalign(16,png->clutnum * 4)))return -1;
		for(i = 0;i < png->clutnum;++i)
		{
			c = (255 / ((1 << (bitDepth > 8 ? 8 : bitDepth)) - 1)) * i;
			png->clut[i] = 0xff000000 | (c << 16) | (c << 8) | (c);
		}
	}else
	{
		png->clut = 0;
		png->clutnum = 0;
	}

	//tRNS
	if(tRNS.valid)
	{
		int i;
		png->alpha = 1;
		if(colorType == 3)//clut
		{
			if(png->clutnum < tRNS.length){params->funcs.pfree(png->clut);return -1;}
			for(i = 0;i < tRNS.length;++i)png->clut[i] = (((u32)tRNS.data[i]) << 24) | (png->clut[i] & 0x00ffffff);
		}else if(colorType == 2)//rgb
		{
			trnscolor[0] = bytearray2word(tRNS.data + 0);
			trnscolor[1] = bytearray2word(tRNS.data + 2);
			trnscolor[2] = bytearray2word(tRNS.data + 4);
		}else if(colorType == 0)//gray
		{
			trnscolor[0] = bytearray2word(tRNS.data);
			for(i = 0;i < png->clutnum;++i)
				if((png->clut[i] & 0xff) == (trnscolor[0] >> 8))png->clut[i] &= 0x00ffffff;
		}else {params->funcs.pfree(png->clut);return -1;}
	}

	{//IDAT
		u32 rawsize,linesize;//byte
		u8 *rawbuf,*linebuf[2],cl = 0;//cl::current linebuf
		u32 interlace_size[7][2];//pixel
		linesize = (width * bitDepth * epp + 7) >> 3;
		rawsize = png->heightN2 * png->pitch * (((png->psm == PSM_8888 ? 8 : (png->psm == PSM_T8 ? 2 : 1)) + 1) / 2);
		rawbuf = funcs->pmemalign(16,rawsize);
		linebuf[0] = funcs->pmalloc((linesize << 1) + 2);
		linebuf[1] = linebuf[0] + linesize + 1;
		if(!rawbuf || !linebuf[0])
		{
			funcs->pfree(png->clut);
			funcs->pfree(rawbuf);
			funcs->pfree(linebuf[0]);
			return -1;
		}
		if(interlace)
		{
			interlace_size[0][0] = (width + 7) >> 3;	interlace_size[0][1] = (height + 7) >> 3;
			interlace_size[1][0] = (width + 3) >> 3;	interlace_size[1][1] = (height + 7) >> 3;
			interlace_size[2][0] = (width + 3) >> 2;	interlace_size[2][1] = (height + 3) >> 3;
			interlace_size[3][0] = (width + 1) >> 2;	interlace_size[3][1] = (height + 3) >> 2;
			interlace_size[4][0] = (width + 1) >> 1;	interlace_size[4][1] = (height + 1) >> 2;
			interlace_size[5][0] = width >> 1;			interlace_size[5][1] = (height + 1) >> 1;
			interlace_size[6][0] = width;				interlace_size[6][1] = height >> 1;
		}else
		{
			interlace_size[0][0] = width;
			interlace_size[0][1] = height;
		}

		{//uncompress
			int retinf,errflag = 0;
			u32 i,j,k,pathnum,bpp;
			z_stream z;
			CHUNK tmpcnk;
			bpp= (epp * bitDepth + 7) >> 3;
			z.zalloc = 0;
			z.zfree = 0;
			z.opaque = 0;
			inflateInit(&z);
			tmpcnk = IDAT;
			z.next_in = tmpcnk.data;
			z.avail_in = tmpcnk.length;
			pathnum = interlace ? 7 : 1;
			for(i = 0;i < pathnum;++i)//path loop
			{
				linesize = ((interlace_size[i][0] * epp * bitDepth + 7) >> 3);
				for(j = 0;j < linesize + 1;++j)linebuf[0][j] = linebuf[1][j] = 0;//zero clear line buf
				for(j = 0;j < interlace_size[i][1];++j)//hight loop
				{
					
					u8 fltr;
					z.next_out = linebuf[cl];
					z.avail_out = linesize + 1;
					while(1)//read 1 line
					{
						if(!z.avail_in)
						{
							if(getchunk(tmpcnk.pos + 12 + tmpcnk.length,&tmpcnk) < 0){errflag = 1;break;}
							if(comparechunkname(tmpcnk.name,(u8*)"IDAT")){errflag = 1;break;}
							z.next_in = tmpcnk.data;
							z.avail_in = tmpcnk.length;
						}
						retinf = inflate(&z,Z_NO_FLUSH);
						if(retinf < 0){errflag = 1;break;}
						if(retinf == Z_OK)
						{
							if(!z.avail_out)break;
							continue;
						}else if(retinf == Z_STREAM_END)break;
					}
					if(errflag)
						break;
					//filter release
					fltr = linebuf[cl][0];
					for(k = 0;k < linesize;++k)//release filter
					{
						int a,b,c,p,pa,pb,pc;
						u8 left,up,upperleft;
						left = (k >= bpp ? linebuf[cl][k - bpp] : 0);
						up = linebuf[cl ^ 1][k];
						upperleft =  k >= bpp ? linebuf[cl ^ 1][k - bpp] : 0;
						switch(fltr)
						{
						case 0:
							linebuf[cl][k] = linebuf[cl][k + 1];
							break;
						case 1:
							linebuf[cl][k] = (u8)(((int)linebuf[cl][k + 1] + (int)left) & 0xff);
							break;
						case 2:
							linebuf[cl][k] = (u8)(((int)linebuf[cl][k + 1] + (int)up) & 0xff);
							break;
						case 3:
							linebuf[cl][k] = (u8)(((int)linebuf[cl][k + 1] + (((int)left + (int)up) >> 1)) & 0xff);
							break;
						case 4:
							a = left;
							b = up;
							c = upperleft;
							p = a + b - c;
							pa = p - a;
							pb = p - b;
							pc = p - c;
							pa = pa < 0 ? -pa : pa;
							pb = pb < 0 ? -pb : pb;
							pc = pc < 0 ? -pc : pc;
							linebuf[cl][k] = (u8)(((int)linebuf[cl][k + 1] + (int)(pa <= pb && pa <= pc ? a : (pb <= pc ? b : c))) & 0xff);
							break;
						default:
							errflag = 1;
						}
					}
					if(errflag)
						break;
					//copy from linebuf to rawbuf
					for(k = 0;k < interlace_size[i][0];++k)
					{
						u32 x,y,color,bit,a;
						u16 r,g,b;
						u8 *out;
						r = g = b = 0;
						x = k;
						y = j;
						if(interlace)
						{
							x = (x << (3 - (i >> 1))) + (i & 1) * (1 << (2 - (i >> 1)));
							if(!i)y <<= 3;
							else y = (y << (3 - ((i - 1) >> 1))) + ((i & 1) ^ 1) * (1 << (2 - ((i - 1) >> 1)));
						}
						out = rawbuf + (y * png->pitch + x) * (png->psm == PSM_8888 ? 8 : (png->psm == PSM_T8 ? 2 : 1)) / 2;
						switch(bitDepth)
						{
						case 1://g or p 1
							bit = (~k) & 7;
							color = (linebuf[cl][k >> 3] & (1 << bit)) >> bit;
							if(!params->mode)//raw
							{
								*(u32*)out = png->clut[color];
								break;
							}
							if(k & 1)
							{
								*out |= (color & 0xf) << 4;
							}else
							{
								*out = color & 0xf;
							}
							break;
						case 2://g or p 1
							bit = (~k & 3) << 1;
							color = (linebuf[cl][k >> 2] & (3 << bit)) >> bit;
							if(!params->mode)//raw
							{
								*(u32*)out = png->clut[color];
								break;
							}
							if(k & 1)
							{
								*out |= (color & 0xf) << 4;
							}else
							{
								*out = color & 0xf;
							}
							break;
						case 4://g or p 1
							bit = (~k & 1) << 2;
							color = (linebuf[cl][k >> 1] & (15 << bit)) >> bit;
							if(!params->mode)//raw
							{
								*(u32*)out = png->clut[color];
								break;
							}
							if(k & 1)
							{
								*out |= (color & 0xf) << 4;
							}else
							{
								*out = color & 0xf;
							}
							break;
						case 8://all 1 2 3 4
							switch(epp)
							{
							case 1://g or p
								color = linebuf[cl][k];
								if(!params->mode)//raw
								{
									*(u32*)out = png->clut[color];
									break;
								}
								*out = color;
								break;
							case 2://g + a
								color = linebuf[cl][k << 1];
								*(u32*)out = ((u32)linebuf[cl][(k << 1) + 1] << 24) | (color << 16) | (color << 8) | color;
								break;
							case 3://rgb
								a = 0xff000000;
								if(tRNS.valid)
								{
									r = trnscolor[0] >> 8;
									g = trnscolor[1] >> 8;
									b = trnscolor[2] >> 8;
									if(linebuf[cl][k * 3 + 2] == r && linebuf[cl][k * 3 + 1] == g && linebuf[cl][k * 3] == b)a = 0; 
								}
								*(u32*)out = a | ((u32)linebuf[cl][k * 3 + 2] << 16) | ((u32)linebuf[cl][k * 3 + 1] << 8) | ((u32)linebuf[cl][k * 3]);
								break;
							case 4://rgba
								*(u32*)out = ((u32)linebuf[cl][(k << 2) + 3] << 24) | ((u32)linebuf[cl][(k << 2) + 2] << 16) | ((u32)linebuf[cl][(k << 2) + 1] << 8) | ((u32)linebuf[cl][(k << 2) + 0]);
								break;
							}
							break;
						case 16://except for p 1 2 3 4
							switch(epp)
							{
							case 1://g or p
								color = bytearray2word(linebuf[cl] + (k << 1));
								if(!params->mode)//raw
								{
									*(u32*)out = png->clut[color >> 8];
									break;
								}
								*out = color >> 8;
								break;
							case 2://g + a
								color = linebuf[cl][k << 2];
								*(u32*)out = ((u32)linebuf[cl][(k << 2) + 2] << 24) | (color << 16) | (color << 8) | color;
								break;
							case 3://rgb
								a = 0xff000000;
								if(tRNS.valid)
								{
									r = bytearray2word(linebuf[cl] + k * 6);
									g = bytearray2word(linebuf[cl] + k * 6 + 2);
									b = bytearray2word(linebuf[cl] + k * 6 + 4);
									if(trnscolor[0] == r && trnscolor[1] == g && trnscolor[2] == b)a = 0; 
								}
								*(u32*)out = a | ((u32)b << 16) | ((u32)g << 8) | ((u32)r);
								break;
							case 4://rgba
								*(u32*)out = ((u32)linebuf[cl][(k << 3) + 6] << 24) | ((u32)linebuf[cl][(k << 3) + 4] << 16) | ((u32)linebuf[cl][(k << 3) + 2] << 8) | ((u32)linebuf[cl][k << 3]);
								break;
							}
							break;
						}
					}
					cl ^= 1;
				}
				if(errflag)break;
			}
			if(errflag)
			{
				funcs->pfree(png->clut);
				funcs->pfree(rawbuf);
				funcs->pfree(linebuf[0]);
				return -1;
			}
			inflateEnd(&z);
			funcs->pfree(linebuf[0]);
			png->raw = rawbuf;
		}//uncompress end
		if(png->psm == PSM_8888)
		{
			params->funcs.pfree(png->clut);
			png->clutnum = 0;
		}
	}
	return 0;
}
