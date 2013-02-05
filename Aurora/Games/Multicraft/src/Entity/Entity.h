#ifndef ENTITY_H
#define ENTITY_H

#include <Aurora/Math/BoundingBox.h>
#include <Aurora/Math/Vector3.h>

using namespace Aurora::Math;

class World;

class Entity
{
protected:

	World *_world;

protected:

	BoundingBox _aabb;
	Vector3 _position;

	Vector3 _downSize;
	Vector3 _upSize;

public:

	BoundingBox GetAabb();
	Vector3 GetPosition();

	void SetPosition(float x,float y,float z);
	void SetSize(Vector3 downSize,Vector3 upSize);

	void UpdateAABB();

};

#endif
