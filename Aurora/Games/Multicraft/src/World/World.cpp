#include "World.h"
#include "WorldConfig.h"
#include "../Utils/MathHelper.h"
#include <Aurora/Math/Ray.h>
#include "../Terrain/TerrainGenerator.h"

#include <algorithm>

World::World()
{
	_initData();

	_worldSeed = 0;
	_worldName = "MultiCraft";
}

World::World(std::string worldName)
{
	_initData();

	_worldSeed = 0;
	_worldName = worldName;
}

World::World(std::string worldName,int seed)
{
	_initData();

	_worldSeed = seed;
	_worldName = worldName;

	TerrainGenerator::Instace()->SetSeed(seed);
}

World::~World()
{
	_visibleChunks.clear();
	_dirtyChunks.clear();
	_updateChunks.clear();
	_neighbors.clear();
	_deleteChunks.clear();

	for(MapHash::const_iterator it = _chunkCache.begin(); it != _chunkCache.end(); it++)
	{
		int chunkId = it->first;
		Chunk* chunk = it->second;

		delete chunk;
	}

	_chunkCache.clear();

	for(unsigned int i = 0; i < _blocks.size();i++)
	{
		delete _blocks[i];
	}

	_blocks.clear();

	delete _player;
}

void World::_initData()
{
	_textureSize = 0;
	_textureChunk = 0;
	_texturePercent = 0.0f;

	_chunkWidth = 0;
	_chunkHeight = 0;
	_lastChunk = 0;

	_visibleChunksNumber = 0;

	//texture pack
	_texturePack = new TexturePack();

	//chunk provider
	_chunkprovider = 0;

	//add blocks
	_blocks.push_back(new Block());
	_blocks.push_back(new BlockStone());
	_blocks.push_back(new BlockGrass());
	_blocks.push_back(new BlockDirt());
	_blocks.push_back(new BlockCobbleStone());
	_blocks.push_back(new BlockPlanks());
	_blocks.push_back(new BlockSapling());
	_blocks.push_back(new BlockBedrock());
	_blocks.push_back(new BlockWaterMoving());
	_blocks.push_back(new BlockWaterStill());
	_blocks.push_back(new BlockLavaMoving());
	_blocks.push_back(new BlockLavaStill());
	_blocks.push_back(new BlockSand());
	_blocks.push_back(new BlockLog());
	_blocks.push_back(new BlockLeaf());
	
	_updateChunksTimer = 0.0f;

#ifndef AURORA_PSP
	_processThread = new Thread(&World::UpdateThread,this);
	_processThread->launch();
#endif
	
}

void World::SetPlayer(Player* player)
{
	_player = player;
	_player->_world = this;
}

void World::SetWorldSeed(int seed)
{
	_worldSeed = seed;
}

void World::SetWorldName(std::string worldName)
{
	_worldName = worldName;
}

void World::SetTextureSize(int textureSize,int textureChunkSize)
{
	_textureSize = textureSize;
	_textureChunk = textureChunkSize;

	_texturePercent = (float)_textureChunk / (float)_textureSize;
}

void World::SetChukSize(int width,int height)
{
	_chunkWidth = width;
	_chunkHeight = height;
}

void World::SetChunkProvider(ChunkProvider* chunkprovider)
{
	_chunkprovider = chunkprovider;
}

void World::SetAllChunksDirty()
{
	for(MapHash::iterator it = _chunkCache.begin();it != _chunkCache.end();)
	{
		Chunk* chunk = it->second;

		if(chunk != 0)
		{
			chunk->ResetVBO();
			chunk->SetDirty(true);
		}
		++it;
	}
}

Chunk* World::GetChunkAt(int x,int y,int z)
{
	return _loadOrPrepareChunk(x,y,z);
}

Chunk* World::_blockProperty(int x,int y,int z)
{
	if (y < 0 || y >= 128) 
		return 0;

	int chunkX = x;
	int chunkY = y;
	int chunkZ = z;

	if(chunkX < 0)
	{
		chunkX = ((chunkX - _chunkWidth + 1) / _chunkWidth) * _chunkWidth;
	}else
	{
		chunkX = (x/_chunkWidth)*_chunkWidth;
	}

	if(chunkZ < 0)
	{
		chunkZ = ((chunkZ - _chunkWidth + 1) /_chunkWidth) * _chunkWidth;
	}else
	{
		chunkZ = (z/_chunkWidth)*_chunkWidth;
	}

	chunkY = (y/_chunkHeight)*_chunkHeight;

	return _loadOrPrepareChunk(chunkX,chunkY,chunkZ);
}

int World::GetBlock(int x,int y,int z)
{
	Chunk* chunk = _blockProperty(x,y,z);

	if(chunk == 0)
		return 0;

	return chunk->GetBlock(x,y,z);
}

bool World::IsBlockTransparent(int x,int y,int z)
{
	Chunk* chunk = _blockProperty(x,y,z);

	if(chunk == 0)
		return false;

	return _blocks[chunk->GetBlock(x,y,z)]->IsTransparent();
}

bool World::IsBlockVisible(int x,int y,int z)
{
	Chunk* chunk = _blockProperty(x,y,z);

	if(chunk == 0)
		return true;

	return _blocks[chunk->GetBlock(x,y,z)]->IsVisible();
}

bool World::IsBlockSpecial(int x,int y,int z)
{
	Chunk* chunk = _blockProperty(x,y,z);

	if(chunk == 0)
		return false;

	return _blocks[chunk->GetBlock(x,y,z)]->IsSpecial();
}

bool World::IsBlockSingle(int x,int y,int z)
{
	Chunk* chunk = _blockProperty(x,y,z);

	if(chunk == 0)
		return false;

	return _blocks[chunk->GetBlock(x,y,z)]->IsSingle();
}

bool World::IsBlockCollidable(int x,int y,int z)
{
	Chunk* chunk = _blockProperty(x,y,z);

	if(chunk == 0)
		return false;

	return _blocks[chunk->GetBlock(x,y,z)]->IsCollidable();
}

bool World::CubePick(Vector3 pos,Vector3 dir,float lenght,float step,Vector3 &pickPos)
{
	Vector3 tempPos;

	dir.normalize();

	for (float i = 0;i <= lenght;i+=step)
	{
		tempPos = pos + (dir * i);

		tempPos.x = tempPos.x < 0 ? tempPos.x-1 : tempPos.x;
		tempPos.y = tempPos.y < 0 ? tempPos.y-1 : tempPos.y;
		tempPos.z = tempPos.z < 0 ? tempPos.z-1 : tempPos.z;

		if (IsBlockCollidable(tempPos.x,tempPos.y,tempPos.z))
		{
 			pickPos.x = (int)tempPos.x + 0.5f;
 			pickPos.y = (int)tempPos.y + 0.5f;
 			pickPos.z = (int)tempPos.z + 0.5f;
			return true;
		}
	}

	return false;
}

void World::UpdateWorld(float dt)
{
	_updateChunksTimer += dt;

	_processChunks();

	#ifdef AURORA_PSP
		UpdateThread();
	#endif
	
	_updateChunksDisplay();
	_clearCache();
}

void World::_processChunks()
{
	_dirtyChunks = _visibleChunks;

	//remove not dirty chunks or processing one
	for(unsigned int i = 0; i < _dirtyChunks.size();i++)
	{
		if(_dirtyChunks[i]->IsProcessing() || !(_dirtyChunks[i]->IsDirty() || _dirtyChunks[i]->IsFresh()))
		{
			_dirtyChunks.erase(_dirtyChunks.begin() + i);
			i--;
		}
	}

	//if there are any dirty chunks we process most closed one to player = at 0 position
	if(_dirtyChunks.size() > 0)
	{

		//update time
		#ifdef AURORA_PSP
		if(_updateChunksTimer > 0.10f)
		{
			_updateChunksTimer = 0.0f;
		#endif
			
			_updateChunk(_dirtyChunks[0]);

		#ifdef AURORA_PSP
		}
		#endif
	}
}

//chunks
void World::_updateChunk(Chunk* chunk)
{
	if(chunk != 0)
	{
		//if it's not dirty chunk or smelling fresh created then return
		if(!chunk->IsDirty() && !chunk->IsFresh())
		{
			return;
		}

		//generate its neighbors
		_createOrGetNeighbors(chunk->GetChunkX(),chunk->GetChunkY(),chunk->GetChunkZ());

		for(unsigned int i = 0;i < _neighbors.size();i++)
		{
			if(_neighbors[i]->IsProcessing())
				return;
		}

		//here pass chunk to second thread that will process this chunk
		std::vector<Chunk*> transportPack;

		chunk->SetProcessing(true);
		transportPack.push_back(chunk);

		for(unsigned int i = 0;i < _neighbors.size();i++)
		{
			_neighbors[i]->SetProcessing(true);
			transportPack.push_back(_neighbors[i]);
		}

		_toProcess.push(transportPack);
	}
}

void World::UpdateThread()
{
	#ifndef AURORA_PSP
	while (true)
	{
	#endif
		if (!_toProcess.empty())
		{
			//get from queue transport pack
			std::vector<Chunk*> transportPack = _toProcess.front();
			_toProcess.pop();

			//what is better place - in theory second thread
			//or maybe place where chunk is created - new instance

			for(unsigned int i = 0;i < transportPack.size();i++)
			{
				if(_chunkprovider != 0)
				{
					if(!_chunkprovider->LoadChunk(transportPack[i]))
					{
						transportPack[i]->Generate();
					}
				}else
				{
					transportPack[i]->Generate();
				}				
			}

			transportPack[0]->BuildMesh();

			//now pack it again
			_fromProcess.push(transportPack);
		}

	#ifndef AURORA_PSP
		Aurora::Threads::sleep(16);
	}
	#endif
}

void World::_updateChunksDisplay()
{
	//check if second thread is giving any chunks to update
	if (!_fromProcess.empty())
	{

		std::vector<Chunk*> transportPack = _fromProcess.front();
		_fromProcess.pop();

		for(unsigned int i = 0;i < transportPack.size();i++)
			transportPack[i]->SetProcessing(false);

		transportPack[0]->BuildVBO();
	}
}

void World::DrawWorld(bool transparent)
{
	_texturePack->SetWorldTexture();
		
	if (!transparent)
	{
		//opaque
		for(int i = 0;i < _visibleChunks.size();i++)
		{
			_visibleChunks[i]->Draw(true);
		}
	}else
	{
		//transparent
		for(int i = _visibleChunks.size()-1;i >=0 ;i--)
		{
			_visibleChunks[i]->Draw(false);
		}
	}	
}

bool _chunkSortFunc(Chunk* d1, Chunk* d2)
{
	return d1->GetDistanceToPlayer() < d2->GetDistanceToPlayer();
}

void World::_divideWordAABB(int minx,int miny,int minz,int maxx,int maxy,int maxz)
{
	int halfX = (maxx - minx) / 2;
	int halfZ = (maxz - minz) / 2;

	_checkWorldAABB(minx			,miny,minz			,minx + halfX	,maxy,minz + halfZ	);
	_checkWorldAABB(minx			,miny,minz + halfZ	,minx + halfX	,maxy,maxz			);
	_checkWorldAABB(minx + halfX	,miny,minz			,maxx			,maxy,minz + halfZ	);
	_checkWorldAABB(minx + halfX	,miny,minz + halfZ	,maxx			,maxy,maxz			);
}

void World::_checkWorldAABB(int minx,int miny,int minz,int maxx,int maxy,int maxz)
{
	BoundingBox worldAABB(minx,miny,minz,maxx,maxy,maxz);

	if(_player->_playerFrustum->BoxInFrustum(worldAABB))
	{
		for (int y = miny; y < maxy; y += _chunkHeight)
		{
			for(int x = minx; x < maxx; x += _chunkWidth)
			{
				for(int z = minz; z < maxz; z += _chunkWidth)
				{
					Chunk *chunk = _loadOrPrepareChunk(x,y,z);

					if(_player->_playerFrustum->BoxInFrustum(chunk->_aabb))
					{
						chunk->_distanceFromPlayer = Vector3::distanceSq(_player->GetPosition(),chunk->_aabb.getCenter());
						_visibleChunks.push_back(chunk);
					}
				}
			}
		}
	}
}

void World::FetchVisibleChunks()
{
	_visibleChunks.clear();

	int minX = (_getPlayerChunkPosX() - WorldConfig::WorldViewX) * _chunkWidth;
	int maxX = (_getPlayerChunkPosX() + WorldConfig::WorldViewX) * _chunkWidth;

	int minY = (_getPlayerChunkPosY() - WorldConfig::WorldViewY) * _chunkHeight;	
	int maxY = (_getPlayerChunkPosY() + WorldConfig::WorldViewY) * _chunkHeight;
	minY = minY < WorldConfig::WorldMinY ? WorldConfig::WorldMinY : minY;
	maxY = maxY > WorldConfig::WorldMaxY ? WorldConfig::WorldMaxY : maxY;

	int minZ = (_getPlayerChunkPosZ() - WorldConfig::WorldViewZ) * _chunkWidth;
	int maxZ = (_getPlayerChunkPosZ() + WorldConfig::WorldViewZ) * _chunkWidth;

	//better fast way
	{
		//generate 4 aabb from world aabbox
		int halfX = (maxX - minX) / 2;
		int halfZ = (maxZ - minZ) / 2;

		_divideWordAABB(minX			,minY,minZ			,minX + halfX	,maxY,minZ + halfZ	);
		_divideWordAABB(minX			,minY,minZ + halfZ	,minX + halfX	,maxY,maxZ			);

		_divideWordAABB(minX + halfX	,minY,minZ			,maxX			,maxY,minZ + halfZ	);
		_divideWordAABB(minX + halfX	,minY,minZ + halfZ	,maxX			,maxY,maxZ			);
	}


	//old slow way
	/*
	{
		for (int y = minY; y < maxY; y += _chunkHeight)
		{
			for(int x = minX; x < maxX ; x += _chunkWidth)
			{
				for(int z = minZ; z < maxZ ; z += _chunkWidth)
				{
					Chunk *chunk = _loadOrPrepareChunk(x,y,z);

					if(_player->_playerFrustum->BoxInFrustum(chunk->_aabb))
					{
						chunk->_distanceFromPlayer = Vector3::distanceSq(_player->GetPosition(),chunk->_aabb.getCenter());
						_visibleChunks.push_back(chunk);
					}
				}
			}
		}
	}*/

	_visibleChunksNumber = _visibleChunks.size();

	//sort them
	std::sort(_visibleChunks.begin(),_visibleChunks.end(),_chunkSortFunc);
}



void World::_createOrGetNeighbors(int x,int y,int z)
{
	_neighbors.clear();

	Chunk* chunk;

	//try this
	chunk = _loadOrPrepareChunk(x + _chunkWidth	,y,z);
	_neighbors.push_back(chunk);
	chunk = _loadOrPrepareChunk(x - _chunkWidth	,y,z);
	_neighbors.push_back(chunk);
	chunk = _loadOrPrepareChunk(x				,y,z + _chunkWidth);
	_neighbors.push_back(chunk);
	chunk = _loadOrPrepareChunk(x				,y,z - _chunkWidth);
	_neighbors.push_back(chunk);


	//y + _chunkHeight
	if (y < WorldConfig::WorldMaxY)
	{
		chunk = _loadOrPrepareChunk(x ,y + _chunkHeight,z);
		_neighbors.push_back(chunk);
	}

	//y - _chunkHeight
	if (y > WorldConfig::WorldMinY)
	{
		chunk = _loadOrPrepareChunk(x ,y - _chunkHeight,z);
		_neighbors.push_back(chunk);
	}
}

Chunk* World::_loadOrPrepareChunk(int x,int y,int z)
{
	Chunk* chunk = 0;

	chunk = _getChunkFromCache(MathHelper::Cantorize(x,y,z));

	if (chunk != NULL)
	{
		if (chunk->GetChunkX() != x || chunk->GetChunkY() != y || chunk->GetChunkZ() != z)
		{
			//this should not happened!!!
			x = x;
		}

		return chunk;
	}

	chunk = new Chunk(this,x,y,z,_chunkWidth,_chunkHeight);
	chunk->_chunkId = MathHelper::Cantorize(x,y,z);

	_addChunkToChache(chunk);
	chunk->SetCached(true);

	return chunk;
}

//cache

Chunk* World::_getChunkFromCache(int id)
{
	MapHash::const_iterator it = _chunkCache.find(id);
	if (it != _chunkCache.end())
	{
		return it->second;
		//return _chunkCache[id];
	}else
	{
		return NULL;
	}	
}

void World::_addChunkToChache(Chunk* chunk)
{
	_chunkCache.insert(std::pair<int,Chunk*>(chunk->GetId(),chunk));
	//_chunkCache[chunk->GetId()] = chunk;
}

void World::_clearCache()
{
	int size = _chunkCache.size();

	//if(size < WorldConfig::ChunkCacheCapacity)
	//	return;

	int minX = (_getPlayerChunkPosX() - WorldConfig::ChunkCacheView) * _chunkWidth;
	int maxX = (_getPlayerChunkPosX() + WorldConfig::ChunkCacheView) * _chunkWidth;

	int minY = (_getPlayerChunkPosY() - WorldConfig::ChunkCacheView) * _chunkHeight;	
	int maxY = (_getPlayerChunkPosY() + WorldConfig::ChunkCacheView) * _chunkHeight;
	minY = minY < WorldConfig::WorldMinY ? WorldConfig::WorldMinY : minY;
	maxY = maxY > WorldConfig::WorldMaxY ? WorldConfig::WorldMaxY : maxY;

	int minZ = (_getPlayerChunkPosZ() - WorldConfig::ChunkCacheView) * _chunkWidth;
	int maxZ = (_getPlayerChunkPosZ() + WorldConfig::ChunkCacheView) * _chunkWidth;

	BoundingBox worldBox = BoundingBox(Vector3(minX,minY,minZ),Vector3(maxX,maxY,maxZ));

	for(MapHash::iterator it = _chunkCache.begin();it != _chunkCache.end();)
	{
		int chunkId = it->first;
		Chunk* chunk = it->second;

		if(chunk == 0)
		{
			_chunkCache.erase(it++);
		}else
		{
			//don't remove if chunks is in aabb or is in processing mode
			if (worldBox.intersect(chunk->_aabb) || chunk->IsProcessing())
			{
				++it;
			}else
			{
				//if need to save then save
				if(chunk->IsSaveNeed())
				{
					if(_chunkprovider != 0)
					{
						_chunkprovider->SaveChunk(chunk);
					}
				}


				//delete and remove
				delete _chunkCache[chunkId];
				_chunkCache[chunkId] = 0;
				_chunkCache.erase(it++);
			}
		}	
	}
}

int World::_getPlayerChunkPosX()
{
	return _player->GetPosition().x / _chunkWidth;
}

int World::_getPlayerChunkPosY()
{
	return _player->GetPosition().y / _chunkHeight;
}

int World::_getPlayerChunkPosZ()
{
	return _player->GetPosition().z / _chunkWidth;
}

void World::AddBlock(int x,int y,int z, unsigned char blockCube)
{
	//we need to get normal from pointed block
	Vector3 minBox(x,y,z);
	Vector3 maxBox(x + 1,y + 1,z + 1);

	BoundingBox testBox(minBox,maxBox);
	Vector3 direction = Vector3::normalized(_player->_playerCamera->GetViewDirection());
	Ray testray(_player->_playerCamera->m_vPosition,direction);

	float distance = 999.0f;
	testray.hasIntersected(testBox,distance);
	if (distance != -1.0f)
	{
		Vector3 hitPos = _player->_playerCamera->m_vPosition + distance * direction;

		Vector3 dirFromCenterTohit = hitPos - testBox.getCenter();

		dirFromCenterTohit.normalize();

		if (fabsf(dirFromCenterTohit.x) > fabsf(dirFromCenterTohit.y) && fabsf(dirFromCenterTohit.x) > fabsf(dirFromCenterTohit.z))
		{
			dirFromCenterTohit.set(dirFromCenterTohit.x < 0.0f ? -1:1,0,0);
		}else if (fabsf(dirFromCenterTohit.y) > fabsf(dirFromCenterTohit.x) && fabsf(dirFromCenterTohit.y) > fabsf(dirFromCenterTohit.z))
		{
			dirFromCenterTohit.set(0,dirFromCenterTohit.y < 0.0f ? -1:1,0);
		}else
		{
			dirFromCenterTohit.set(0,0,dirFromCenterTohit.z < 0.0f ? -1:1);
		}

		minBox += dirFromCenterTohit;

		//now check if it's not colliding with player aabb
		maxBox.set(minBox.x + 1,minBox.y + 1,minBox.z + 1);

		testBox.min = minBox;
		testBox.max = maxBox;
		testBox.Validity = BoundingBox::Valid;

		if (!testBox.intersect(_player->_aabb) && !IsBlockCollidable(minBox.x,minBox.y,minBox.z))
		{
			Chunk* chunk = _blockProperty(minBox.x,minBox.y,minBox.z);

			if (chunk == NULL)
				return;

			chunk->SetBlock(minBox.x,minBox.y,minBox.z,blockCube);
			chunk->SetSaveState(true);

			_blocks[Blocks::Air]->OnBlockAdded(this,minBox.x,minBox.y,minBox.z);

			chunk->SetDirty(true);
		}		
	}
}

void World::RemoveBlock( int x,int y,int z )
{
	Chunk* chunk = _blockProperty(x,y,z);

	if (chunk == NULL)
		return;
	
	chunk->SetBlock(x,y,z,Blocks::Air);
	chunk->SetSaveState(true);

	_blocks[Blocks::Air]->OnBlockRemoved(this,x,y,z);

	chunk->SetDirty(true);
}


//////////////////////////////////////////////////////////////////////////
//testing part
//////////////////////////////////////////////////////////////////////////

World::World(std::string worldName,int seed,bool testing)
{
	_worldSeed = seed;
	_worldName = worldName;

	TerrainGenerator::Instace()->SetSeed(seed);
}

void World::TestInit()
{
	//_textureSize = 0;
	//_textureChunk = 0;
	//_texturePercent = 0.0f;

	//_chunkWidth = 0;
	//_chunkHeight = 0;
	_lastChunk = 0;

	_visibleChunksNumber = 0;

	//texture pack
	_texturePack = new TexturePack();

	//add blocks
	_blocks.push_back(new Block());
	_blocks.push_back(new BlockStone());
	_blocks.push_back(new BlockGrass());
	_blocks.push_back(new BlockDirt());
	_blocks.push_back(new BlockCobbleStone());
	_blocks.push_back(new BlockPlanks());
	_blocks.push_back(new BlockSapling());
	_blocks.push_back(new BlockBedrock());
	_blocks.push_back(new BlockWaterMoving());
	_blocks.push_back(new BlockWaterStill());
	_blocks.push_back(new BlockLavaMoving());
	_blocks.push_back(new BlockLavaStill());
	_blocks.push_back(new BlockSand());
	_blocks.push_back(new BlockLog());
	_blocks.push_back(new BlockLeaf());

	_updateChunksTimer = 0.0f;

	//get all chunks from player aabb
	_visibleChunks.clear();

	int minX = (_getPlayerChunkPosX() - WorldConfig::ChunkCacheView) * _chunkWidth;
	int maxX = (_getPlayerChunkPosX() + WorldConfig::ChunkCacheView) * _chunkWidth;

	int minY = (_getPlayerChunkPosY() - WorldConfig::WorldViewY - 1) * _chunkHeight;	
	int maxY = (_getPlayerChunkPosY() + WorldConfig::WorldViewY + 1) * _chunkHeight;
	minY = minY < WorldConfig::WorldMinY ? WorldConfig::WorldMinY : minY;
	maxY = maxY > WorldConfig::WorldMaxY ? WorldConfig::WorldMaxY : maxY;

	int minZ = (_getPlayerChunkPosZ() - WorldConfig::ChunkCacheView) * _chunkWidth;
	int maxZ = (_getPlayerChunkPosZ() + WorldConfig::ChunkCacheView) * _chunkWidth;

	for (int y = minY; y <= maxY; y += _chunkHeight)
	{
		for(int x = minX; x <= maxX ; x += _chunkWidth)
		{
			for(int z = minZ; z <= maxZ ; z += _chunkWidth)
			{
				Chunk *chunk = _loadOrPrepareChunk(x,y,z);

				chunk->_distanceFromPlayer = Vector3::distanceSq(_player->GetPosition(),chunk->_aabb.getCenter());
				_visibleChunks.push_back(chunk);
			}
		}
	}

	_visibleChunksNumber = _visibleChunks.size();
}

void World::TestUpdate(float dt)
{
	TestGenerationTime = TestMeshTime = TestVboTime = 0.0f;
	float tempTime = 0.0f;

	int visibleChunksSize = _visibleChunks.size();

	//generate terrain for them
	_testClock.Reset();
	for (int i = 0;i < visibleChunksSize;i++)
	{
		_visibleChunks[i]->Generate();
	}
	TestGenerationTime = _testClock.getTime();

	//get right now
	_visibleChunks.clear();

	int minX = (_getPlayerChunkPosX() - WorldConfig::WorldViewX) * _chunkWidth;
	int maxX = (_getPlayerChunkPosX() + WorldConfig::WorldViewX) * _chunkWidth;

	int minY = (_getPlayerChunkPosY() - WorldConfig::WorldViewY) * _chunkHeight;	
	int maxY = (_getPlayerChunkPosY() + WorldConfig::WorldViewY) * _chunkHeight;
	minY = minY < WorldConfig::WorldMinY ? WorldConfig::WorldMinY : minY;
	maxY = maxY > WorldConfig::WorldMaxY ? WorldConfig::WorldMaxY : maxY;

	int minZ = (_getPlayerChunkPosZ() - WorldConfig::WorldViewZ) * _chunkWidth;
	int maxZ = (_getPlayerChunkPosZ() + WorldConfig::WorldViewZ) * _chunkWidth;

	for (int y = minY; y <= maxY; y += _chunkHeight)
	{
		for(int x = minX; x <= maxX ; x += _chunkWidth)
		{
			for(int z = minZ; z <= maxZ ; z += _chunkWidth)
			{
				Chunk *chunk = _loadOrPrepareChunk(x,y,z);

				chunk->_distanceFromPlayer = Vector3::distanceSq(_player->GetPosition(),chunk->_aabb.getCenter());
				_visibleChunks.push_back(chunk);
			}
		}
	}

	_visibleChunksNumber = _visibleChunks.size();
	visibleChunksSize = _visibleChunks.size();

	//build mesh
	_testClock.Reset();
	for (int i = 0;i < visibleChunksSize;i++)
	{
		_visibleChunks[i]->BuildMesh();
	}
	TestMeshTime = _testClock.getTime();

	//generate vbo
	_testClock.Reset();
	for (int i = 0;i < visibleChunksSize;i++)
	{
		_visibleChunks[i]->BuildVBO();
	}
	TestVboTime = _testClock.getTime();
}

void World::TestDraw(bool transparent)
{
	_texturePack->SetWorldTexture();

	//opaque
	if (!transparent)
	{
		for(int i = 0;i < _visibleChunks.size();i++)
		{
			_visibleChunks[i]->Draw(true);
		}
	}
	

	//transparent
	if (transparent)
	{
		for(int i = _visibleChunks.size()-1;i >=0 ;i--)
		{
			_visibleChunks[i]->Draw(false);
		}
	}
}