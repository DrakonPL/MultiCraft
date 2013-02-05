#include "Entity.h"

BoundingBox Entity::GetAabb()
{
	return _aabb;
}

void Entity::SetPosition(float x,float y,float z)
{
	_position.set(x,y,z);
	UpdateAABB();
}

void Entity::SetSize(Vector3 downSize,Vector3 upSize)
{
	_downSize = downSize;
	_upSize = upSize;
	UpdateAABB();
}

void Entity::UpdateAABB()
{
	_aabb.min.set(_position.x - _downSize.x,_position.y - _downSize.y,_position.z - _downSize.z);
	_aabb.max.set(_position.x + _upSize.x,_position.y + _upSize.y,_position.z + _upSize.z);
	_aabb.Validity = BoundingBox::Valid;
}

Vector3 Entity::GetPosition()
{
	return _position;
}

