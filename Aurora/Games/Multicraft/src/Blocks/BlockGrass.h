#ifndef BLOCKGRASS_H
#define BLOCKGRASS_H

#include "BaseBlock.h"

class BlockGrass : public Block
{

public:

	BlockGrass()
	{
		_blockName = "Grass";
		_blockID = 2;

		_transparent = false;
		_collision = true;
		_lightSource = false;
		_visible = true;
		_editable = true;
		_special = false;
		_single = false;

		//texture position
		_upPlane.x	= 0; _upPlane.y	= 0;
		_downPlane.x = 2; _downPlane.y = 0;
		_sidePlane.x = 3; _sidePlane.y = 0;
	}

};

#endif
