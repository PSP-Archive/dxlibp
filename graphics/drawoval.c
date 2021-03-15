#include "../graphics.h"
#include <math.h>

int DrawOval(int x,int y,int rx,int ry,int color,int fillflag)
{
	int i;
	GUINITCHECK;
	if(fillflag)
	{
		for(i = 0;i < 32;++i)
			DrawTriangle(x,y,x + rx * cos(2 * M_PI * i / 32),y + ry * sin(2 * M_PI * i / 32),x + rx * cos(2 * M_PI * (i + 1) / 32),y + ry * sin(2 * M_PI * (i + 1) / 32),color,1);
	}else
	{
		for(i = 0;i < 32;++i)
			DrawLine(x + rx * cos(2 * M_PI * i / 32),y + ry * sin(2 * M_PI * i / 32),x + rx * cos(2 * M_PI * (i + 1) / 32),y + ry * sin(2 * M_PI * (i + 1) / 32),color);
	}
	return 0;
}

