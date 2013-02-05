#ifndef PLAYER_H
#define PLAYER_H

#include <Aurora/Math/Frustum.h>
#include <Aurora/Graphics/Camera.h>
#include <Aurora/Graphics/models/ModelMd5.h>

using namespace Aurora::Math;
using namespace Aurora::Graphics;

#include "../Entity/MovingEntity.h"
#include <string>

class Player : public MovingEntity
{
private:

	std::string _playerName;

	Camera* _playerCamera;
	Frustum* _playerFrustum;


	Vector2 _modelDirection;
	ModelMd5* _playerModel;

public:

	Player(std::string playerName);

	void SetPlayerName(std::string playerName);

	void SetPlayerCamera(Camera* cam);
	void SetPlayerFrustum(Frustum* frustum);

	void UpdateViewDirection();

	void Update(float dt);

	void Draw();


	friend class World;
};


#endif
