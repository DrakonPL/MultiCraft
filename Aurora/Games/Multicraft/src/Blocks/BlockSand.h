#ifndef BLOCKSAND_H
#define BLOCKSAND_H

#include "BaseBlock.h"

class BlockSand : public Block
{

public:

	BlockSand()
	{
		_blockName = "BlockSand";
		_blockID = 12;

		_transparent = false;
		_collision = true;
		_lightSource = false;
		_visible = true;
		_editable = false;
		_special = false;
		_single = false;

		//texture position
		_upPlane.x	= 2.0f; _upPlane.y	= 1.0f;
		_downPlane.x = 2.0f; _downPlane.y = 1.0f;
		_sidePlane.x = 2.0f; _sidePlane.y = 1.0f;
	}

};

#endif
