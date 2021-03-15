#include "../dxlibp.h"

MATRIX MGetIdent()
{
	MATRIX m;
	__asm__ volatile (
		"vmidt.q	M000\n"
		"usv.q		R000,	0  + %0\n"
		"usv.q		R001,	16 + %0\n"
		"usv.q		R002,	32 + %0\n"
		"usv.q		R003,	48 + %0\n"
		:"=m"(m)
		);
	return m;
}

MATRIX MGetScale(VECTOR v)
{
	MATRIX m;
	__asm__ volatile (
		"ulv.q		R100,	%1\n"
		"vmov.q		R000,	R100[x,0,0,0]\n"
		"vmov.q		R001,	R100[0,y,0,0]\n"
		"vmov.q		R002,	R100[0,0,z,0]\n"
		"vmov.q		R003,	R100[0,0,0,1]\n"
		"usv.q		R000,	0  + %0\n"
		"usv.q		R001,	16 + %0\n"
		"usv.q		R002,	32 + %0\n"
		"usv.q		R003,	48 + %0\n"
		:"=m"(m)
		:"m"(v)
		);
	return m;	
}

MATRIX MGetTranslate(VECTOR v)
{
	MATRIX m;
	__asm__ volatile(
		"ulv.q		R100,	%1\n"
		"vmidt.q	M000\n"
		"vmov.q		R003,	R100[x,y,z,1]\n"
		"usv.q		R000,	0  + %0\n"
		"usv.q		R001,	16 + %0\n"
		"usv.q		R002,	32 + %0\n"
		"usv.q		R003,	48 + %0\n"
		:"=m"(m)
		:"m"(v)
	);
	return m;
}

MATRIX MGetRotX(float angle)
{
	MATRIX m;
	__asm__ volatile(
		"mtv		%1,	S100\n"
		"vcst.s		S110,	VFPU_2_PI\n"
		"vmul.s		S100,	S100,	S110\n"
		"vmidt.q	M000\n"
		"vrot.q		R001,	S100,	[0,c,s,0]\n"
		"vrot.q		R002,	S100,	[0,-s,c,0]\n"
		"usv.q		R000,	0  + %0\n"
		"usv.q		R001,	16 + %0\n"
		"usv.q		R002,	32 + %0\n"
		"usv.q		R003,	48 + %0\n"
		:"=m"(m)
		:"r"(angle)
	);
	return m;
}

MATRIX MGetRotY(float angle)
{
	MATRIX m;
	__asm__ volatile(
		"mtv		%1,	S100\n"
		"vcst.s		S110,	VFPU_2_PI\n"
		"vmul.s		S100,	S100,	S110\n"
		"vmidt.q	M000\n"
		"vrot.q		R000,	S100,	[c,0,-s,0]\n"
		"vrot.q		R002,	S100,	[s,0,c,0]\n"
		"usv.q		R000,	0  + %0\n"
		"usv.q		R001,	16 + %0\n"
		"usv.q		R002,	32 + %0\n"
		"usv.q		R003,	48 + %0\n"
		:"=m"(m)
		:"r"(angle)
	);
	return m;
}

MATRIX MGetRotZ(float angle)
{
	MATRIX m;
	__asm__ volatile(
		"mtv		%1,	S100\n"
		"vcst.s		S110,	VFPU_2_PI\n"
		"vmul.s		S100,	S100,	S110\n"
		"vmidt.q	M000\n"
		"vrot.q		R000,	S100,	[c,s,0,0]\n"
		"vrot.q		R001,	S100,	[-s,c,0,0]\n"
		"usv.q		R000,	0  + %0\n"
		"usv.q		R001,	16 + %0\n"
		"usv.q		R002,	32 + %0\n"
		"usv.q		R003,	48 + %0\n"
		:"=m"(m)
		:"r"(angle)
	);
	return m;
}


MATRIX MGetRotAxis(VECTOR v,float angle)
{
	MATRIX m;
	__asm__ volatile(
		"ulv.q		R100,	%2\n"//[x,y,z,a]
		"vdot.t		S103,	R100,	R100\n"//[x,y,z]を正規化
		"vrsq.s		S103,	S103\n"
		"vscl.t		R100,	R100,	S103\n"

		"mtv		%1,		S130\n"//[a]
		"vcst.s		S103,	VFPU_2_PI\n"//VFPU用の角度に変換
		"vmul.s		S130,	S130,	S103\n"

		"vrot.p		R101,	S130,	[s,c]\n"//s,c,1-c
		"vocp.s		S121,	S111\n"//ocp(t):1 - t

		"vscl.t		R102,	R100,	S121\n"//[_cx,_cy,_cz]

		"vmov.q		R000,	R100[x,y,z,0]\n"//[_cxx,_cxy,_cxz,0]
		"vmov.q		R001,	R100[x,y,z,0]\n"//[_cxy,_cyy,_cyz,0]
		"vmov.q		R002,	R100[x,y,z,0]\n"//[_cxz,_cyz,_czz,0]
		"vmov.q		R003,	R100[0,0,0,1]\n"//[0,0,0,1]
		"vscl.t		R000,	R000,	S102\n"
		"vscl.t		R001,	R001,	S112\n"
		"vscl.t		R002,	R002,	S122\n"

		"vmul.t		R102,	R101[y,x,-x],	R100[1,z,y]\n"
		"vadd.t		R000,	R000,	R102\n"
		"vmul.t		R102,	R101[-x,y,x],	R100[z,1,x]\n"
		"vadd.t		R001,	R001,	R102\n"
		"vmul.t		R102,	R101[x,-x,y],	R100[y,x,1]\n"
		"vadd.t		R002,	R002,	R102\n"

		"usv.q		R000,	0  + %0\n"
		"usv.q		R001,	16 + %0\n"
		"usv.q		R002,	32 + %0\n"
		"usv.q		R003,	48 + %0\n"
		:"=m"(m)
		:"r"(angle),"m"(v)
	);
	return m;
}

MATRIX MGetRotVec2(VECTOR v1,VECTOR v2)
{
	MATRIX m;
	__asm__ volatile (
		"ulv.q		R000,	%1\n"//行列の読み込みと正規化
		"ulv.q		R001,	%2\n"
		"vdot.t		S002,	R000,	R000\n"
		"vdot.t		S012,	R001,	R001\n"
		"vrsq.p		R002,	R002\n"
		"vscl.t		R000,	R000,	S002\n"
		"vscl.t		R001,	R001,	S012\n"

		"vdot.t		S002,	R000,	R001\n"//cosθの値

		"vmul.s		S012,	S002,	S002\n"
		"vocp.p		R022,	R002\n"//[c,cc,1-c,1-cc]
		"vsqrt.s	S012,	S032\n"//[c,s,1-c]

		"vcrsp.t	R003,	R000,	R001\n"//外積は両ベクトルに直交するから、これに対して回転行列を作ればいい
		"vdot.t		S032,	R003,	R003\n"//正規化してやらにゃ・・・
		"vrsq.s		S032,	S032\n"
		"vscl.t		R003,	R003,	S032\n"

		"vscl.t		R000,	R003,	S022\n"//[_cx,_cy,_cz]
		"vmov.q		R100,	R003[x,y,z,0]\n"
		"vmov.q		R101,	R003[x,y,z,0]\n"
		"vmov.q		R102,	R003[x,y,z,0]\n"
		"vmov.q		R103,	R003[0,0,0,1]\n"

		"vscl.t		R100,	R100,	S000\n"
		"vscl.t		R101,	R101,	S010\n"
		"vscl.t		R102,	R102,	S020\n"

		"vmul.t		R000,	R002[x,y,-y],	R003[1,z,y]\n"
		"vadd.t		R100,	R100,	R000\n"
		"vmul.t		R000,	R002[-y,x,y],	R003[z,1,x]\n"
		"vadd.t		R101,	R101,	R000\n"
		"vmul.t		R000,	R002[y,-y,x],	R003[y,x,1]\n"
		"vadd.t		R102,	R102,	R000\n"

		"usv.q		R100,	0  + %0\n"
		"usv.q		R101,	16 + %0\n"
		"usv.q		R102,	32 + %0\n"
		"usv.q		R103,	48 + %0\n"
		
		:"=m"(m)
		:"m"(v1),"m"(v2)
	);
	return m;
}

MATRIX MGetAxis1(VECTOR xAxis,VECTOR yAxis,VECTOR zAxis,VECTOR pos)
{
	MATRIX m;
	m.m[0][0] = xAxis.x;m.m[0][1] = xAxis.y;m.m[0][2] = xAxis.z;m.m[0][3] = 0.0f;
	m.m[1][0] = yAxis.x;m.m[1][1] = yAxis.y;m.m[1][2] = yAxis.z;m.m[1][3] = 0.0f;
	m.m[2][0] = zAxis.x;m.m[2][1] = zAxis.y;m.m[2][2] = zAxis.z;m.m[2][3] = 0.0f;
	m.m[3][0] = pos.x;m.m[3][1] = pos.y;m.m[3][2] = pos.z;m.m[3][3] = 1.0f;
	return m;
}


MATRIX MGetAxis2(VECTOR xAxis,VECTOR yAxis,VECTOR zAxis,VECTOR pos)
{
	MATRIX m;
	__asm__ volatile (
		"ulv.q		C010,	%1\n"
		"ulv.q		C020,	%2\n"
		"ulv.q		C030,	%3\n"
		"ulv.q		C040,	%4\n"
		"vdot.t		S003,	C010,	C040[-x,-y,-z]\n"
		"vdot.t		S013,	C020,	C040[-x,-y,-z]\n"
		"vdot.t		S023,	C030,	C040[-x,-y,-z]\n"
		"vmov.q		C040,	C040[0,0,0,1]\n"
		"usv.q		R000,	0  + %0\n"
		"usv.q		R001,	16 + %0\n"
		"usv.q		R002,	32 + %0\n"
		"usv.q		R003,	48 + %0\n"
		:"=m"(m)
		:"m"(xAxis),"m"(yAxis),"m"(zAxis),"m"(pos)
		);
	return m;
}


MATRIX MAdd(MATRIX m1,MATRIX m2)
{
	MATRIX m;
	__asm__ volatile (
		"ulv.q		R000,	0  + %1\n"
		"ulv.q		R001,	16 + %1\n"
		"ulv.q		R002,	32 + %1\n"
		"ulv.q		R003,	48 + %1\n"
		"ulv.q		R100,	0  + %2\n"
		"ulv.q		R101,	16 + %2\n"
		"ulv.q		R102,	32 + %2\n"
		"ulv.q		R103,	48 + %2\n"
		"vadd.q		R200,	R000,	R100\n"
		"vadd.q		R201,	R001,	R101\n"
		"vadd.q		R202,	R002,	R102\n"
		"vadd.q		R203,	R003,	R103\n"
		"usv.q		R200,	0  + %0\n"
		"usv.q		R201,	16 + %0\n"
		"usv.q		R202,	32 + %0\n"
		"usv.q		R203,	48 + %0\n"
		:"=m"(m)
		:"m"(m1),"m"(m2)
		);
	return m;
}

MATRIX MMult(MATRIX m1,MATRIX m2)
{
	MATRIX m;
	__asm__ volatile (
		"ulv.q		R000,	0  + %1\n"
		"ulv.q		R001,	16 + %1\n"
		"ulv.q		R002,	32 + %1\n"
		"ulv.q		R003,	48 + %1\n"
		"ulv.q		R100,	0  + %2\n"
		"ulv.q		R101,	16 + %2\n"
		"ulv.q		R102,	32 + %2\n"
		"ulv.q		R103,	48 + %2\n"
		"vmmul.q	M200,	M000,	M100\n"
		"usv.q		R200,	0  + %0\n"
		"usv.q		R201,	16 + %0\n"
		"usv.q		R202,	32 + %0\n"
		"usv.q		R203,	48 + %0\n"
		:"=m"(m)
		:"m"(m1),"m"(m2)
		);
	return m;
}

MATRIX MScale(MATRIX m,float scale)
{
	__asm__ volatile (
		"ulv.q		R000,	0  + %0\n"
		"ulv.q		R001,	16 + %0\n"
		"ulv.q		R002,	32 + %0\n"
		"ulv.q		R003,	48 + %0\n"
		"mtv		%1,		S100\n"
		"vmscl.q	M000,	M000,	S100\n"
		"usv.q		R000,	0  + %0\n"
		"usv.q		R001,	16 + %0\n"
		"usv.q		R002,	32 + %0\n"
		"usv.q		R003,	48 + %0\n"
		:"=m"(m)
		:"r"(scale)
		);
	return m;
}

MATRIX MTranspose(MATRIX m)
{
	__asm__ volatile (
		"ulv.q		R000,	0  + %0\n"
		"ulv.q		R001,	16 + %0\n"
		"ulv.q		R002,	32 + %0\n"
		"ulv.q		R003,	48 + %0\n"
		"usv.q		C000,	0  + %0\n"
		"usv.q		C010,	16 + %0\n"
		"usv.q		C020,	32 + %0\n"
		"usv.q		C030,	48 + %0\n"
		:"=m"(m)
		);
	return m;
}

MATRIX MInverse(MATRIX m)//組むのが一番大変で一番楽しかったアセンブラコードで賞
{
	register float d;
	__asm__ volatile (//d算出
		"ulv.q		R200,	0  + %1\n"
		"ulv.q		R201,	16 + %1\n"
		"ulv.q		R202,	32 + %1\n"
		"ulv.q		R203,	48 + %1\n"

		"vmul.q		R100,	R201[y,x,x,x],	R202[z,z,y,y]\n"
		"vmul.q		R100,	R203[w,w,w,z],	R100\n"

		"vmul.q		R101,	R201[z,z,y,y],	R202[w,w,w,z]\n"
		"vmul.q		R101,	R203[y,x,x,x],	R101\n"
		"vadd.q		R100,	R100,	R101\n"

		"vmul.q		R101,	R201[w,w,w,z],	R202[y,x,x,x]\n"
		"vmul.q		R101,	R203[z,z,y,y],	R101\n"
		"vadd.q		R100,	R100,	R101\n"

		"vmul.q		R101,	R203[y,x,x,x],	R202[z,z,y,y]\n"
		"vmul.q		R101,	R201[w,w,w,z],	R101\n"
		"vsub.q		R100,	R100,	R101\n"

		"vmul.q		R101,	R203[z,z,y,y],	R202[w,w,w,z]\n"
		"vmul.q		R101,	R201[y,x,x,x],	R101\n"
		"vsub.q		R100,	R100,	R101\n"

		"vmul.q		R101,	R203[w,w,w,z],	R202[y,x,x,x]\n"
		"vmul.q		R101,	R201[z,z,y,y],	R101\n"
		"vsub.q		R100,	R100,	R101\n"

		"vdot.q		S101,	R100,	R200[x,-y,z,-w]\n"

		"vabs.s		S102,	S101\n"
		"mfv		%0,		S102\n"
		:"=r"(d)
		:"m"(m)
	);
	if(d < 0.00001f)//VFPUで扱える最小値･･･？
	{//行列式の値が０だと逆行列が無いので、とりあえず単位行列を返す
		__asm__ volatile(
			"vmidt.q	M000\n"
			"usv.q		R000,	0  + %0\n"
			"usv.q		R001,	16 + %0\n"
			"usv.q		R002,	32 + %0\n"
			"usv.q		R003,	48 + %0\n"
			:"=m"(m)
		);
		return m;
	}
	__asm__ volatile(
		"vrcp.s		S101,	S101\n"
		"vscl.q		C000,	R100[x,-y,z,-w],	S101\n"

		"vmul.q		R102,	R200[y,x,x,x],	R203[w,w,w,z]\n"
		"vmul.q		R103,	R203[y,x,x,x],	R200[w,w,w,z]\n"
		"vsub.q		R102,	R102,	R103\n"
		"vmul.q		C010,	R102,	R202[z,z,y,y]\n"

		"vmul.q		R102,	R200[z,z,y,y],	R203[y,x,x,x]\n"
		"vmul.q		R103,	R203[z,z,y,y],	R200[y,x,x,x]\n"
		"vsub.q		R102,	R102,	R103\n"
		"vmul.q		R102,	R102,	R202[w,w,w,z]\n"
		"vadd.q		C010,	C010,	R102\n"

		"vmul.q		R102,	R200[w,w,w,z],	R203[z,z,y,y]\n"
		"vmul.q		R103,	R203[w,w,w,z],	R200[z,z,y,y]\n"
		"vsub.q		R102,	R102,	R103\n"
		"vmul.q		R102,	R102,	R202[y,x,x,x]\n"
		"vadd.q		C010,	C010,	R102\n"

		"vscl.q		C010,	C010[-x,y,-z,w],	S101\n"

		"vmul.q		R102,	R200[y,x,x,x],	R203[w,w,w,z]\n"
		"vmul.q		R103,	R203[y,x,x,x],	R200[w,w,w,z]\n"
		"vsub.q		R102,	R102,	R103\n"
		"vmul.q		C020,	R102,	R201[z,z,y,y]\n"

		"vmul.q		R102,	R200[z,z,y,y],	R203[y,x,x,x]\n"
		"vmul.q		R103,	R203[z,z,y,y],	R200[y,x,x,x]\n"
		"vsub.q		R102,	R102,	R103\n"
		"vmul.q		R102,	R102,	R201[w,w,w,z]\n"
		"vadd.q		C020,	C020,	R102\n"

		"vmul.q		R102,	R200[w,w,w,z],	R203[z,z,y,y]\n"
		"vmul.q		R103,	R203[w,w,w,z],	R200[z,z,y,y]\n"
		"vsub.q		R102,	R102,	R103\n"
		"vmul.q		R102,	R102,	R201[y,x,x,x]\n"
		"vadd.q		C020,	C020,	R102\n"

		"vscl.q		C020,	C020[x,-y,z,-w],	S101\n"

		"vmul.q		R102,	R200[y,x,x,x],	R202[w,w,w,z]\n"
		"vmul.q		R103,	R202[y,x,x,x],	R200[w,w,w,z]\n"
		"vsub.q		R102,	R102,	R103\n"
		"vmul.q		C030,	R102,	R201[z,z,y,y]\n"

		"vmul.q		R102,	R200[z,z,y,y],	R202[y,x,x,x]\n"
		"vmul.q		R103,	R202[z,z,y,y],	R200[y,x,x,x]\n"
		"vsub.q		R102,	R102,	R103\n"
		"vmul.q		R102,	R102,	R201[w,w,w,z]\n"
		"vadd.q		C030,	C030,	R102\n"

		"vmul.q		R102,	R200[w,w,w,z],	R202[z,z,y,y]\n"
		"vmul.q		R103,	R202[w,w,w,z],	R200[z,z,y,y]\n"
		"vsub.q		R102,	R102,	R103\n"
		"vmul.q		R102,	R102,	R201[y,x,x,x]\n"
		"vadd.q		C030,	C030,	R102\n"

		"vscl.q		C030,	C030[-x,y,-z,w],	S101\n"

		"usv.q		R000,	0  + %0\n"
		"usv.q		R001,	16 + %0\n"
		"usv.q		R002,	32 + %0\n"
		"usv.q		R003,	48 + %0\n"
		:"=m"(m)
		);
	return m;
}
