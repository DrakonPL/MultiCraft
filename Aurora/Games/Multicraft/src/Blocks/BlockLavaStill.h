#ifndef BLOCKLAVASTILL_H
#define BLOCKLAVASTILL_H

#include "BaseBlock.h"

class BlockLavaStill : public Block
{

public:

	BlockLavaStill()
	{
		_blockName = "BlockLavaStill";
		_blockID = 11;

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
