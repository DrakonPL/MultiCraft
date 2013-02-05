#include "TreeGenerator.h"
#include "../../Blocks/Blocks.h"

#include "Aurora/Math/MathUtils.h"

//Basic tree generation
void TreeGenerator::BuildTree(int wx, int wy, int wz, int height, Chunk * chunk)
{
	//HACK HACK HACK Makes tree NEVER on chunk boundries so we dont have to deal with that
	if(wy + height >= chunk->GetChunkY() + chunk->GetChunkHeight())
		return;

	if(wx-height/2.0f < chunk->GetChunkX())
		return;

	if(wz-height/2.0f < chunk->GetChunkZ())
		return;

	if(wx+height/2.0f > chunk->GetChunkX()+chunk->GetChunkWidth())
		return;

	if(wz+height/2.0f > chunk->GetChunkZ()+chunk->GetChunkWidth())
		return;

	//End hack

	//Build Trunk
	if(height < 4)
		height = 4;

	for(int i=0;i<height;i++)
	{
		if(wy+i > 128)
			break;

		chunk->SetBlock(wx, wy+i, wz,Blocks::Log);
	}

	//Leaves
	for(int x=-4;x<=4;x++)
	{
		for(int y=0-floor(height/2.0f);y<=4;y++)
		{
			for(int z=-4;z<=4;z++)
			{
				int x1 = x+wx;
				int y1 = y+wy+height;
				int z1 = z+wz;

				if(ASqrtf(powf((wx-x1),2.0f) + powf((wy+height-y1),2.0f) + powf((wz-z1),2.0f))<height/2.0f) //Get the distance of
				{	
					if (y1 < chunk->GetChunkY() + chunk->GetChunkHeight())
					{
						if(chunk->GetBlock(x1,y1,z1) == Blocks::Air )
							chunk->SetBlock(x1,y1,z1,Blocks::Leaf);
					}					
				}
			}
		}
	}
}
