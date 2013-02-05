#include "BaseBlock.h"
#include "../World/World.h"
#include "../World/Chunk.h"

Block::Block()
{
	_blockName = "Air";
	_blockID = 0;

	_transparent = true;
	_collision = false;
	_lightSource = false;
	_visible = false;
	_editable = false;
	_special = true;
	_single = false;
}

Block::~Block()
{

}

unsigned char Block::GetBlockId()
{
	return _blockID;
}

std::string Block::GetBlockName()
{
	return _blockName;
}

void Block::OnBlockAdded(World *mWorld,int x,int y,int z)
{
	//get neighbors of added block and rebuild it's chunks if they are not the same
	Chunk *baseChunk = mWorld->_blockProperty(x,y,z);

	for (int yy = y-1;yy <= y+1;yy++)
	{
		for (int xx = x-1;xx <= x+1;xx++)
		{
			for (int zz = z-1;zz <= z+1;zz++)
			{
				Chunk *differentChunk = mWorld->_blockProperty(xx,yy,zz);
				if (differentChunk->GetId() != baseChunk->GetId())
				{
					differentChunk->SetDirty(true);
				}
			}
		}
	}
}

void Block::OnBlockRemoved(World *mWorld,int x,int y,int z)
{
	//get neighbors of removed block and rebuild it's chunks if they are not the same
	Chunk *baseChunk = mWorld->_blockProperty(x,y,z);

	for (int yy = y-1;yy <= y+1;yy++)
	{
		for (int xx = x-1;xx <= x+1;xx++)
		{
			for (int zz = z-1;zz <= z+1;zz++)
			{
				Chunk *differentChunk = mWorld->_blockProperty(xx,yy,zz);
				if (differentChunk->GetId() != baseChunk->GetId())
				{
					differentChunk->SetDirty(true);
				}
			}
		}
	}

}

void Block::OnNeighborBlockChanged(World *mWorld,int x,int y,int z)
{

}

void Block::OnBlockClicked(World *mWorld,int x,int y,int z)
{

}