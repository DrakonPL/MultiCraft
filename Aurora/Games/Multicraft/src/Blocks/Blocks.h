#ifndef BLOCKS_H
#define BLOCKS_H

#include "BaseBlock.h"
#include "BlockStone.h"
#include "BlockGrass.h"
#include "BlockDirt.h"
#include "BlockCobbleStone.h"
#include "BlockPlanks.h"
#include "BlockSapling.h"
#include "BlockBedrock.h"
#include "BlockWaterMoving.h"
#include "BlockWaterStill.h"
#include "BlockLavaMoving.h"
#include "BlockLavaStill.h"
#include "BlockSand.h"
#include "BlockLog.h"
#include "BlockLeaf.h"

class Blocks
{
public:

	static const unsigned char Air = 0;

	static const unsigned char Stone = 1;
	static const unsigned char Grass = 2;
	static const unsigned char Dirt = 3;
	static const unsigned char CobbleStone = 4;
	static const unsigned char Planks = 5;
	static const unsigned char Sapling = 6;
	static const unsigned char Bedrock = 7;
	static const unsigned char WaterMoving = 8;
	static const unsigned char WaterStill = 9;
	static const unsigned char LavaMoving = 10;
	static const unsigned char LavaStill = 11;
	static const unsigned char Sand = 12;
	static const unsigned char Log = 13;
	static const unsigned char Leaf = 14;

};

#endif
