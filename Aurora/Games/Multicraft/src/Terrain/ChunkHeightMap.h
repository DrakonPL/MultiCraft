#ifndef CHUNKHEIGHTMAP_H
#define CHUNKHEIGHTMAP_H

class ChunkHeightMap
{
public:

	long _chunkID;
	int _chunkX,_chunkZ;
	int _chunkWidth;

	unsigned char *heightmap;
	unsigned char *humidity;
	unsigned char *temps;

public:

	ChunkHeightMap(int posX,int posZ,int chunkWidth);
	~ChunkHeightMap();

	unsigned char &GetTemp(int x,int z);
	unsigned char &GetHumidity(int x,int z);
	unsigned char &GetHeightMap(int x,int z);

};

#endif

