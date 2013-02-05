#ifndef BLOCKDIRT_H
#define BLOCKDIRT_H

#include "BaseBlock.h"

class BlockDirt : public Block
{

public:

	BlockDirt()
	{
		_blockName = "Dirt";
		_blockID = 3;

		_transparent = false;
		_collision = true;
		_lightSource = false;
		_visible = true;
		_editable = true;
		_special = false;
		_single = false;

		//texture position
		_upPlane.x	= 2; _upPlane.y	= 0;
		_downPlane.x = 2; _downPlane.y = 0;
		_sidePlane.x = 2; _sidePlane.y = 0;
	}

};

#endif
