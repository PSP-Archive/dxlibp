#include "../../graphics.h"
#ifndef DXP_BUILDOPTION_NO3D
#include <fastmath.h>

int SetCameraPositionAndTargetAndUpVec( VECTOR Position, VECTOR Target, VECTOR Up )
{
	ScePspFMatrix4 zinv = //ç¿ïWånÇÃà·Ç¢Çãzé˚Ç≥ÇπÇÈ
	{
		{1,0,0,0},
		{0,1,0,0},
		{0,0,-1,0},
		{0,0,0,1},
	};
	Position.z *= -1;
	sceGumMatrixMode(GU_VIEW);
	sceGumLoadIdentity();
	sceGumLookAt(&Position,&Target,&Up);
	sceGumMultMatrix(&zinv);
	sceGumStoreMatrix(&dxpGraphicsData.camera.viewMatrix);
	return 0;
}

void dxpGraphics3dUpdateProjectionMatrix()
{
	sceGumMatrixMode(GU_PROJECTION);
	sceGumLoadIdentity();
	sceGumPerspective(
		dxpGraphicsData.camera.fov,
		dxpGraphicsData.camera.aspect,
		dxpGraphicsData.camera.near,
		dxpGraphicsData.camera.far
	);
	sceGumStoreMatrix(&dxpGraphicsData.camera.projectionMatrix);
}

int SetupCamera_ProjectionMatrix( MATRIX ProjectionMatrix )
{
	dxpGraphicsData.camera.projectionMatrix = ProjectionMatrix;
	sceGumMatrixMode(GU_PROJECTION);
	sceGumLoadMatrix(&ProjectionMatrix);
	return 0;
}

int SetupCamera_Perspective( float Fov )
{
	dxpGraphicsData.camera.fov = Fov * 180 / GU_PI;
	dxpGraphics3dUpdateProjectionMatrix();
	return 0;
}

int SetCameraNearFar( float Near, float Far )
{
	dxpGraphicsData.camera.near = Near;
	dxpGraphicsData.camera.far = Far;
	dxpGraphics3dUpdateProjectionMatrix();
	return 0;
}

int SetCameraDotAspect(float Aspect)
{
	dxpGraphicsData.camera.aspect = Aspect;
	dxpGraphics3dUpdateProjectionMatrix();
	return 0;
}

#endif