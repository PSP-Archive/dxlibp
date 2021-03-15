#include "../graphics.h"
#include <pspvfpu.h>

int DrawRotaGraph(int x,int y,float scale,float angle,int gh,int trans,int turn)
{
	DXPGRAPHICSHANDLE *gptr;
	ScePspFMatrix4 pos;
	GHANDLE2GPTR(gptr,gh);
	register int w = gptr->u1 - gptr->u0;
	register int h = gptr->v1 - gptr->v0;

//	pspvfpu_use_matrices(NULL,0,VMAT0 | VMAT1 | VMAT2);//�Ȃ񂩗p�r���悭�킩��Ȃ�

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

		"mtv		%5,		S220\n"			//�摜�̏c�A���̑傫��
		"mtv		%6,		S221\n"
		"vi2f.p		C220,	C220,	1\n"	//int��float�ϊ��Ɠ�����1/2�{����
		"vone.s		S222\n"					//1���Z�b�g�B[w,h,1]�Ƃ����x�N�g�����ł���̂ŁA������ϊ�����

		"vmov.t		C200,	C220[-x,-y,1]\n"//[-w,-h-1]
		"vmov.t		C210,	C220[x,-y,1]\n"//[w,-h-1]
		"vmov.t		C230,	C220[-x,y,1]\n"//[-w,h-1]

		"vtfm3.t	C020,	M100,	C220\n"//�s��ϊ�
		"vtfm3.t	C000,	M100,	C200\n"
		"vtfm3.t	C010,	M100,	C210\n"
		"vtfm3.t	C030,	M100,	C230\n"

		"usv.q		C020,	32 + %0\n"//�ϊ������f�[�^�̏����o��
		"usv.q		C000,	0  + %0\n"
		"usv.q		C010,	16 + %0\n"
		"usv.q		C030,	48 + %0\n"
		:"=m"(pos): "r"(scale), "r"(angle), "r"(x), "r"(y),"r"(w),"r"(h)
	);

	if(turn)return DrawModiGraphF(pos.y.x,pos.y.y,pos.x.x,pos.x.y,pos.w.x,pos.w.y,pos.z.x,pos.z.y,gh,trans);
	return DrawModiGraphF(pos.x.x,pos.x.y,pos.y.x,pos.y.y,pos.z.x,pos.z.y,pos.w.x,pos.w.y,gh,trans);
}



