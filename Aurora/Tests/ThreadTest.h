#include <Aurora/Graphics/RenderManager.h>
#include <Aurora/Graphics/TrueTypeFont.h>
#include <Aurora/Utils/GameManager.h>
#include <Aurora/Utils/GameState.h>
#include <Aurora/System/SystemManager.h>
#include <Aurora/System/Clock.h>
#include <Aurora/Threads/Thread.hpp>
#include <Aurora/Threads/Sleep.h>

using namespace Aurora;
using namespace Aurora::Graphics;
using namespace Aurora::Utils;
using namespace Aurora::System;
using namespace Aurora::Math;
using namespace Aurora::Threads;

#include <string>

class ThreadsTest : public GameState
{
private:

	TrueTypeFont* font;

	RenderManager* _renderManager;
	SystemManager* _systemManager;

	Clock _clock;
	float dt;

	Thread *_thread;
	int test;

public:

	void ThreadFunc();

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

