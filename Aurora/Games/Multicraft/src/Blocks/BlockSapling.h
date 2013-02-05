#ifndef BLOCKSAPLING_H
#define BLOCKSAPLING_H

#include "BaseBlock.h"

class BlockSapling : public Block
{

public:

	BlockSapling()
	{
		_blockName = "BlockSapling";
		_blockID = 6;

		_transparent = false;
		_collision = true;
		_lightSource = false;
		_visible = true;
		_editable = false;
		_special = false;
		_single = false;

		//texture position
		_upPlane.x	= 15.0f; _upPlane.y	= 0.0f;
		_downPlane.x = 15.0f; _downPlane.y = 0.0f;
		_sidePlane.x = 15.0f; _sidePlane.y = 0.0f;
	}

};

#endif
