#include "Biome.h"
#include "TreeGenerator.h"

#include "Aurora/Math/MathUtils.h"
#include "../../Utils/mersenne-twister.h"

Biome::Biome()
{

}

void Biome::SetSeed(float seed)
{
	mt::srand(seed);
	_seed = seed;
}

int Biome::GenHeightMap(Chunk * c, int x, int z, int h2, float t, float h)
{
	int height = h2 >= (c->GetChunkY() + c->GetChunkHeight()) ? c->GetChunkY() + c->GetChunkHeight() : h2; 

	for (int y = c->GetChunkY(); y < height; ++y)
	{
		c->SetBlock(x,y,z,Blocks::Stone);
	}

	return h2;
}

int Biome::GenErode(Chunk * c, int x, int z, int hm, float t, float h)
{
	for (int y = 1; y < 3; ++y)
	{
		if (hm-y >= c->GetChunkY() && hm-y < (c->GetChunkY() + c->GetChunkHeight()))
			c->SetBlock(x,hm-y,z,Blocks::Dirt);
	}

	return hm;
}

int Biome::GenGrass(Chunk * c, int x, int z, int hm, float t, float h)
{
	if (hm-1 >= c->GetChunkY() && hm-1 < (c->GetChunkY() + c->GetChunkHeight()))
	{
		if(c->GetBlock(x,hm-1,z) == Blocks::Dirt)
		{
			c->SetBlock(x,hm-1,z,Blocks::Grass);
		}
	}
	

	return hm;
}

int Biome::GenBeaches(Chunk * c, int x, int z, int hm, float t, float h)
{
	/*for (int y = 60; y < 66; ++y)
	{				
		if(c->GetBlock(x,y,z))
			c->GetBlock(x,y,z) = 5;
	}*/

	if(hm-1 < 66)
	{
		for(int y=1; y<3; ++y)
		{
			if (hm-y >= c->GetChunkY() && hm-y < (c->GetChunkY() + c->GetChunkHeight()))
			{
				if(c->GetBlock(x,hm-y,z))
					c->SetBlock(x,hm-y,z,Blocks::Sand);
			}
		}
	}

	return hm;
}

int Biome::GenWater(Chunk * c, int x, int z, int hm, float t, float h)
{
	int height = 64 >= (c->GetChunkY() + c->GetChunkHeight()) ? c->GetChunkY() + c->GetChunkHeight() : 64; 

	for (int y = c->GetChunkY(); y < height; ++y)
	{				
		if(c->GetBlock(x,y,z) == Blocks::Air)
		{
			c->SetBlock(x,y,z,Blocks::WaterStill);
			hm = 64;
		}
	}

	return hm;
}

int Biome::GenTree(Chunk * c, int x, int z, int hm, float t, float h)
{
	if (hm-1 >= c->GetChunkY() && hm-1 < (c->GetChunkY() + c->GetChunkHeight()))
	{
		if(c->GetBlock(x,hm-1,z) == Blocks::Grass || c->GetBlock(x,hm-1,z) == Blocks::Dirt)
		{
			if(floorf((mt::rand()%256*(1-h*(1-fabsf(t-0.5f)*2))))==0) // bigger number less trees. We need to switch rand to something that
				TreeGenerator::BuildTree(x, hm, z, mt::rand() % 4 + 4, c); // will always be the same for each point
		}
	}
	return hm;
}

int Biome::GenPassTwo(Chunk * c, int x, int z, int hm, float t, float h)
{
	return hm;
}