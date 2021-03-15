#include "../graphics.h"

int DrawCircle(int x,int y,int r,int color,int fillflag)
{
	return DrawOval(x,y,r,r,color,fillflag);
}