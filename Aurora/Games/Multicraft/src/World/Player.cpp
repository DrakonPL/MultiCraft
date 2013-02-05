#include "Player.h"

Player::Player(std::string playerName)
{
	_playerName = playerName;

	_canFly = false;

	//load model
	//_playerModel  = new ModelMd5();
	//_playerModel->Load("Assets/Models/Md5/player/player.md5b");

	//_playerModel->GetMesh("head")->isVisible = false;
}

void Player::SetPlayerName(std::string playerName)
{
	_playerName = playerName;
}

void Player::SetPlayerCamera(Camera* cam)
{
	_playerCamera = cam;
}

void Player::SetPlayerFrustum(Frustum* frustum)
{
	_playerFrustum = frustum;
}

void Player::UpdateViewDirection()
{
	SetViewDirection(_playerCamera->GetViewDirection());

	_modelDirection = Vector2(_viewDirection.x,_viewDirection.z);
	_modelDirection.normalize();

	//_playerModel->FaceDirection(_modelDirection);
}

void Player::Update(float dt)
{
	//if (_isWalking)
	//{
	//	_playerModel->UpdateAnimation(dt);
	//}

	MovingEntity::Update(dt);

	if (_oldPosition != _position)
	{
		_playerCamera->SetPosition(_position);
	}	

	Vector2 test = _modelDirection * 0.4f;
	//_playerModel->SetPosition(_position - Vector3(test.x,1.5f,test.y));	
}

void Player::Draw()
{
	//_playerModel->Draw();
}
