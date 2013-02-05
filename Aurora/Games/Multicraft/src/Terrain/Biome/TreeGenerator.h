#ifndef TREEGENERATOR_H
#define TREEGENERATOR_H

#include "../../World/Chunk.h"
#include <math.h>

class TreeGenerator
{
public:
	static void BuildTree(int wx, int wy, int wz, int height, Chunk * chunk);
};


#endif