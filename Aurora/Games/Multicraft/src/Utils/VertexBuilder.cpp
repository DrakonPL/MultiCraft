#include "VertexBuilder.h"
#include "../World/World.h"

template <typename T>
void FreeAll( T & t ) {
	T tmp;
	t.swap( tmp );
}


VertexBuilder* VertexBuilder::_instance = 0;

VertexBuilder* VertexBuilder::Instance()
{
	if (_instance == 0)
	{
		_instance = new VertexBuilder();
	}

	return _instance;
}

VertexBuilder::VertexBuilder()
{

}

void VertexBuilder::triangle(int x,int y,int z)
{
	mTriangle.push_back(x);
	mTriangle.push_back(y);
	mTriangle.push_back(z);
}

void VertexBuilder::_cleanBuffers()
{
	FreeAll(mVertices);
	FreeAll(mTriangle);

	//mVertices.clear();
	//mTriangle.clear();
}

float VertexBuilder::_boundDown(float number)
{
	if(number != 0.0f)
	{
		number = number - 0.01f * 0.0625f;
	}

	return number;
}

float VertexBuilder::_boundUp(float number)
{
	if(number != 0.0f)
	{
		number = number + 0.01f * 0.0625f;
	}

	return number;
}

void VertexBuilder::vert(float x,float y,float z,float u,float v,float r,float g,float b)
{
	MultiVertex vertex;

	vertex.u = u;
	vertex.v = v;
	vertex.r = r;
	vertex.g = g;
	vertex.b = b;
	vertex.a = 1.0f;
	vertex.x = x;
	vertex.y = y;
	vertex.z = z;

	mVertices.push_back(vertex);
}

void VertexBuilder::BuildChunkVertexBuffer(Chunk *chunk,bool transparent)
{
	int indicesCount = 0;
	int vertexCount = 0;

	MultiVertex* meshVertices;
	unsigned short* indices;

	if(mTriangle.size() > 0)
	{
		//vertices
		meshVertices = new MultiVertex[mVertices.size()];
		MultiVertex* verTemp = &mVertices[0];

		memcpy(meshVertices,verTemp,sizeof(MultiVertex)*mVertices.size());

		//indices
		indices = new unsigned short[mTriangle.size()];
		unsigned short* indTemp = &mTriangle[0];

		memcpy(indices,indTemp,sizeof(unsigned short)*mTriangle.size());

		//indices
		indicesCount = mTriangle.size();
		vertexCount =  mVertices.size();

		//optimize here
		if(transparent)
		{
			chunk->_transparentVBO.Generate(meshVertices,indices,vertexCount,indicesCount,true);	
		}else
		{
			chunk->_opaqueVBO.Generate(meshVertices,indices,vertexCount,indicesCount,true);	
		}

		_cleanBuffers();		
	}
}

void VertexBuilder::BuildTransparentChunk(Chunk* chunk)
{
	World *world = chunk->_world;

	unsigned int block = 0;

	int iVertex = 0;

	bool transparentBlock;
	bool visibleBlock = false;
	bool DefaultBlock = false;
	bool singleBlock = false;

	int SX = 0;
	int SY = 0;
	int SZ = 0;

	//get neighboor chunks before entering this mega loop
	Chunk* chunkleft = world->GetChunkAt(chunk->GetChunkX() - chunk->GetChunkWidth(),chunk->GetChunkY(),chunk->GetChunkZ());
	Chunk* chunkright = world->GetChunkAt(chunk->GetChunkX() + chunk->GetChunkWidth(),chunk->GetChunkY(),chunk->GetChunkZ());

	Chunk* chunkfront = world->GetChunkAt(chunk->GetChunkX(),chunk->GetChunkY(),chunk->GetChunkZ() + chunk->GetChunkWidth());
	Chunk* chunkback = world->GetChunkAt(chunk->GetChunkX(),chunk->GetChunkY(),chunk->GetChunkZ() - chunk->GetChunkWidth());

	Chunk* chunkup = world->GetChunkAt(chunk->GetChunkX(),chunk->GetChunkY() + chunk->GetChunkHeight(),chunk->GetChunkZ() );
	Chunk* chunkdown = world->GetChunkAt(chunk->GetChunkX(),chunk->GetChunkY() - chunk->GetChunkHeight(),chunk->GetChunkZ());

	float colMulR, colMulG, colMulB;
	float down, up, left, right;

	//go go go
	for (int y = chunk->_chunkY; y < chunk->_chunkY + chunk->_chunkHeight; y++)
	{
		for (int x = chunk->_chunkX; x < chunk->_chunkX + chunk->_chunkWidth; x++)
		{
			for (int z = chunk->_chunkZ; z < chunk->_chunkZ + chunk->_chunkWidth; z++)
			{
				block = chunk->GetBlock(x,y,z);
				Block *blockType = world->_blocks[block];
				singleBlock = blockType->IsSingle();

				if (block == Blocks::Air)
				{
					continue;
				}

				if(!blockType->IsTransparent() && !blockType->IsSpecial())continue;//if block is transparent but not special don't continue

				if(!blockType->IsSpecial())
				{
					//faces
					//x-1
					transparentBlock = DefaultBlock;
					visibleBlock = true;

					if (x-1 < chunk->_chunkX)
					{
						transparentBlock = chunkleft->IsBlockVisible(x-1,y,z);
						visibleBlock = chunkleft->IsBlockSpecial(x-1,y,z);
					}else
					{
						transparentBlock = chunk->IsBlockVisible(x-1,y,z);
						visibleBlock = chunk->IsBlockSpecial(x-1,y,z);
					}

					if (!transparentBlock || visibleBlock || singleBlock)
					{
						down = _boundDown(world->_texturePercent * (blockType->_sidePlane.y + 1.0f));
						up = _boundUp(down - world->_texturePercent);

						left = world->_texturePercent * blockType->_sidePlane.x;
						right = _boundDown(left + world->_texturePercent);

						vert(x, y,   z+1,right, down,0.9f,0.9f,0.9f);
						vert(x, y+1, z+1,right, up,0.9f,0.9f,0.9f);
						vert(x, y+1, z,left, up,0.9f,0.9f,0.9f);
						vert(x, y,   z,left, down,0.9f,0.9f,0.9f);

						triangle(iVertex, iVertex+1, iVertex+2);
						triangle(iVertex+2, iVertex+3, iVertex);

						iVertex += 4;
					}


					//x+1
					transparentBlock = DefaultBlock;
					visibleBlock = true;

					if(x + 1 >= chunk->_chunkX + chunk->_chunkWidth)
					{
						transparentBlock = chunkright->IsBlockVisible(x+1,y,z);
						visibleBlock = chunkright->IsBlockSpecial(x+1,y,z);
					}else
					{
						transparentBlock = chunk->IsBlockVisible(x+1,y,z);
						visibleBlock = chunk->IsBlockSpecial(x+1,y,z);
					}

					if (!transparentBlock || visibleBlock || singleBlock)
					{
						down = _boundDown(world->_texturePercent * (blockType->_sidePlane.y + 1.0f));
						up = _boundUp(down - world->_texturePercent);

						left = world->_texturePercent * blockType->_sidePlane.x;
						right = _boundDown(left + world->_texturePercent);

						vert(x+1, y,   z,right, down,0.65f,0.65f,0.65f);
						vert(x+1, y+1, z,right, up,0.65f,0.65f,0.65f);
						vert(x+1, y+1, z+1,left, up,0.65f,0.65f,0.65f);
						vert(x+1, y,   z+1,left, down,0.65f,0.65f,0.65f);

						triangle(iVertex, iVertex+1, iVertex+2);
						triangle(iVertex+2, iVertex+3, iVertex);

						iVertex += 4;
					}

					//y-1
					transparentBlock = DefaultBlock;
					visibleBlock = true;

					if (y-1 < chunk->_chunkY)
					{
						transparentBlock = chunkdown->IsBlockVisible(x,y-1,z);
						visibleBlock = chunkdown->IsBlockSpecial(x,y-1,z);
					}else
					{
						transparentBlock = chunk->IsBlockVisible(x,y-1,z);
						visibleBlock = chunk->IsBlockSpecial(x,y-1,z);
					}

					if (!transparentBlock || visibleBlock || singleBlock)
					{
						//up
						down = world->_texturePercent * (blockType->_downPlane.y + 1.0f);
						up = down - world->_texturePercent;

						left = world->_texturePercent * blockType->_downPlane.x;
						right = left + world->_texturePercent;

						vert(x,   y, z,left, up,0.55f,0.55f,0.55f);
						vert(x+1, y, z,right, up,0.55f,0.55f,0.55f);
						vert(x+1, y, z+1,right, down,0.55f,0.55f,0.55f);
						vert(x,   y, z+1,left, down,0.55f,0.55f,0.55f);

						triangle(iVertex, iVertex+1, iVertex+2);
						triangle(iVertex+2, iVertex+3, iVertex);

						iVertex += 4;
					}


					//top face
					//y+1
					transparentBlock = DefaultBlock;
					visibleBlock = true;

					if (y+1 >= chunk->_chunkY + chunk->_chunkHeight)
					{
						transparentBlock = chunkup->IsBlockVisible(x,y+1,z);
						visibleBlock = chunkup->IsBlockSpecial(x,y+1,z);
					}
					else
					{
						transparentBlock = chunk->IsBlockVisible(x,y+1,z);
						visibleBlock = chunk->IsBlockSpecial(x,y+1,z);
					}

					if (!transparentBlock || visibleBlock || singleBlock)
					{
						//down
						down = _boundDown(world->_texturePercent * (blockType->_upPlane.y + 1.0f));
						up = _boundUp(down - world->_texturePercent);

						left = world->_texturePercent * blockType->_upPlane.x;
						right = _boundDown(left + world->_texturePercent);

						vert(x,   y+1, z+1,left, up,1.0f,1.0f,1.0f);
						vert(x+1, y+1, z+1,right, up,1.0f,1.0f,1.0f);
						vert(x+1, y+1, z,right, down,1.0f,1.0f,1.0f);
						vert(x,   y+1, z,left, down,1.0f,1.0f,1.0f);

						triangle(iVertex, iVertex+1, iVertex+2);
						triangle(iVertex+2, iVertex+3, iVertex);

						iVertex += 4;
					}

					//z-1
					transparentBlock = DefaultBlock;
					visibleBlock = true;

					if (z-1 < chunk->_chunkZ)
					{
						transparentBlock = chunkback->IsBlockVisible(x,y,z-1);
						visibleBlock = chunkback->IsBlockSpecial(x,y,z-1);
					}
					else
					{
						transparentBlock = chunk->IsBlockVisible(x,y,z-1);
						visibleBlock = chunk->IsBlockSpecial(x,y,z-1);
					}

					if (!transparentBlock || visibleBlock || singleBlock)
					{
						down = _boundDown(world->_texturePercent * (blockType->_sidePlane.y + 1.0f));
						up = _boundUp(down - world->_texturePercent);

						left = world->_texturePercent * blockType->_sidePlane.x;
						right = _boundDown(left + world->_texturePercent);

						vert(x,   y+1, z,right, up,0.8f,0.8f,0.8f);
						vert(x+1, y+1, z,left, up,0.8f,0.8f,0.8f);
						vert(x+1, y,   z,left, down,0.8f,0.8f,0.8f);
						vert(x,   y,   z,right, down,0.8f,0.8f,0.8f);

						triangle(iVertex, iVertex+1, iVertex+2);
						triangle(iVertex+2, iVertex+3, iVertex);

						iVertex += 4;
					}


					//z+1
					transparentBlock = DefaultBlock;
					visibleBlock = true;

					if (z+1 >= chunk->_chunkZ + chunk->_chunkWidth)
					{
						transparentBlock = chunkfront->IsBlockVisible(x,y,z+1);
						visibleBlock = chunkfront->IsBlockSpecial(x,y,z+1);
					}else
					{
						transparentBlock = chunk->IsBlockVisible(x,y,z+1);
						visibleBlock = chunk->IsBlockSpecial(x,y,z+1);
					}

					if (!transparentBlock || visibleBlock || singleBlock)
					{
						down = _boundDown(world->_texturePercent * (blockType->_sidePlane.y + 1.0f));
						up = _boundUp(down - world->_texturePercent);

						left = world->_texturePercent * blockType->_sidePlane.x;
						right = _boundDown(left + world->_texturePercent);

						vert(x,   y,   z+1,left, down,0.75f,0.75f,0.75f);
						vert(x+1, y,   z+1,right, down,0.75f,0.75f,0.75f);
						vert(x+1, y+1, z+1,right, up,0.75f,0.75f,0.75f);
						vert(x,   y+1, z+1,left, up,0.75f,0.75f,0.75f);

						triangle(iVertex, iVertex+1, iVertex+2);
						triangle(iVertex+2, iVertex+3, iVertex);

						iVertex += 4;
					}
				}
			}
		}
	}

	//done
	if(iVertex > 0)
	{
		BuildChunkVertexBuffer(chunk,true);
	}
}

void VertexBuilder::BuildChunk(Chunk* chunk)
{
	World *world = chunk->_world;

	unsigned int block = 0;
	
	int iVertex = 0;
	
	bool transparentBlock;
	bool specialBlock = false;
	bool DefaultBlock = false;

	int SX = 0;
	int SY = 0;
	int SZ = 0;

	//get neighboor chunks before entering this mega loop
	Chunk* chunkleft = world->GetChunkAt(chunk->GetChunkX() - chunk->GetChunkWidth(),chunk->GetChunkY(),chunk->GetChunkZ());
	if (chunkleft->GetChunkX() != chunk->GetChunkX() - chunk->GetChunkWidth() || chunkleft->GetChunkY() != chunk->GetChunkY() || chunkleft->GetChunkZ() != chunk->GetChunkZ())
	{
		SX = SX;
	}
	
	Chunk* chunkright = world->GetChunkAt(chunk->GetChunkX() + chunk->GetChunkWidth(),chunk->GetChunkY(),chunk->GetChunkZ());
	if (chunkright->GetChunkX() != chunk->GetChunkX() + chunk->GetChunkWidth() || chunkright->GetChunkY() != chunk->GetChunkY() || chunkright->GetChunkZ() != chunk->GetChunkZ())
	{
		SX = SX;
	}

	Chunk* chunkfront = world->GetChunkAt(chunk->GetChunkX(),chunk->GetChunkY(),chunk->GetChunkZ() + chunk->GetChunkWidth());
	if (chunkfront->GetChunkX() != chunk->GetChunkX() || chunkfront->GetChunkY() != chunk->GetChunkY() || chunkfront->GetChunkZ() != chunk->GetChunkZ() + chunk->GetChunkWidth())
	{
		SX = SX;
	}
	
	Chunk* chunkback = world->GetChunkAt(chunk->GetChunkX(),chunk->GetChunkY(),chunk->GetChunkZ() - chunk->GetChunkWidth());
	if (chunkback->GetChunkX() != chunk->GetChunkX() || chunkback->GetChunkY() != chunk->GetChunkY() || chunkback->GetChunkZ() != chunk->GetChunkZ() - chunk->GetChunkWidth())
	{
		SX = SX;
	}

	Chunk* chunkup = world->GetChunkAt(chunk->GetChunkX(),chunk->GetChunkY() + chunk->GetChunkHeight(),chunk->GetChunkZ() );
	if (chunkup->GetChunkX() != chunk->GetChunkX() || chunkup->GetChunkY() != chunk->GetChunkY() + chunk->GetChunkHeight()|| chunkup->GetChunkZ() != chunk->GetChunkZ())
	{
		SX = SX;
	}
	
	Chunk* chunkdown = world->GetChunkAt(chunk->GetChunkX(),chunk->GetChunkY() - chunk->GetChunkHeight(),chunk->GetChunkZ());
	if (chunkdown->GetChunkX() != chunk->GetChunkX() || chunkdown->GetChunkY() != chunk->GetChunkY() - chunk->GetChunkHeight()|| chunkdown->GetChunkZ() != chunk->GetChunkZ())
	{
		SX = SX;
	}


	float colMulR, colMulG, colMulB;
	float down, up, left, right;

	//go go go
	for (int y = chunk->_chunkY; y < chunk->_chunkY + chunk->_chunkHeight; y++)
	{
		for (int x = chunk->_chunkX; x < chunk->_chunkX + chunk->_chunkWidth; x++)
		{
			for (int z = chunk->_chunkZ; z < chunk->_chunkZ + chunk->_chunkWidth; z++)
			{
				block = chunk->GetBlock(x,y,z);
				Block *blockType = world->_blocks[block];

				if(blockType->IsTransparent() && !blockType->IsSpecial())continue;//if block is transparent but not special don't continue

				if(!blockType->IsSpecial())
				{
					//faces
					//x-1
					transparentBlock = DefaultBlock;
					
					if (x-1 < chunk->_chunkX)
					{
						transparentBlock = chunkleft->IsBlockTransparent(x-1,y,z);
						specialBlock = chunkleft->IsBlockSpecial(x-1,y,z);
					}else
					{
						transparentBlock = chunk->IsBlockTransparent(x-1,y,z);
						specialBlock = chunk->IsBlockSpecial(x-1,y,z);
					}

					if (transparentBlock == true || specialBlock == true)
					{
						down = _boundDown(world->_texturePercent * (blockType->_sidePlane.y + 1.0f));
						up = _boundUp(down - world->_texturePercent);

						left = world->_texturePercent * blockType->_sidePlane.x;
						right = _boundDown(left + world->_texturePercent);

						vert(x, y,   z+1,right, down,0.9f,0.9f,0.9f);
						vert(x, y+1, z+1,right, up,0.9f,0.9f,0.9f);
						vert(x, y+1, z,left, up,0.9f,0.9f,0.9f);
						vert(x, y,   z,left, down,0.9f,0.9f,0.9f);

						triangle(iVertex, iVertex+1, iVertex+2);
						triangle(iVertex+2, iVertex+3, iVertex);

						iVertex += 4;
					}


					//x+1
					transparentBlock = DefaultBlock;
					if(x + 1 >= chunk->_chunkX + chunk->_chunkWidth)
					{
						transparentBlock = chunkright->IsBlockTransparent(x+1,y,z);
						specialBlock = chunkright->IsBlockSpecial(x+1,y,z);
					}else
					{
						transparentBlock = chunk->IsBlockTransparent(x+1,y,z);
						specialBlock = chunk->IsBlockSpecial(x+1,y,z);
					}

					if (transparentBlock == true || specialBlock == true)
					{
						down = _boundDown(world->_texturePercent * (blockType->_sidePlane.y + 1.0f));
						up = _boundUp(down - world->_texturePercent);

						left = world->_texturePercent * blockType->_sidePlane.x;
						right = _boundDown(left + world->_texturePercent);

						vert(x+1, y,   z,right, down,0.65f,0.65f,0.65f);
						vert(x+1, y+1, z,right, up,0.65f,0.65f,0.65f);
						vert(x+1, y+1, z+1,left, up,0.65f,0.65f,0.65f);
						vert(x+1, y,   z+1,left, down,0.65f,0.65f,0.65f);

						triangle(iVertex, iVertex+1, iVertex+2);
						triangle(iVertex+2, iVertex+3, iVertex);

						iVertex += 4;
					}

					//y-1
					transparentBlock = DefaultBlock;					
					if (y-1 < chunk->_chunkY)
					{
						transparentBlock = chunkdown->IsBlockTransparent(x,y-1,z);
						specialBlock = chunkdown->IsBlockSpecial(x,y-1,z);
					}else
					{
						transparentBlock = chunk->IsBlockTransparent(x,y-1,z);
						specialBlock = chunk->IsBlockSpecial(x,y-1,z);
					}

					if (transparentBlock == true || specialBlock == true)
					{
						//up
						down = world->_texturePercent * (blockType->_downPlane.y + 1.0f);
						up = down - world->_texturePercent;

						left = world->_texturePercent * blockType->_downPlane.x;
						right = left + world->_texturePercent;

						vert(x,   y, z,left, up,0.55f,0.55f,0.55f);
						vert(x+1, y, z,right, up,0.55f,0.55f,0.55f);
						vert(x+1, y, z+1,right, down,0.55f,0.55f,0.55f);
						vert(x,   y, z+1,left, down,0.55f,0.55f,0.55f);

						triangle(iVertex, iVertex+1, iVertex+2);
						triangle(iVertex+2, iVertex+3, iVertex);

						iVertex += 4;
					}


					//top face
					//y+1
					transparentBlock = DefaultBlock;
					if (y+1 >= chunk->_chunkY + chunk->_chunkHeight)
					{
						transparentBlock = chunkup->IsBlockTransparent(x,y+1,z);
						specialBlock = chunkup->IsBlockSpecial(x,y+1,z);
					}
					else
					{
						transparentBlock = chunk->IsBlockTransparent(x,y+1,z);
						specialBlock = chunk->IsBlockSpecial(x,y+1,z);
					}

					if (transparentBlock == true || specialBlock == true)
					{
						//down
						down = _boundDown(world->_texturePercent * (blockType->_upPlane.y + 1.0f));
						up = _boundUp(down - world->_texturePercent);

						left = world->_texturePercent * blockType->_upPlane.x;
						right = _boundDown(left + world->_texturePercent);

						vert(x,   y+1, z+1,left, up,1.0f,1.0f,1.0f);
						vert(x+1, y+1, z+1,right, up,1.0f,1.0f,1.0f);
						vert(x+1, y+1, z,right, down,1.0f,1.0f,1.0f);
						vert(x,   y+1, z,left, down,1.0f,1.0f,1.0f);

						triangle(iVertex, iVertex+1, iVertex+2);
						triangle(iVertex+2, iVertex+3, iVertex);

						iVertex += 4;
					}

					//z-1
					transparentBlock = DefaultBlock;
					if (z-1 < chunk->_chunkZ)
					{
						transparentBlock = chunkback->IsBlockTransparent(x,y,z-1);
						specialBlock = chunkback->IsBlockSpecial(x,y,z-1);
					}
					else
					{
						transparentBlock = chunk->IsBlockTransparent(x,y,z-1);
						specialBlock = chunk->IsBlockSpecial(x,y,z-1);
					}

					if (transparentBlock == true || specialBlock == true)
					{
						down = _boundDown(world->_texturePercent * (blockType->_sidePlane.y + 1.0f));
						up = _boundUp(down - world->_texturePercent);

						left = world->_texturePercent * blockType->_sidePlane.x;
						right = _boundDown(left + world->_texturePercent);

						vert(x,   y+1, z,right, up,0.8f,0.8f,0.8f);
						vert(x+1, y+1, z,left, up,0.8f,0.8f,0.8f);
						vert(x+1, y,   z,left, down,0.8f,0.8f,0.8f);
						vert(x,   y,   z,right, down,0.8f,0.8f,0.8f);

						triangle(iVertex, iVertex+1, iVertex+2);
						triangle(iVertex+2, iVertex+3, iVertex);

						iVertex += 4;
					}


					//z+1
					transparentBlock = DefaultBlock;
					if (z+1 >= chunk->_chunkZ + chunk->_chunkWidth)
					{
						transparentBlock = chunkfront->IsBlockTransparent(x,y,z+1);
						specialBlock = chunkfront->IsBlockSpecial(x,y,z+1);
					}else
					{
						transparentBlock = chunk->IsBlockTransparent(x,y,z+1);
						specialBlock = chunk->IsBlockSpecial(x,y,z+1);
					}

					if (transparentBlock == true || specialBlock == true)
					{
						down = _boundDown(world->_texturePercent * (blockType->_sidePlane.y + 1.0f));
						up = _boundUp(down - world->_texturePercent);

						left = world->_texturePercent * blockType->_sidePlane.x;
						right = _boundDown(left + world->_texturePercent);

						vert(x,   y,   z+1,left, down,0.75f,0.75f,0.75f);
						vert(x+1, y,   z+1,right, down,0.75f,0.75f,0.75f);
						vert(x+1, y+1, z+1,right, up,0.75f,0.75f,0.75f);
						vert(x,   y+1, z+1,left, up,0.75f,0.75f,0.75f);

						triangle(iVertex, iVertex+1, iVertex+2);
						triangle(iVertex+2, iVertex+3, iVertex);

						iVertex += 4;
					}
				}
			}
		}
	}

	//done
	if(iVertex > 0)
	{
		BuildChunkVertexBuffer(chunk,false);
	}
}
