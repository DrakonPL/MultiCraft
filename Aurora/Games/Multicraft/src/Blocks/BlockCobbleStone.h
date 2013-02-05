#ifndef BLOCKCOBBLESTONE_H
#define BLOCKCOBBLESTONE_H

#include "BaseBlock.h"

class BlockCobbleStone : public Block
{

public:

	BlockCobbleStone()
	{
		_blockName = "Cobblestone";
		_blockID = 4;

		_transparent = false;
		_collision = true;
		_lightSource = false;
		_visible = true;
		_editable = true;
		_special = false;
		_single = false;

		//texture position
		_upPlane.x	= 0; _upPlane.y	= 1;
		_downPlane.x = 0; _downPlane.y = 1;
		_sidePlane.x = 0; _sidePlane.y = 1;
	}

};

#endif
