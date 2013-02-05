#ifndef BLOCKPOSITION_H 
#define BLOCKPOSITION_H

#include <Aurora/Math/Vector3.h>

using namespace Aurora::Math;

class BlockPosition
{
public:

	Vector3 position;
	float distanceToOrigin;

	BlockPosition(int x,int y,int z,float distance)
	{
		position.set(x,y,z);
		distanceToOrigin = distance;
	}

};

#endif