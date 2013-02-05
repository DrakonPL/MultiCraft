#ifndef BIOME_H
#define BIOME_H

#include "../../Blocks/Blocks.h"
#include "../../World/Chunk.h"
#include "../../Utils/Interpolation.h"
#include "../../Utils/SimplexNoise.h"
#include <math.h>

class Biome
{
protected:

	int _seed;

public:

	Biome();

	virtual void SetSeed(float seed);

	virtual int GenHeightMap(Chunk * c, int x, int z, int h2, float t, float h);
	virtual int GenErode(Chunk * c, int x, int z, int hm, float t, float h);
	virtual int GenGrass(Chunk * c, int x, int z, int hm, float t, float h);
	virtual int GenBeaches(Chunk * c, int x, int z, int hm, float t, float h);
	virtual int GenWater(Chunk * c, int x, int z, int hm, float t, float h);
	virtual int GenTree(Chunk * c, int x, int z, int hm, float t, float h);
	virtual int GenPassTwo(Chunk * c, int x, int z, int hm, float t, float h);
};

#endif