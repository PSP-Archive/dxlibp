#include "../graphics.h"
#include <pspvfpu.h>

int DrawRotaGraph(int x,int y,float scale,float angle,int gh,int trans,int turn)
{
	DXPGRAPHICSHANDLE *gptr;
	ScePspFMatrix4 pos;
	GHANDLE2GPTR(gptr,gh);
	register int w = gptr->u1 - gptr->u0;
	register int h = gptr->v1 - gptr->v0;

//	pspvfpu_use_matrices(NULL,0,VMAT0 | VMAT1 | VMAT2);//なんか用途がよくわからない

	__asm__ volatile(
		"mtv		%1,		S000\n"			//scale
		"mtv		%2,		S001\n"			//angle

		"vcst.s		S002,	VFPU_2_PI\n"	//angleをラジアン単位からVFPU内部の角度単位に直す
		"vmul.s		S001,	S001[-x],	S002\n"

		"vmidt.t	M100\n"					//回転行列を作成
		"vrot.t		C100,	S001,	[c,s,0]\n"
		"vrot.t		C110,	S001,	[-s,c,0]\n"

		"vmscl.t	M100,	M100,	S000\n"	//回転行列をscale倍する

		"mtv		%3,		S102\n"			//x,y座標を回転行列に入れる。これで変換用の行列が完成する
		"mtv		%4,		S112\n"
		"vi2f.p		R102,	R102,	1/2\n"	//int→float変換

		"mtv		%5,		S220\n"			//画像の縦、横の大きさ
		"mtv		%6,		S221\n"
		"vi2f.p		C220,	C220,	1\n"	//int→float変換と同時に1/2倍する
		"vone.s		S222\n"					//1をセット。[w,h,1]というベクトルができるので、これらを変換する

		"vmov.t		C200,	C220[-x,-y,1]\n"//[-w,-h-1]
		"vmov.t		C210,	C220[x,-y,1]\n"//[w,-h-1]
		"vmov.t		C230,	C220[-x,y,1]\n"//[-w,h-1]

		"vtfm3.t	C020,	M100,	C220\n"//行列変換
		"vtfm3.t	C000,	M100,	C200\n"
		"vtfm3.t	C010,	M100,	C210\n"
		"vtfm3.t	C030,	M100,	C230\n"

		"usv.q		C020,	32 + %0\n"//変換したデータの書き出し
		"usv.q		C000,	0  + %0\n"
		"usv.q		C010,	16 + %0\n"
		"usv.q		C030,	48 + %0\n"
		:"=m"(pos): "r"(scale), "r"(angle), "r"(x), "r"(y),"r"(w),"r"(h)
	);

	if(turn)return DrawModiGraphF(pos.y.x,pos.y.y,pos.x.x,pos.x.y,pos.w.x,pos.w.y,pos.z.x,pos.z.y,gh,trans);
	return DrawModiGraphF(pos.x.x,pos.x.y,pos.y.x,pos.y.y,pos.z.x,pos.z.y,pos.w.x,pos.w.y,gh,trans);
}



