#include "MovingEntity.h"

#include "../Blocks/Blocks.h"
#include "../World/World.h"

#include <math.h>
#include <algorithm>

/*template <typename T>
void FreeAll( T & t ) {
	T tmp;
	t.swap( tmp );
}*/

MovingEntity::MovingEntity()
{
	_viewDirection = Vector3(0,0,0);
	_moveDirection = Vector3(1,0,1);
	_moveVelocity = Vector3(0,0,0);

	_canFly = false;
	_canAutostep = true;
	_onGround = false;
	_isSwimming = false;
	_isJumping = false;
	_isRunning = false;
	_isWalking = false;

	_maxAcceleration = 56.0f;
	_earthAcceleration = 28.0f;
	_movementAcceleration = 3.0f;//5.0f;
	_runningFactor = 1.5f;
	_jumpIntensity = 10.0f;	
	_groundFristion = 8.0f;

	_earthVelocity = 0.0f;
	_activeMovementAcc = _movementAcceleration;
}

bool MovingEntity::OnGround()
{
	return _onGround;  // not air
}

int BlockPositionsSort (const void * a, const void * b)
{
	BlockPosition *ia = (BlockPosition *)a;
	BlockPosition *ib = (BlockPosition *)b;

	if( ia->distanceToOrigin < ib->distanceToOrigin ) return -1;
    else if( ia->distanceToOrigin == ib->distanceToOrigin ) return 0;
    else return 1;
}

void MovingEntity::GatherBlockPositions(Vector3 pos)
{
	//clear block positions
	_blockPositions.clear();

	//add new ones
	for (int x = -1; x <= 1; x++)
	{
		for (int z = -1; z <= 1; z++)
		{
			for (int y = -2; y < 2; y++)
			{
				int blockPosX = (int) (pos.x + (pos.x >= 0 ? 0.5f : -0.5f)) + x;
				int blockPosY = (int) (pos.y + (pos.y >= 0 ? 0.5f : -0.5f)) + y;
				int blockPosZ = (int) (pos.z + (pos.z >= 0 ? 0.5f : -0.5f)) + z;

				float fromOrigin = Vector3(blockPosX - pos.x, blockPosY - pos.y, blockPosZ - pos.z).magnitudeSq();
				
				_blockPositions.push_back(BlockPosition(blockPosX, blockPosY, blockPosZ,fromOrigin));
			}
		}
	}

	//sort them
	qsort(&_blockPositions[0],_blockPositions.size(),sizeof(BlockPosition),BlockPositionsSort);
}

bool MovingEntity::VerticalCollision(Vector3 pos)
{
	bool colided = false;

	for (int i = 0;i < 36;i++)
	{
		BlockPosition *bp = &_blockPositions[i];

		if (_world->IsBlockCollidable(bp->position.x,bp->position.y,bp->position.z))
		{
			BoundingBox blockBox = BoundingBox(Vector3(bp->position.x,bp->position.y,bp->position.z),Vector3(bp->position.x+1,bp->position.y+1,bp->position.z+1));

			if (blockBox.contains(pos))
			{
				colided = true;
				float moveDirection = pos.y - blockBox.getCenter().y;

				if (moveDirection > 0.0f)
				{
					_position.y += (0.5f - moveDirection);
				}else if (moveDirection < 0.0f)
				{
					_position.y -= (0.51f + moveDirection);
				}
			}
		}
	}

	return colided;
}

bool MovingEntity::_checkUpDownCollision()
{
	bool colided = false;

	Vector3 footPos = _position + Vector3(0,-_downSize.y,0);
	GatherBlockPositions(footPos);
	if(VerticalCollision(footPos))
		colided = true;

	Vector3 footPos1 = _position + Vector3(-0.2f,-_downSize.y,-0.2f);
	if(VerticalCollision(footPos1))
		colided = true;

	Vector3 footPos2 = _position + Vector3(-0.2f,-_downSize.y,0.2f);
	if(VerticalCollision(footPos2))
		colided = true;

	Vector3 footPos3 = _position + Vector3(0.2f,-_downSize.y,-0.2f);
	if(VerticalCollision(footPos3))
		colided = true;

	Vector3 footPos4 = _position + Vector3(0.2f,-_downSize.y,0.2f);	
	if(VerticalCollision(footPos4))
		colided = true;

	//also check collision with ceiling
	Vector3 headPos = _position + Vector3(0,_upSize.y,0);
	GatherBlockPositions(headPos);
	VerticalCollision(headPos);

	if (colided)
	{
		_oldPosition = _position;
	}

	return colided;
}

bool MovingEntity::HorizontalColision(Vector3 pos)
{
	bool colided = false;

	GatherBlockPositions(pos);

	for (int i = 0;i < 36;i++)
	{
		BlockPosition *bp = &_blockPositions[i];

		if (_world->IsBlockCollidable(bp->position.x,bp->position.y,bp->position.z))
		{
			BoundingBox blockBox = BoundingBox(Vector3(bp->position.x,bp->position.y,bp->position.z),Vector3(bp->position.x+1,bp->position.y+1,bp->position.z+1));

			if (_aabb.intersect(blockBox))
			{
				colided = true;

				Vector3 direction = Vector3(_position.x, 0.0f, _position.z);
                direction.x -= pos.x;
                direction.z -= pos.z;

				// Calculate the point of intersection on the block's AABB
				Vector3 blockPoi = blockBox.ClosestPointTo(pos);

				BoundingBox newAABB = BoundingBox(Vector3(pos.x - _downSize.x,pos.y - _downSize.y,pos.z - _downSize.z),Vector3(pos.x + _upSize.x,pos.y + _upSize.y,pos.z +_upSize.z));
				Vector3 entityPoi = newAABB.ClosestPointTo(blockPoi);

				Vector3 planeNormal = blockBox.getFirstHitPlane(direction, pos, _aabb.getDimension(), true, false, true);

				// Find a vector parallel to the surface normal
                Vector3 slideVector = Vector3(planeNormal.z, 0, -planeNormal.x);
                Vector3 pushBack = blockPoi - entityPoi;

                // Calculate the intensity of the diversion alongside the block
                double length =  Vector3::dot(slideVector,direction);

                Vector3 newPosition;
                newPosition.z = pos.z + pushBack.z * 0.2f + length * slideVector.z;
                newPosition.x = pos.x + pushBack.x * 0.2f + length * slideVector.x;
                newPosition.y = _position.y;

				_position = newPosition;

				UpdateAABB();
			}
		}
	}

	return colided;
}

bool MovingEntity::GetFlyState()
{
	return _canFly;
}

void MovingEntity::SetFlyState(bool state)
{
	_canFly = state;
}

void  MovingEntity::Jump()
{
	if (!_isJumping && _onGround)
	{
		_isJumping = true;
		_onGround = false;
	}
}

void  MovingEntity::MoveForward()
{
	_moveDirection.x += _viewDirection.x;
	_moveDirection.z += _viewDirection.z;

	if (_canFly)
	{
		_moveDirection.y += _viewDirection.y;
	}	

	_isWalking = true;
}

void  MovingEntity::MoveBackward()
{
	_moveDirection.x -= _viewDirection.x;
	_moveDirection.z -= _viewDirection.z;

	if (_canFly)
	{
		_moveDirection.y -= _viewDirection.y;
	}

	_isWalking = true;
}

void  MovingEntity::StrafeLeft()
{
	_moveDirection -= Vector3::cross(_viewDirection, Vector3(0,1,0));

	_isWalking = true;
}

void  MovingEntity::StrafeRight()
{
	_moveDirection += Vector3::cross(_viewDirection, Vector3(0,1,0));
	
	_isWalking = true;
}

void MovingEntity::SetViewDirection(Vector3 direction)
{
	_viewDirection = direction;
}

void  MovingEntity::Update(float dt)
{
	if (!_isRunning)
		_activeMovementAcc = _movementAcceleration;
	else if (_canFly)
		_activeMovementAcc = _movementAcceleration * 4.0;
	else
		_activeMovementAcc = _movementAcceleration * _runningFactor;

	_oldPosition = _position;

	//slow down speed each pass
	if (fabsf(_moveVelocity.y) > 0.0f)
	{
		_moveVelocity.y += -1.0f * _moveVelocity.y * _groundFristion * dt;
	}

	if (fabsf(_moveVelocity.x) > 0.0f)
	{
		_moveVelocity.x += -1.0f * _moveVelocity.x * _groundFristion * dt;
	}

	if (fabsf(_moveVelocity.z) > 0.0f)
	{
		_moveVelocity.z += -1.0f * _moveVelocity.z * _groundFristion * dt;
	}

	//friction
	if (fabsf(_moveVelocity.x) > _activeMovementAcc || fabsf(_moveVelocity.z) > _activeMovementAcc || fabsf(_moveVelocity.y) > _activeMovementAcc)
	{
		
		float max = std::max(std::max(fabsf(_moveVelocity.x), fabsf(_moveVelocity.z)), fabsf(_moveVelocity.y));
		float div = max / _activeMovementAcc;

		if (div != 0.0f)
		{
			_moveVelocity.x /= div;
			_moveVelocity.z /= div;
			_moveVelocity.y /= div;
		}		
	}

	if (_moveDirection.magnitudeSq() > 0.0f)
	{
		_moveDirection.normalize();
	}

	_moveVelocity.x += _moveDirection.x * _activeMovementAcc;
	_moveVelocity.y += _moveDirection.y * _activeMovementAcc;
	_moveVelocity.z += _moveDirection.z * _activeMovementAcc;

	if (_earthVelocity > -_maxAcceleration && !_canFly && !_isSwimming)
	{
		_earthVelocity -= _earthAcceleration * dt;
	}

	if (_earthVelocity < -_maxAcceleration && !_canFly && !_isSwimming)
	{
		_earthVelocity = -_maxAcceleration * dt;
	}

	// Pull the player down when swimming
	/*if (!_canFly && _isSwimming)
	{
		_earthVelocity = -MAX_EARTH_ACC_WATER;
	}*/

	_position.y += (_moveVelocity.y + _earthVelocity) * dt;

	if(_checkUpDownCollision())
	{
		float oldEarthVelocity = _earthVelocity;
		_earthVelocity = 0;

		if (oldEarthVelocity <= 0)
		{
			// Jumping is only possible, if the entity is standing on ground
			if (_isJumping)
			{
				//playRandomFootstep();
				_isJumping = false;
				_earthVelocity = _jumpIntensity;
			} 
			else if (!_onGround)
			{ 
				// Entity reaches the ground
				//playRandomFootstep();
				_onGround = true;
			}
		} else
		{
			_onGround = false;
		}
	}
	else
	{
		_onGround = false;
	}

	if (_canFly)
	{
		 _earthVelocity = 0.0f;
	}

	_position.x += _moveVelocity.x * dt;
	_position.z += _moveVelocity.z * dt;

	UpdateAABB();

	if(HorizontalColision(_oldPosition))
	{
		if (_canAutostep && _onGround && !_canFly)
		{
			Vector3 testPos = _position + Vector3(0,-_downSize.y,0);
			Vector3 testPos2 = _position + Vector3(0,-_downSize.y,0);
			Vector3 velocityCopy = Vector3::normalized(_moveVelocity);

			testPos.x = testPos.x < 0 ? testPos.x - 1 : testPos.x;
			testPos.y = testPos.y < 0 ? testPos.y - 1 : testPos.y;
			testPos.z = testPos.z < 0 ? testPos.z - 1 : testPos.z;

			testPos2.x = testPos2.x < 0 ? testPos2.x - 1 : testPos2.x;
			testPos2.y = testPos2.y < 0 ? testPos2.y - 1 : testPos2.y;
			testPos2.z = testPos2.z < 0 ? testPos2.z - 1 : testPos2.z;

			//first test if at normal y there is blocking block
			testPos2.x += velocityCopy.x;			
			testPos2.z += velocityCopy.z;

			//second test if at y+1 is non blocking block
			testPos.x += velocityCopy.x;			
			testPos.z += velocityCopy.z;
			testPos.y += 1.0f;
	
			if (!_world->IsBlockCollidable(testPos.x,testPos.y,testPos.z) && _world->IsBlockCollidable(testPos2.x,testPos2.y,testPos2.z))
			{
				Jump();
			}
		}
	}

	/*_moveVelocity = _moveDirection * 4.0f * dt;

	//first check collision with floor
	_position.y = _position.y + _moveVelocity.y;

	if(_checkUpDownCollision())
	{

	}

	//now check collision with walls
	_position.x = _position.x + _moveVelocity.x;	
	_position.z = _position.z + _moveVelocity.z;
	
	UpdateAABB();
	
	HorizontalColision(_oldPosition);*/

	//reset move direction
	_moveDirection.set(0,0,0);

	_isWalking = false;
}

