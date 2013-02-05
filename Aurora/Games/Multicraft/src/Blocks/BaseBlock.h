#ifndef BLOCK_H
#define BLOCK_H

#include <string>

#include <Aurora/Math/Vector2.h>
using namespace Aurora::Math;


class World;
class VertexBuilder;

class Block
{
protected:

	bool _transparent;
	bool _collision;
	bool _lightSource;
	bool _visible;
	bool _editable;
	bool _special;
	bool _single;

	unsigned char _blockID;

	std::string _blockName;
	
	Vector2 _upPlane;
	Vector2 _downPlane;
	Vector2 _sidePlane;

public:

	Block();
	virtual ~Block();

	unsigned char GetBlockId();
	std::string GetBlockName();

	virtual void OnBlockAdded(World *mWorld,int x,int y,int z);
	virtual void OnBlockRemoved(World *mWorld,int x,int y,int z);

	virtual void OnNeighborBlockChanged(World *mWorld,int x,int y,int z);
	virtual void OnBlockClicked(World *mWorld,int x,int y,int z);	

	inline bool IsTransparent() { return _transparent; }
	inline bool IsCollidable() { return _collision; }
	inline bool IsLightSource() { return _lightSource; }
	inline bool IsVisible() { return _visible; }
	inline bool IsEditable() { return _editable; }
	inline bool IsSpecial() { return _special; }
	inline bool IsSingle() { return _single; }

	friend class VertexBuilder;
};

#endif
