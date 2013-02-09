#include <Aurora/Graphics/RenderManager.h>
#include <Aurora/Graphics/UnicodeFont.h>
#include <Aurora/Utils/GameManager.h>
#include <Aurora/Utils/GameState.h>
#include <Aurora/System/SystemManager.h>
#include <Aurora/System/Clock.h>

using namespace Aurora;
using namespace Aurora::Graphics;
using namespace Aurora::Utils;
using namespace Aurora::System;
using namespace Aurora::Math;

#include <string>

class UnicodeTest : public GameState
{
private:

	int italic,bold,regular,japanese;
	float sx,sy,dx,dy,lh;

	UnicodeFont* font;
	TrueTypeFont* font2;

	RenderManager* _renderManager;
	SystemManager* _systemManager;

	Clock _clock;
	float dt;

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

