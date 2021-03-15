#include "../graphics.h"

int SetDisplayFormat(int psm)
{
	return SetGraphMode(480,272,psm);
}