#ifndef BLOCKSTONE_H
#define BLOCKSTONE_H

#include "BaseBlock.h"

class BlockStone : public Block
{

public:

	BlockStone()
	{
		_blockName = "Stone";
		_blockID = 1;

		_transparent = false;
		_collision = true;
		_lightSource = false;
		_visible = true;
		_editable = true;
		_special = false;
		_single = false;

		//texture position
		_upPlane.x	= 1.0f; _upPlane.y	= 0.0f;
		_downPlane.x = 1.0f; _downPlane.y = 0.0f;
		_sidePlane.x = 1.0f; _sidePlane.y = 0.0f;
	}

};

#endif
