#include "dxlibp.h"
#include <pspgu.h>
#include <pspgum.h>

#include <valloc.h>
/*
3D�n�̉摜�������ꍇ�͉摜�T�C�Y��2^n�݂̂Ƃ���

���_�o�b�t�@���t���b�V���������

�e�N�X�`���̐؂�ւ�
2D�A3D�̐؂�ւ�
�e�N�X�`���̗L��
�o�b�t�@������Ȃ��Ȃ�����

���_�o�b�t�@�̏������݌�ɃI�[�o�[�������Ă�����G���[�o���ċ����I��
���_�o�b�t�@�̏������݌�Ɏc�肪1���؂��Ă�����t���b�V��

VRAM��ł�swizzle���Ȃ�
DRAM��ł�swizzle����
��VRAM-DRAM�Ԃ̃f�[�^�]���̎��_��swizzle�Aunswizzle���������Ă��܂���
*/

//macros ----
#define VRAM_BASE	(0x44000000)
#define VRAM_OFFSET2ADDRESS(OFFSET) (VRAM_BASE | OFFSET)

#define GULIST_LEN	262144	/*(256 * 1024) 1MByte*/
#define GULIST_LIM	200000


#define DXP_DRAWSTATE_EITHER	0xffffffff
#define DXP_DRAWSTATE_NONTEX2D	0
#define DXP_DRAWSTATE_TEX2D		1
#define DXP_DRAWSTATE_NONTEX3D	2
#define DXP_DRAWSTATE_TEX3D		3
#define DXP_DRAWSTATE_INTRAFONT	4
#define DXP_DRAWSTATE_MODEL		5

#define GUENABLE(STATUS) {if(!sceGuGetStatus(STATUS))sceGuEnable(STATUS);}
#define GUDISABLE(STATUS) {if(sceGuGetStatus(STATUS))sceGuDisable(STATUS);}
#define GUINITCHECK	{if(!dxpGraphicsData.init)return -1;}
#define GUSTART		{if(!dxpGraphicsData.gustarted){sceGuStart(GU_DIRECT,dxpGuList);dxpGraphicsData.gustarted = 1;}}
#define GUFINISH	{if(dxpGraphicsData.gustarted){sceGuFinish();dxpGraphicsData.gustarted = 0;}}
#define GUSYNC		{GUFINISH;sceGuSync(0,0);}

#define MIN(a,b) (a < b ? a : b)
#define GHANDLE2GPTR(PTR,HANDLE) {if(HANDLE < 0 || HANDLE >= DXP_BUILDOPTION_GHANDLE_MAXNUM)return -1;PTR = dxpGraphicsData.grapharray[HANDLE];if(!PTR)return -1;if(!PTR->tex)return -1;}

static inline int dxpN_2(int num)
{
	unsigned int i;
	num -= 1;
	__asm__(
		"clz %0, %1\n"
		:"=r"(i):"r"(num)
	);
	return 1 << (32-i);
}

static inline u32 dxpGraphics4444_8888(u32 src)
{
	return ((src & 0xf000) << 28) | ((src & 0xf000) << 24) | ((src & 0x0f00) << 20) | ((src & 0x0f00) << 16) | ((src & 0x00f0) << 12) | ((src & 0x00f0) << 8 ) | ((src & 0x000f) << 4 ) | ((src & 0x000f) << 0 );
}

static inline u32 dxpGraphics5551_8888(u32 src)
{
	return ((src & 0x8000) << 16) | ((src & 0x8000) << 15) | ((src & 0x8000) << 14) | ((src & 0x8000) << 13) | ((src & 0x8000) << 12) | ((src & 0x8000) << 11) | ((src & 0x8000) << 10) | ((src & 0x8000) << 9) |
		((src & 0x7c00) << 9) | ((src & 0x7000) << 4) | ((src & 0x03e0) << 6) | ((src & 0x0380) << 1) | ((src & 0x001f) << 3) | ((src & 0x001c) >> 2);
}

static inline u32 dxpGraphics5650_8888(u32 src)
{
	return 0xff000000 | ((src & 0xf800) << 8) | ((src & 0xe000) << 3) | ((src & 0x07e0) << 5) | ((src & 0x0600) >> 1) | ((src & 0x001f) << 3) | ((src & 0x001c) >> 2);
}

static inline u32 dxpGraphics8888_4444(u32 src)
{
	return ((src & 0xf0000000) >> 16) | ((src & 0x00f00000) >> 12) | ((src & 0x0000f000) >> 8 ) | ((src & 0x000000f0) >> 4 );
}

static inline u32 dxpGraphics8888_5551(u32 src)
{
	return ((src & 0x80000000) >> 16) | ((src & 0x00f80000) >> 9 ) | ((src & 0x0000f800) >> 6 ) | ((src & 0x000000f8) >> 3 );
}

static inline u32 dxpGraphics8888_5650(u32 src)
{
	return ((src & 0x00f80000) >> 8) | ((src & 0x0000fc00) >> 5) | ((src & 0x000000f8) >> 3);
}
//structures ----

//typedef struct STREAMDATA STREAMDATA;

#define DXP_VTYPE_2D	(GU_VERTEX_16BIT)
typedef struct DXP_FVF_2D
{
	s16			x,y,z;
}DXP_FVF_2D;

#define DXP_VTYPE_2DTEX	(GU_VERTEX_16BIT | GU_TEXTURE_16BIT)
typedef struct DXP_FVF_2DTEX
{
	u16			u,v;
	s16			x,y,z;
}DXP_FVF_2DTEX;

#define DXP_VTYPE_2DTEX_F (GU_VERTEX_32BITF | GU_TEXTURE_16BIT)
typedef struct DXP_FVF_2DTEX_F
{
	u16			u,v;
	float		x,y,z;
}DXP_FVF_2DTEX_F;

#define DXP_VTYPE_3D GU_VERTEX_32BITF
typedef struct DXP_FVF_3D
{
	float x,y,z;
}DXP_FVF_3D;

typedef struct DXPTEXTURE3
{
	int			psm;
	u32			*ppalette;
	void*		texdata;				/*�e�N�X�`���̎��f�[�^*/
	void*		texvram;				/*vram�ɃR�s�[�����f�[�^*/
	u16			width;					/*�e�N�X�`���̉��T�C�Y*/
	u16			height;					/*�e�N�X�`���̏c�T�C�Y*/
	u16			pitch;					/*�e�N�X�`���̉��T�C�Y*/
	u16			umax,vmax;				/*�L����UV���W�̍ő�l*/
	u32			colorkey;				/*�J���[�L�[*/

	unsigned	size2_nflag:1;			/*�e�N�X�`���T�C�Y��2��n��ɂȂ��Ă��邩�ǂ���*/
	unsigned	swizzledflag:1;
	unsigned	reloadflag:1;
	unsigned	alphabit:1;				/*�ǂݍ��񂾉摜�Ƀ���񂪑��݂������ǂ���*/
	u32			refcount;
	struct DXPTEXTURE3 **thisptrptr;			/*�C���f�b�N�X���e�N�X�`���z��̂ǂ����������w���Ă��邩*/
}DXPTEXTURE3;

typedef	struct	DXPGRAPHICSHANDLE
{
	DXPTEXTURE3	*tex;
	u16			u0,v0,u1,v1;
	int			handle;
}DXPGRAPHICSHANDLE;

typedef struct DXPMODEL_VERTEX
{
//	float weight;
	float uv;
	COLOR_U8 color;
	VECTOR norm;
	VECTOR pos;
}DXPMODEL_VERTEX;


typedef struct DXPMODEL_TEXTURE
{
	int handle;
	int backup;
	int addressMode[2];
	int filterMode;
}DXPMODEL_TEXTURE;

typedef struct DXPMODEL_MATERIAL
{
	//���O
	char *name;
	//�F
	COLOR_U8 ambient;
	COLOR_U8 emissive;
	COLOR_U8 diffuse;
	COLOR_U8 specular;
	float specularPower;
	//�e�N�X�`��
	int texIndex;
	//�p�����[�^
	int blendMode;
	int blendParam;
}DXPMODEL_MATERIAL;

typedef struct DXPMODEL_SHAPE
{
	
}DXPMODEL_SHAPE;

typedef struct DXPMODEL_MESH
{
	char *name;
	//bone
	//int boneIndex;
	//material
	int materialIndex;
	DXPMODEL_SHAPE *shape;
}DXPMODEL_MESH;

typedef struct DXPMODEL_BONE
{
	MATRIX Offset;
	int MainFrame;
	MATRIX q;
}DXPMODEL_BONE;

typedef struct DXPMODEL_FRAME
{
	struct
	{
		struct DXPMODEL_FRAME//�񕪖ؓI�ȕ��ŊǗ�����
			*parent,
			*next,
			*prev,
			*child;
	}fcb;

	char *name;
	int MeshNum;
	DXPMODEL_MESH *Mesh;

	VECTOR Translate;
	VECTOR Scale;
	QUATERNION Rotate;
	
	unsigned visible : 1;


	
}DXPMODEL_FRAME;

typedef struct DXPMODEL
{
	char *name;
	//
	VECTOR Translate;
	VECTOR Scale;
	QUATERNION Rotate;
	MATRIX Local2WorldMatrix;
	struct
	{
		COLOR_F 
			diffuse,
			specular,
			emissive,
			ambient;
	}ColorScale;//�}�e���A���J���[��CPU�ŏ�Z���Ă���
	unsigned SemiTransFlag : 1;//�������̃��m�������Ă��邩�ǂ���
	float Alpha;
	unsigned visible : 1;
	unsigned usedepth : 1;
	unsigned writedepth : 1;
	unsigned usevertdiffcolor : 1;

	//bone
	;
	//texture
	int TextureNum;
	DXPMODEL_TEXTURE *Texture;
	//material
	int MaterialNum;
	DXPMODEL_MATERIAL *Material;
	//mesh
	int MeshNum;
	DXPMODEL_MESH *Mesh;
}DXPMODEL;


typedef struct DXPGRAPHICSDATA
{
	unsigned	init : 1;
	u32			display_psm;
	DXPTEXTURE3	displaybuffer[2];		/*�t���[���o�b�t�@*/
	DXPTEXTURE3	*displaybuffer_back;	/*�o�b�N�o�b�t�@*/
	DXPTEXTURE3	depthbuffer;			/*�[�x�o�b�t�@*/
	DXPTEXTURE3	*rendertarget;			/*�`���*/
	DXPTEXTURE3	*texture;				/*�e�N�X�`��*/

	void (*debugScreenCallback)();

	u32			clear_color_value;
	u32			clear_depth_value;
	u32			clear_stencil_value;

	unsigned	gustarted : 1;			/*ON between GUSTART and GUFINISH*/
	unsigned	clear_depth	: 1;		/*clear depth buffer when ClearDrawScreen is called*/
	unsigned	clear_stencil : 1;		/*clear stencil buffer when ClearDrawScreen is called*/
	unsigned	bilinear_enable : 1;	/*bilinear or nearest*/
	unsigned	create_vram_graph: 1;
	unsigned	create_swizzled_graph: 1;
	unsigned	usestencil:1;
	unsigned	writestencil:1;
	unsigned	waitvsinc:1;

	u32			colorkey;	/*�J���[�L�[*/
	int			blendmode;	/*�u�����h���[�h*/
	u32			drawstate;
	int			flag;
	unsigned	forceupdate : 1;
	u16	z_2d;		/*2D�`�掞�Ɏw�肳���z���W*/
	u32	color;	/*SetDrawBright��SetDrawBlendmode�̃p�����[�^*/

	u32 gucolor;/*�ݒ肳�ꂽ�f�B�t���[�Y�F*/
	int op;
	int src;
	int dest;
	u32 srcfix;
	u32 destfix;
	int tfx,tcc;
	unsigned usedepth:1;
	unsigned writedepth:1;
	unsigned usedepth3d:1;
	unsigned writedepth3d:1;
	float fog_near,fog_far;
	u32 fog_color;

	int intrafont_statebuf;
	int intrafont_scissor[4];
	int depthfunc;
	struct
	{
		MATRIX viewMatrix;
		float near;
		float far;
		float aspect;
		float fov;
		MATRIX projectionMatrix;
	}camera;


	DXPTEXTURE3 *texarray[DXP_BUILDOPTION_TEXTURE_MAXNUM];
	DXPGRAPHICSHANDLE *grapharray[DXP_BUILDOPTION_GHANDLE_MAXNUM];
	DXPMODEL *modelarray[DXP_BUILDOPTION_MODEL_MAXNUM];
}DXPGRAPHICSDATA;

int dxpGraphicsInit();
int dxpGraphicsEnd();

int dxpGraphicsCalcTexSize(int width,int height,int psm);

DXPTEXTURE3* dxpGraphicsCreateTexture();
int dxpGraphicsReleseTexture(DXPTEXTURE3* texptr);

DXPGRAPHICSHANDLE* dxpGraphicsCreateGraphicHandle();
int dxpGraphicsReleseGraphicHandle(DXPGRAPHICSHANDLE* gptr);


int dxpGraphicsSetup2DTex(DXPTEXTURE3 *texptr,int flag);
int dxpGraphicsSetup3DTex(DXPGRAPHICSHANDLE *gptr,int flag);
int dxpGraphicsSetup2D(u32 color);
int dxpGraphicsSetup3D(u32 color);

#define PSM2BYTEX2(PSM) (PSM == GU_PSM_5650 || PSM == GU_PSM_5551 || PSM == GU_PSM_4444 ? 4 : (PSM == GU_PSM_8888 ? 8 : (PSM == GU_PSM_T8 ? 2 : 1)))

void dxpGraphicsWaitVSync();

//variables ----

extern DXPGRAPHICSDATA dxpGraphicsData;
extern u32 dxpGuList[GULIST_LEN];
extern u8 dxpPsm2SliceSize[11][2];

static inline DXPTEXTURE3* dxpGraphHandle2TexPtr(int gh)
{
	if(gh < 0 || gh >= DXP_BUILDOPTION_GHANDLE_MAXNUM)return NULL;
	return dxpGraphicsData.grapharray[gh]->tex;
}

static inline void* dxpGuGetMemory(u32 size)
{
	int p = sceGuCheckList();
	if(p > GULIST_LEN){return NULL;}//�v���I�ȃG���[�B�o�b�t�@�I�[�o�[�������������Ă���
	if(p > GULIST_LIM){GUSYNC;GUSTART;}//�R�}���h���X�g���c�菭�Ȃ��̂ň�x�������Ƃ�
	return sceGuGetMemory(size);
}

void dxpGraphics3dUpdateProjectionMatrix();

//void printfDxMatrix(MATRIX* m);
//void printfDxVector(VECTOR *v);