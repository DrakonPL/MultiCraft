#ifndef BLOCKLAVAMOVING_H
#define BLOCKLAVAMOVING_H

#include "BaseBlock.h"

class BlockLavaMoving : public Block
{

public:

	BlockLavaMoving()
	{
		_blockName = "BlockLavaMoving";
		_blockID = 10;

		_transparent = false;
		_collision = true;
		_lightSource = false;
		_visible = true;
		_editable = true;
		_special = false;
		_single = false;

		//texture position
		_upPlane.x	= 13; _upPlane.y	= 14;
		_downPlane.x = 13; _downPlane.y = 14;
		_sidePlane.x = 13; _sidePlane.y = 14;
	}

};

#endif