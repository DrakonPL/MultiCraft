#ifndef BLOCKWATERMOVING_H
#define BLOCKWATERMOVING_H

#include "BaseBlock.h"

class BlockWaterMoving : public Block
{

public:

	BlockWaterMoving()
	{
		_blockName = "BlockWaterMoving";
		_blockID = 8;

		_transparent = true;
		_collision = false;
		_lightSource = false;
		_visible = true;
		_editable = true;
		_special = false;
		_single = false;

		//texture position
		_upPlane.x	= 13.0f; _upPlane.y	= 12.0f;
		_downPlane.x = 13.0f; _downPlane.y = 12.0f;
		_sidePlane.x = 13.0f; _sidePlane.y = 12.0f;
	}

};

#endif
