#ifndef CHUNK_H
#define CHUNK_H

#include <Aurora/Math/BoundingBox.h>
#include <Aurora/Graphics/VertexBuffer.h>
#include <Aurora/Threads/Mutex.hpp>

using namespace Aurora::Math;
using namespace Aurora::Graphics;
using namespace Aurora::Threads;

class World;
class VertexBuilder;
class ChunkProvider;

class Chunk
{
private:

	unsigned char *_blocks;

	int _chunkId;
	
	int _chunkWidth;
	int _chunkWidth2;
	int _chunkHeight;

	int _chunkX;
	int _chunkY;
	int _chunkZ;

	bool _dirty;
	bool _cached;
	bool _fresh;
	bool _processing;
	bool _needSave;

	float _distanceFromPlayer;
	
	BoundingBox _aabb;

	VertexBuffer _opaqueVBO;
	VertexBuffer _transparentVBO;

	//Mutex _mutex;

	World *_world;

private:

	void _initData();

public:

	Chunk(World* world,int x,int y,int z,int width,int height);
	~Chunk();
	
	void RemoveBlocks();

	//generate chunk terrain data
	void Generate();

	//build chunk vertices
	void BuildMesh();
	void BuildVBO();
	void ResetVBO();

	

	//draw chunk
	void Draw(bool state);
	
	unsigned char GetBlock(int x,int y,int z);
	void SetBlock(int x,int y,int z,unsigned char blockValue);

	bool IsBlockTransparent(int x,int y,int z);
	bool IsBlockVisible(int x,int y,int z);
	bool IsBlockSpecial(int x,int y,int z);
	bool IsBlockSingle(int x,int y,int z);

	bool IsProcessing();
	void SetProcessing(bool state);

	inline int GetId() { return _chunkId; }

	inline int GetChunkWidth(){ return _chunkWidth; }
	inline int GetChunkWidth2(){ return _chunkWidth2; }
	inline int GetChunkHeight(){ return _chunkHeight; }

	inline int GetChunkX(){ return _chunkX; }
	inline int GetChunkY(){ return _chunkY; }
	inline int GetChunkZ(){ return _chunkZ; }

	inline void SetDirty(bool state) { _dirty = state; }
	inline void SetCached(bool state) { _cached = state; }
	inline void SetSaveState(bool state) {_needSave = state; }

	inline bool IsDirty(){ return _dirty; }
	inline bool IsCached(){ return _cached; }
	inline bool IsFresh(){ return _fresh; }
	inline bool IsSaveNeed() { return _needSave; }

	inline float GetDistanceToPlayer(){ return _distanceFromPlayer; }
	inline BoundingBox GetAabb() { return _aabb; }

	friend class World;
	friend class VertexBuilder;
	friend class ChunkProvider;
};


#endif
