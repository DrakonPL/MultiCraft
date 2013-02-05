#ifndef BLOCKBEDEROCK_H
#define BLOCKBEDEROCK_H

#include "BaseBlock.h"

class BlockBedrock : public Block
{

public:

	BlockBedrock()
	{
		_blockName = "BlockBedrock";
		_blockID = 7;

		_transparent = false;
		_collision = true;
		_lightSource = false;
		_visible = true;
		_editable = true;
		_special = false;
		_single = false;

		//texture position
		_upPlane.x	= 1; _upPlane.y	= 1;
		_downPlane.x = 1; _downPlane.y = 1;
		_sidePlane.x = 1; _sidePlane.y = 1;
	}

};

#endif