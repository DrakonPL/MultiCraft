#ifndef CHUNKPROVIDER_H
#define CHUNKPROVIDER_H

#include "Chunk.h"

#include <string>


class ChunkProvider
{
private:

	std::string _folderName;

public:

	ChunkProvider(std::string folderName);

	bool LoadChunk(Chunk* chunk);
	bool SaveChunk(Chunk* chunk);

};

#endif