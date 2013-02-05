#ifndef WORLD_H
#define WORLD_H

#include "../Blocks/Blocks.h"
#include "Chunk.h"
#include "Player.h"

#include "../Utils/TexturePack.h"
#include "ChunkProvider.h"

#include <string>
#include <vector>

#ifdef AURORA_PSP

#include <ext/hash_map> 
typedef __gnu_cxx::hash_map<int,Chunk*> MapHash;

#elif AURORA_IOS

#include <ext/hash_map> 
typedef __gnu_cxx::hash_map<int,Chunk*> MapHash;

#elif AURORA_UNIX_MAC

#include <ext/hash_map> 
typedef __gnu_cxx::hash_map<int,Chunk*> MapHash;

#else

#include <hash_map>
typedef std::hash_map<int,Chunk*> MapHash;

#endif

#include <Aurora/System/Clock.h>

//thread

#include <Aurora/Threads/Thread.hpp>
#include <Aurora/Threads/Mutex.hpp>
#include <Aurora/Threads/Sleep.h>
#include <Aurora/Threads/ConcurentQueue.h>

using namespace Aurora::Threads;

class VertexBuilder;

class World
{
private:

	//world texture properties
	int _textureSize;
	int _textureChunk;
	float _texturePercent;

	//chunk properties
	int _chunkWidth;
	int _chunkHeight;
	int _lastChunk;

	int _worldSeed;

	int _visibleChunksNumber;

	std::string _worldName;

	std::vector<Chunk*> _visibleChunks;
	std::vector<Chunk*> _dirtyChunks;
	std::vector<Chunk*> _updateChunks;
	std::vector<Chunk*> _neighbors;
	std::vector<Chunk*> _deleteChunks;

	ConcurentQueue< std::vector<Chunk*> > _toProcess;
	ConcurentQueue< std::vector<Chunk*> > _fromProcess;
	Thread *_processThread;

	ChunkProvider* _chunkprovider;

	MapHash _chunkCache;

	Player* _player;

	TexturePack* _texturePack;

	float _updateChunksTimer;
private:

	void _initData();

	//chunks	
	void _updateChunk(Chunk* chunk);
	void _processChunks();
	void _updateChunksDisplay();


	void _divideWordAABB(int minx,int miny,int minz,int maxx,int maxy,int maxz);
	void _checkWorldAABB(int minx,int miny,int minz,int maxx,int maxy,int maxz);

	void _createOrGetNeighbors(int x,int y,int z);

	Chunk* _loadOrPrepareChunk(int x,int y,int z);
	Chunk* _getChunkById(int id);	

	//cache
	Chunk* _getChunkFromCache(int id);
	void _addChunkToChache(Chunk* chunk);
	void _clearCache();

	//player
	int _getPlayerChunkPosX();
	int _getPlayerChunkPosY();
	int _getPlayerChunkPosZ();

public:
	
	std::vector<Block*> _blocks;

	World();
	World(std::string worldName);
	World(std::string worldName,int seed);

	~World();

	void SetPlayer(Player* player);

	void SetWorldSeed(int seed);
	void SetWorldName(std::string worldName);

	void SetTextureSize(int textureSize,int textureChunkSize);
	void SetChukSize(int width,int height);

	void SetChunkProvider(ChunkProvider* chunkprovider);

	void SetAllChunksDirty();

	int GetBlock(int x,int y,int z);
	Chunk* GetChunkAt(int x,int y,int z);
	Chunk* _blockProperty(int x,int y,int z);

	bool IsBlockTransparent(int x,int y,int z);
	bool IsBlockVisible(int x,int y,int z);
	bool IsBlockSpecial(int x,int y,int z);
	bool IsBlockSingle(int x,int y,int z);
	bool IsBlockCollidable(int x,int y,int z);

	bool CubePick(Vector3 pos,Vector3 dir,float lenght,float step,Vector3 &pickPos);

	void RemoveBlock( int x,int y,int z );
	void AddBlock(int x,int y,int z, unsigned char blockCube);

	void UpdateThread();
	void UpdateWorld(float dt);

	void FetchVisibleChunks();
	void DrawWorld(bool transparent);

	inline int GetVisibleChunksNumber() { return _visibleChunksNumber; }
	
	friend class VertexBuilder;

public:

	World(std::string worldName,int seed,bool testing);
	void TestInit();
	void TestUpdate(float dt);
	void TestDraw(bool transparent);
	Aurora::System::Clock _testClock;

	float TestGenerationTime,TestMeshTime,TestVboTime;
};


#endif
