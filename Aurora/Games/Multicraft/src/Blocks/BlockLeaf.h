#ifndef BLOCKLEAF_H
#define BLOCKLEAF_H

#include "BaseBlock.h"

class BlockLeaf : public Block
{

public:

	BlockLeaf()
	{
		_blockName = "BlockLeaf";
		_blockID = 14;

		_transparent = true;
		_collision = true;
		_lightSource = false;
		_visible = true;
		_editable = true;
		_special = false;
		_single = false;

		//texture position
		_upPlane.x	= 4; _upPlane.y	= 3;
		_downPlane.x = 4; _downPlane.y = 3;
		_sidePlane.x = 4; _sidePlane.y = 3;
	}

};

#endif
