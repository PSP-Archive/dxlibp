#include "../graphics.h"

int LoadGraphScreen(int x,int y,const char* filename,int trans)
{
	int gh,res;
	gh = LoadGraph(filename);
	clsDx();
	if(gh == -1)return -1;
	res = DrawGraph(x,y,gh,trans);
	DeleteGraph(gh);
	return res;
}