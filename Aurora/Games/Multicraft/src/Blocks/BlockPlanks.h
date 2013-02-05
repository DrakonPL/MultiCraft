#ifndef BLOCKPLANKS_H
#define BLOCKPLANKS_H

#include "BaseBlock.h"

class BlockPlanks : public Block
{

public:

	BlockPlanks()
	{
		_blockName = "Planks";
		_blockID = 5;

		_transparent = false;
		_collision = true;
		_lightSource = false;
		_visible = true;
		_editable = true;
		_special = false;
		_single = false;

		//texture position
		_upPlane.x	= 4.0f; _upPlane.y	= 0.0f;
		_downPlane.x = 4.0f; _downPlane.y = 0.0f;
		_sidePlane.x = 4.0f; _sidePlane.y = 0.0f;
	}

};

#endif
