#include "../dxlibp.h"

/*
�����̃R�[�h��dxlibp.h��inline�֐��Ƃ��Ă̎����Ɉڍs�ς݂ł��B
�R�[�h�̕ێ�̂��߂ɂ��̃\�[�X�R�[�h�͎c���Ă��܂��B��΂ɏ����Ȃ��ł��������B
*/

VECTOR VGet(float x,float y,float z)
{
	VECTOR res;
	res.x = x;
	res.y = y;
	res.z = z;
	return res;
}

VECTOR VAdd(VECTOR v1,VECTOR v2)
{

	__asm__ volatile (
		"ulv.q		C000,	%0\n"
		"ulv.q		C010,	%1\n"
		"vadd.t		C000,	C000,	C010\n"
		"sv.s		S000,	0 + %0\n"
		"sv.s		S001,	4 + %0\n"
		"sv.s		S002,	8 + %0\n"
		:"+m"(v1)
		:"m"(v2)
	);
	return v1;
}

VECTOR VSub(VECTOR v1,VECTOR v2)
{

	__asm__ volatile (
		"ulv.q		C000,	%0\n"
		"ulv.q		C010,	%1\n"
		"vsub.t		C000,	C000,	C010\n"
		"sv.s		S000,	0 + %0\n"
		"sv.s		S001,	4 + %0\n"
		"sv.s		S002,	8 + %0\n"
		:"+m"(v1)
		:"m"(v2)
	);
	return v1;
}

float VDot(VECTOR v1,VECTOR v2)
{
	register float res;
	__asm__ volatile (
		"ulv.q		C000,	%1\n"
		"ulv.q		C010,	%2\n"
		"vdot.t		S020,	C000,	C010\n"
		"mfv		%0,		S020\n"
		:"=r"(res)
		:"m"(v1),"m"(v2)
	);
	return res;
}

VECTOR VCross(VECTOR v1,VECTOR v2)
{
	__asm__ volatile (
		"ulv.q		C000,	%0\n"
		"ulv.q		C010,	%1\n"
		"vcrsp.t		C000,	C000,	C010\n"
		"sv.s		S000,	0 + %0\n"
		"sv.s		S001,	4 + %0\n"
		"sv.s		S002,	8 + %0\n"
		:"+m"(v1)
		:"m"(v2)
	);
	return v1;
}

VECTOR VScale(VECTOR v,float scale)
{

	__asm__ volatile (
		"ulv.q		C000,	%0\n"
		"mtv		%1,		S010\n"
		"vscl.t		C000,	C000,	S010\n"
		"sv.s		S000,	0 + %0\n"
		"sv.s		S001,	4 + %0\n"
		"sv.s		S002,	8 + %0\n"
		:"+m"(v)
		:"r"(scale)
	);
	return v;
}

float VSize(VECTOR v)
{
	float res;
	__asm__ volatile (
		"ulv.q		C000,	%1\n"
		"vdot.t		S010,	C000,	C000\n"
		"vsqrt.s	S010,	S010\n"
		"sv.s		%0,		S010\n"
		:"=r"(res)
		:"m"(v)
	);
	return res;
}

float VSquareSize(VECTOR v)
{
	register float res;
	__asm__ volatile(
		"ulv.q		C000,	%1\n"
		"vdot.t		S010,	C000,	C000\n"
		"mfv		%0,		S010\n"
		:"=r"(res)
		:"m"(v)
	);
	return res;
}

VECTOR VNorm(VECTOR v)
{
	__asm__ volatile(
		"ulv.q		C000,	%0\n"
		"vdot.t		S010,	C000,	C000\n"
		"vrsq.s		S010,	S010\n"
		"vscl.t		C000,	C000,	S010\n"
		"usv.s		S000,	0 + %0\n"
		"usv.s		S001,	4 + %0\n"
		"usv.s		S002,	8 + %0\n"
		:"+m"(v)
	);
	return v;
}

VECTOR VTransform(VECTOR v,MATRIX m)
{
	__asm__ volatile(
		"ulv.q		C000,	%0\n"
		"vone.s		S003\n"
		"ulv.q		R100,	0  + %1\n"
		"ulv.q		R101,	16 + %1\n"
		"ulv.q		R102,	32 + %1\n"
		"ulv.q		R103,	48 + %1\n"
		"vtfm4.q	C010,	M100,	C000\n"
		"sv.s		S010,	0 + %0\n"
		"sv.s		S011,	4 + %0\n"
		"sv.s		S012,	8 + %0\n"
		:"+m"(v)
		:"m"(m)
	);
	return v;
}

VECTOR VTransformSR(VECTOR v,MATRIX m)
{
	__asm__ volatile(
		"ulv.q		C000,	%0\n"
		"ulv.q		R100,	0  + %1\n"
		"ulv.q		R101,	16 + %1\n"
		"ulv.q		R102,	32 + %1\n"
		"ulv.q		R103,	48 + %1\n"
		"vtfm3.t	C010,	M100,	C000\n"
		"sv.s		S010,	0 + %0\n"
		"sv.s		S011,	4 + %0\n"
		"sv.s		S012,	8 + %0\n"
		:"+m"(v)
		:"m"(m)
	);
	return v;
}

