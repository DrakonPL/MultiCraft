#include "MathHelper.h"


int MathHelper::Cantorize(int x, int z)
{
	x = (x >> 4) + 2047;
	z = (z >> 4) + 2047;

	return x + (z * 4095);
}

int MathHelper::Cantorize(int x, int y, int z)
{
	x = (x >> 4) + 2047;
	y = (y >> 4) + 2047;
	z = (z >> 4) + 2047;

	return x + (y * 4095) + (z * 16769025);
}
