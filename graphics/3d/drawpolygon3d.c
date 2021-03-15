#include "../../graphics.h"
#ifndef DXP_BUILDOPTION_NO3D


int DrawPolygon3D( VERTEX3D *Vertex, int PolygonNum, int GrHandle, int TransFlag )
{
	DXPGRAPHICSHANDLE *gptr;
	GHANDLE2GPTR(gptr,GrHandle);
	dxpGraphicsSetup3DTex(gptr,TransFlag);
	sceGumDrawArray(GU_TRIANGLES,GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_NORMAL_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_3D,3 * PolygonNum,0,Vertex);
	return 0;
}

#endif
