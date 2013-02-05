#ifndef TERRAINGENERATOR_H 
#define TERRAINGENERATOR_H

#include "../World/Chunk.h"
#include "../Utils/Interpolation.h"
#include "../Utils/SimplexNoise.h"

#include "Biome/Biome.h"
#include <math.h>

#include "ChunkHeightMap.h"

#ifdef AURORA_PSP

#include <ext/hash_map> 
typedef __gnu_cxx::hash_map<int,ChunkHeightMap*> HeightMapHash;

#elif AURORA_IOS

#include <ext/hash_map> 
typedef __gnu_cxx::hash_map<int,ChunkHeightMap*> HeightMapHash;

#elif AURORA_UNIX_MAC

#include <ext/hash_map> 
typedef __gnu_cxx::hash_map<int,ChunkHeightMap*> HeightMapHash;

#else

#include <hash_map>
typedef std::hash_map<int,ChunkHeightMap*> HeightMapHash;

#endif

class TerrainGenerator
{
private:

	static TerrainGenerator *_terrainGenerator;

	HeightMapHash _chunkCache;

	SimplexNoise func1,func2,func3;
	Biome *biome;

	int _terrainSeed;

private:

	float InterpolateBiomeNoise(int wx, int wz);
	float InterpolateMoistureNoise(int wx, int wz);
	float InterpolateElevationNoise(int wx, int wz);
	float InterpolateRoughnessNoise(int wx, int wz);

	unsigned char *heightmap;
	unsigned char *humidity;
	unsigned char *temps;

	unsigned char &GetTemp(Chunk* chunk,int x,int z);
	unsigned char &GetHumidity(Chunk* chunk,int x,int z);
	unsigned char &GetHeightMap(Chunk* chunk,int x,int z);

	void _generateChunk(Chunk* chunk,ChunkHeightMap* heightMapChunk);
	void _generateChunkAndHeightMap(Chunk* chunk);


public:
	
	static TerrainGenerator* Instace();

	TerrainGenerator();	

	void SetSeed(int seed);
	void GenerateChunk(Chunk* chunk);
	void CreateFlatTerrain(Chunk* chunk);

	float GetValue3D(int wx, int wy, int wz, int octaves, float startFrequency, float startAmplitude);
	float GetValue2D(int wx, int wz, int octaves, float startFrequency, float startAmplitude);
	float GetValue2D2(int wx, int wz, int octaves, float startFrequency, float startAmplitude);
	float GetValue2D3(int wx, int wz, int octaves, float startFrequency, float startAmplitude);

	void _addChunk(ChunkHeightMap* chunk);
	void _removeChunk(int id);
	ChunkHeightMap* _getChunk(int x,int z);

};

#endif
