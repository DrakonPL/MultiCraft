#include "TerrainGenerator.h"
#include "../Blocks/Blocks.h"
#include "../Utils/MathHelper.h"

TerrainGenerator* TerrainGenerator::_terrainGenerator = 0;


TerrainGenerator* TerrainGenerator::Instace()
{
	if (_terrainGenerator == 0)
	{
		_terrainGenerator = new TerrainGenerator();
	}

	return _terrainGenerator;
}


TerrainGenerator::TerrainGenerator()
{
	biome = new Biome();

	heightmap = 0;
	humidity = 0;
	temps = 0;
}

float TerrainGenerator::InterpolateBiomeNoise(int wx, int wz)
{
	float q1, q2, q3, q4, xinterp, yinterp, r1, r2, r3;
	int minx, maxx, minz, maxz;

	minx = floor(wx/16.0f);
	maxx = ceil(wx/16.0f);

	minz = floor(wz/16.0f);
	maxz = ceil(wz/16.0f);

	q1 = (GetValue2D3(minx,minz,1,0.1f,1.0f)+1.0f)/2.0f;
	q2 = (GetValue2D3(maxx,minz,1,0.1f,1.0f)+1.0f)/2.0f;
	q3 = (GetValue2D3(minx,maxz,1,0.1f,1.0f)+1.0f)/2.0f;
	q4 = (GetValue2D3(maxx,maxz,1,0.1f,1.0f)+1.0f)/2.0f;

	xinterp = (wx/16.0f - minx);
	yinterp = (wz/16.0f - minz);

	r1 = Interpolation::Linear(q1,q2,xinterp);
	r2 = Interpolation::Linear(q3,q4,xinterp);

	r3 = Interpolation::Linear(r1,r2,yinterp);

	return r3;
}

float TerrainGenerator::InterpolateMoistureNoise(int wx, int wz)
{
	float q1, q2, q3, q4, xinterp, yinterp, r1, r2, r3;
	int minx, maxx, minz, maxz;

	minx = floor(wx/16.0f);
	maxx = ceil(wx/16.0f);

	minz = floor(wz/16.0f);
	maxz = ceil(wz/16.0f);

	q1 = (GetValue2D2(minx,minz,1,0.1f,1.0f)+1.0f)/2.0f;
	q2 = (GetValue2D2(maxx,minz,1,0.1f,1.0f)+1.0f)/2.0f;
	q3 = (GetValue2D2(minx,maxz,1,0.1f,1.0f)+1.0f)/2.0f;
	q4 = (GetValue2D2(maxx,maxz,1,0.1f,1.0f)+1.0f)/2.0f;


	xinterp = (wx/16.0f - minx);
	yinterp = (wz/16.0f - minz);

	r1 = Interpolation::Linear(q1,q2,xinterp);
	r2 = Interpolation::Linear(q3,q4,xinterp);

	r3 = Interpolation::Linear(r1,r2,yinterp);

	return r3;
}

float TerrainGenerator::InterpolateElevationNoise(int wx, int wz)
{
	float q1, q2, q3, q4, xinterp, yinterp, r1, r2, r3;
	int minx, maxx, minz, maxz;

	minx = floor(wx/4.0f);
	maxx = ceil(wx/4.0f);

	minz = floor(wz/4.0f);
	maxz = ceil(wz/4.0f);

	q1 = GetValue2D(minx,minz,1,0.1f,1.0f);
	q2 = GetValue2D(maxx,minz,1,0.1f,1.0f);
	q3 = GetValue2D(minx,maxz,1,0.1f,1.0f);
	q4 = GetValue2D(maxx,maxz,1,0.1f,1.0f);

	xinterp = (wx/4.0f - minx);
	yinterp = (wz/4.0f - minz);

	r1 = Interpolation::Linear(q1,q2,xinterp);
	r2 = Interpolation::Linear(q3,q4,xinterp);

	r3 = Interpolation::Linear(r1,r2,yinterp);

	return r3;
}

float TerrainGenerator::InterpolateRoughnessNoise(int wx, int wz)
{
	float q1, q2, q3, q4, xinterp, yinterp, r1, r2, r3;
	int minx, maxx, minz, maxz;

	minx = floor(wx/2.0f);
	maxx = ceil(wx/2.0f);

	minz = floor(wz/2.0f);
	maxz = ceil(wz/2.0f);

	q1 = GetValue2D(minx,minz,3,0.1f,1.0f);
	q2 = GetValue2D(maxx,minz,3,0.1f,1.0f);
	q3 = GetValue2D(minx,maxz,3,0.1f,1.0f);
	q4 = GetValue2D(maxx,maxz,3,0.1f,1.0f);

	xinterp = (wx/2.0f - minx);
	yinterp = (wz/2.0f - minz);

	r1 = Interpolation::Linear(q1,q2,xinterp);
	r2 = Interpolation::Linear(q3,q4,xinterp);

	r3 = Interpolation::Linear(r1,r2,yinterp);

	return r3;
}

void TerrainGenerator::SetSeed(int seed)
{
	_terrainSeed = seed;

	func1.setSeed(seed);
	func2.setSeed(seed+1);
	func3.setSeed(seed+2);

	heightmap = new unsigned char[256];
	humidity = new unsigned char[256];
	temps = new unsigned char[256];
}

float TerrainGenerator::GetValue3D(int wx, int wy, int wz, int octaves, float startFrequency, float startAmplitude)
{
	float DIVISOR = 1.0f / 16.0f;

	float x = wx * DIVISOR;
	float y = wy * DIVISOR;
	float z = wz * DIVISOR;

	float frequency = startFrequency;
	float amplitude = startAmplitude;

	float sum = 0.0f;

	for (int octave = 0; octave < octaves; octave++)
	{
		float noise = func1.sdnoise3(x * frequency, y * frequency, z * frequency,0,0,0);

		sum += amplitude * noise;
		frequency *= 2;
		amplitude *= 0.5;
	}

	return sum;
}

float TerrainGenerator::GetValue2D(int wx, int wz, int octaves, float startFrequency, float startAmplitude)
{
	float DIVISOR = 1.0f / 16.0f;

	float x = wx * DIVISOR;
	float z = wz * DIVISOR;

	float frequency = startFrequency;
	float amplitude = startAmplitude;

	float sum = 0.0f;

	for (int octave = 0; octave < octaves; octave++)
	{
		float noise = func1.sdnoise2(x * frequency, z * frequency,0,0);

		sum += amplitude * noise;
		frequency *= 2;
		amplitude *= 0.5;
	}

	return sum;
}

float TerrainGenerator::GetValue2D2(int wx, int wz, int octaves, float startFrequency, float startAmplitude)
{
	float DIVISOR = 1.0f / 16.0f;

	float x = wx * DIVISOR;
	float z = wz * DIVISOR;

	float frequency = startFrequency;
	float amplitude = startAmplitude;

	float sum = 0.0f;

	for (int octave = 0; octave < octaves; octave++)
	{
		float noise = func2.sdnoise2(x * frequency, z * frequency,0,0);

		sum += amplitude * noise;
		frequency *= 2;
		amplitude *= 0.5;
	}

	return sum;
}

float TerrainGenerator::GetValue2D3(int wx, int wz, int octaves, float startFrequency, float startAmplitude)
{
	float DIVISOR = 1.0f / 16.0f;

	float x = wx * DIVISOR;
	float z = wz * DIVISOR;

	float frequency = startFrequency;
	float amplitude = startAmplitude;

	float sum = 0.0f;

	for (int octave = 0; octave < octaves; octave++)
	{
		float noise = func3.sdnoise2(x * frequency, z * frequency,0,0);

		sum += amplitude * noise;
		frequency *= 2;
		amplitude *= 0.5;
	}

	return sum;
}

void TerrainGenerator::_addChunk(ChunkHeightMap* chunk)
{
	_chunkCache[chunk->_chunkID] = chunk;
}

void TerrainGenerator::_removeChunk(int id)
{

}

ChunkHeightMap* TerrainGenerator::_getChunk(int x,int z)
{
	long id = MathHelper::Cantorize(x,z);
	return _chunkCache[id];
}

void TerrainGenerator::_generateChunk(Chunk* chunk,ChunkHeightMap* heightMapChunk)
{
	int x = chunk->GetChunkX();
	int z = chunk->GetChunkZ();
	int width = chunk->GetChunkWidth();
	int height = chunk->GetChunkHeight();

	biome->SetSeed(_terrainSeed + chunk->GetId());

	for(int i = x;i < x + width;i++)
	{
		for(int j = z;j < z + width;j++)
		{
			float Temperature = heightMapChunk->GetTemp(i, j);
			float Humidity = heightMapChunk->GetHumidity(i,j);
			int Height = heightMapChunk->GetHeightMap(i, j);

			Temperature /= 255.0f;
			Humidity /= 255.0f;

			Biome *_biome;

			if(Temperature < 0.25f)
			{
				_biome = biome;
			}
			else if(Temperature > 0.65f && Humidity < 0.25f)
			{
				_biome = biome;
			}
			else
			{
				_biome = biome;
			}

			_biome->GenHeightMap(chunk, i, j, Height, Temperature, Humidity);
			_biome->GenErode(chunk, i, j, Height, Temperature, Humidity);
			_biome->GenBeaches(chunk, i, j, Height, Temperature, Humidity);
			_biome->GenWater(chunk, i, j, Height, Temperature, Humidity);
			_biome->GenGrass(chunk, i, j, Height, Temperature, Humidity);
			_biome->GenTree(chunk, i, j, Height, Temperature, Humidity);			
		}
	}
}

void TerrainGenerator::_generateChunkAndHeightMap(Chunk* chunk)
{
	int x = chunk->GetChunkX();
	int z = chunk->GetChunkZ();
	int width = chunk->GetChunkWidth();
	int height = chunk->GetChunkHeight();

	ChunkHeightMap* heightMapChunk = new ChunkHeightMap(x,z,width);
	biome->SetSeed(_terrainSeed + chunk->GetId());

	for(int i = x;i < x + width;i++)
	{
		for(int j = z;j < z + width;j++)
		{
			float HillFrequency = (GetValue2D(i,j,1,0.05f,1)+1.0f)/2.0f;

			float Temperature = InterpolateBiomeNoise(i,j);
			float Humidity = InterpolateMoistureNoise(i,j);

			int Height = (InterpolateElevationNoise(i,j) + InterpolateRoughnessNoise(i,j)*GetValue2D3(i,j,6,0.1f,1))*64*HillFrequency+64;

			if(Height < 8)
				Height = 8;
			else if(Height > 120)
				Height = 120;

			Temperature = floor(Temperature*255.0f);
			Humidity = floor(Humidity*255.0f);

			heightMapChunk->GetHeightMap(i, j) = Height;
			heightMapChunk->GetTemp(i, j) = Temperature;
			heightMapChunk->GetHumidity(i, j) = Humidity;

			Temperature /= 255.0f;
			Humidity /= 255.0f;

			Biome *_biome;

			if(Temperature < 0.25f)
			{
				_biome = biome;
			}
			else if(Temperature > 0.65f && Humidity < 0.25f)
			{
				_biome = biome;
			}
			else
			{
				_biome = biome;
			}

			_biome->GenHeightMap(chunk, i, j, Height, Temperature, Humidity);
			_biome->GenErode(chunk, i, j, Height, Temperature, Humidity);
			_biome->GenBeaches(chunk, i, j, Height, Temperature, Humidity);
			_biome->GenWater(chunk, i, j, Height, Temperature, Humidity);
			_biome->GenGrass(chunk, i, j, Height, Temperature, Humidity);
			_biome->GenTree(chunk, i, j, Height, Temperature, Humidity);			
		}
	}

	_addChunk(heightMapChunk);
}

void TerrainGenerator::GenerateChunk(Chunk* chunk)
{
	ChunkHeightMap* heightmapChunk = _getChunk(chunk->GetChunkX(),chunk->GetChunkZ());

	if (heightmapChunk != NULL)
	{
		_generateChunk(chunk,heightmapChunk);
	}else
	{
		_generateChunkAndHeightMap(chunk);
	}
}

void TerrainGenerator::CreateFlatTerrain(Chunk* chunk)
{
	int x = chunk->GetChunkX();
	int y = chunk->GetChunkY();
	int z = chunk->GetChunkZ();

	int width = chunk->GetChunkWidth();
	int height = chunk->GetChunkHeight();

	int terrainHeight = 64;

	bool notEmpty = false;

	for (int h = y; h < y + height;h++)
	{
		for(int i = x;i < x + width;i++)
		{
			for(int j = z;j < z + width;j++)
			{
				if ( i%2 == 0 && j%5 == 0)
				{
					//if (i%2 == 0)
					//{
					//	terrainHeight = 65;
					//}else
						terrainHeight = 66;
				}else
				{
					terrainHeight = 64;
				}

				if (h < terrainHeight)
				{
					chunk->SetBlock(i,h,j,Blocks::Planks);
					notEmpty = true;
				}

				//if (h == 68)
				//	chunk->GetBlock(i,h,j) = Blocks::Stone;
			}
		}
	}

	if (!notEmpty)
	{
		chunk->RemoveBlocks();
	}
}