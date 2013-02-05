#ifndef MOVINGENTITY_H
#define MOVINGENTITY_H

#include "Entity.h"
#include "BlockPosition.h"

#include <vector>

class MovingEntity : public Entity
{
protected:

	Vector3 _viewDirection;
	Vector3 _moveDirection;
	Vector3 _moveVelocity;

	Vector3 _oldPosition;

	bool _isWalking;
	bool _canFly;
	bool _onGround;
	bool _isSwimming;
	bool _isJumping;
	bool _isRunning;
	bool _canAutostep;

	float _earthVelocity;
	float _activeMovementAcc;

	//settings for movement
	float _maxAcceleration;
	float _earthAcceleration;
	float _movementAcceleration;
	float _runningFactor;
	float _jumpIntensity;	
	float _groundFristion;

	std::vector<BlockPosition> _blockPositions;

protected:

	bool _checkUpDownCollision();

public:

	MovingEntity();

	bool OnGround();

	void GatherBlockPositions(Vector3 pos);
	bool VerticalCollision(Vector3 pos);
	bool HorizontalColision(Vector3 pos);
	bool TryToMove(Vector3 moveVector,float dt);


public:

	void MoveForward();
	void MoveBackward();

	void StrafeLeft();
	void StrafeRight();

	void Jump();

	bool GetFlyState();
	void SetFlyState(bool state);

public:

	void SetViewDirection(Vector3 direction);

	virtual void Update(float dt);

};

#endif
