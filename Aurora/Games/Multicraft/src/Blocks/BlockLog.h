#ifndef BLOCKLOG_H
#define BLOCKLOG_H

#include "BaseBlock.h"

class BlockLog : public Block
{

public:

	BlockLog()
	{
		_blockName = "BlockLog";
		_blockID = 13;

		_transparent = false;
		_collision = true;
		_lightSource = false;
		_visible = true;
		_editable = true;
		_special = false;
		_single = false;

		//texture position
		_upPlane.x	= 5; _upPlane.y	= 1;
		_downPlane.x = 5; _downPlane.y = 1;
		_sidePlane.x = 4; _sidePlane.y = 1;
	}

};

#endif
