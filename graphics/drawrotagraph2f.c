#include "../graphics.h"

int DrawRotaGraph2F(float x,float y,float cx,float cy,float scale,float angle,int gh,int trans,int turn)
{
	DXPGRAPHICSHANDLE *gptr;
	ScePspFMatrix4 pos;
	GHANDLE2GPTR(gptr,gh);
	register int w = gptr->u1 - gptr->u0;
	register int h = gptr->v1 - gptr->v0;

	__asm__ volatile(
		"mtv		%1,		S000\n"			//scale
		"mtv		%2,		S001\n"			//angle

		"vcst.s		S002,	VFPU_2_PI\n"	//angle�����W�A���P�ʂ���VFPU�����̊p�x�P�ʂɒ���
		"vmul.s		S001,	S001[-x],	S002\n"

		"vmidt.t	M100\n"					//��]�s����쐬
		"vrot.t		C100,	S001,	[c,s,0]\n"
		"vrot.t		C110,	S001,	[-s,c,0]\n"

		"vmscl.t	M100,	M100,	S000\n"	//��]�s���scale�{����

		"mtv		%3,		S102\n"			//x,y���W����]�s��ɓ����B����ŕϊ��p�̍s�񂪊�������
		"mtv		%4,		S112\n"
		"vi2f.p		R102,	R102,	1/2\n"	//int��float�ϊ�

		"mtv		%5,		S010\n"			//�摜�̏c�A���̑傫��
		"mtv		%6,		S011\n"

		"mtv		%7,		S020\n"			//cx,cy
		"mtv		%8,		S021\n"

		"vadd.t		C200,	C020[-x,-y,1],	C010[0,0,0]\n"
		"vadd.t		C210,	C020[-x,-y,1],	C010[x,0,0]\n"
		"vadd.t		C220,	C020[-x,-y,1],	C010[x,y,0]\n"
		"vadd.t		C230,	C020[-x,-y,1],	C010[0,y,0]\n"

		"vtfm3.t	C000,	M100,	C200\n"//�s��ϊ�
		"vtfm3.t	C010,	M100,	C210\n"
		"vtfm3.t	C020,	M100,	C220\n"
		"vtfm3.t	C030,	M100,	C230\n"

		"usv.q		C000,	0  + %0\n"//�ϊ������f�[�^�̏����o��
		"usv.q		C010,	16 + %0\n"
		"usv.q		C020,	32 + %0\n"
		"usv.q		C030,	48 + %0\n"
		:"=m"(pos): "r"(scale), "r"(angle), "r"(x), "r"(y),"r"(w),"r"(h),"r"(cx),"r"(cy)
	);

	if(turn)return DrawModiGraphF(pos.y.x,pos.y.y,pos.x.x,pos.x.y,pos.w.x,pos.w.y,pos.z.x,pos.z.y,gh,trans);
	return DrawModiGraphF(pos.x.x,pos.x.y,pos.y.x,pos.y.y,pos.z.x,pos.z.y,pos.w.x,pos.w.y,gh,trans);

}