#ifndef MULTICRAFT_PLAYSTATE_H
#define MULTICRAFT_PLAYSTATE_H

#include <Aurora/Graphics/RenderManager.h>
#include <Aurora/Graphics/TrueTypeFont.h>
#include <Aurora/Graphics/Camera.h>
#include <Aurora/Graphics/SkyBox.h>
#include <Aurora/Graphics/models/ModelObj2.h>
#include <Aurora/Utils/GameManager.h>
#include <Aurora/Utils/GameState.h>
#include <Aurora/Math/Frustum.h>
#include <Aurora/System/SystemManager.h>
#include <Aurora/System/Clock.h>
#include <Aurora/System/Timer.h>
#include <Aurora/System/FileManager.h>

using namespace Aurora;
using namespace Aurora::Graphics;
using namespace Aurora::Utils;
using namespace Aurora::System;
using namespace Aurora::Math;

#include "World/Player.h"
#include "World/World.h"
#include "World/ChunkProvider.h"

class PlayState : public GameState
{
private:

	//framework
	TrueTypeFont* font;
	SkyBox* _skyBox;

	FileManager* _fileManager;
	RenderManager* _renderManager;
	SystemManager* _systemManager;

	Clock _clock;
	Clock _clock2;
	float dt;

	Timer _updateTimer;
	Timer _inputTimer;
	Timer _renderTimer;

	float _renderTime,_updateTime,_inputTime;

	//game
	Player* _player;
	World* _world;
	ChunkProvider* _chunkProvider;

	Camera* _playerCam;
	Frustum* _playerFrustum;

	//testing
	bool blockPicked;
	Vector3 pickPos;
	ModelObj2* cubeModel;

	int mouseX,mouseY;
	float pith,heading;

	//allows mouse to escape
	bool escaped;

public:

	void Init();
	void Enter();
	void CleanUp();

	void Pause();
	void Resume();

	void GamePause();
	void GameResume();

	void HandleEvents(GameManager* sManager);
	void Update(GameManager* sManager);
	void Draw(GameManager* sManager);
};

#endif
