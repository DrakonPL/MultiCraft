#include <Aurora/Graphics/RenderManager.h>
#include <Aurora/Graphics/TrueTypeFont.h>
#include <Aurora/Graphics/Sprite.h>
#include <Aurora/Graphics/TextureManager.h>
#include <Aurora/Utils/GameManager.h>
#include <Aurora/Utils/GameState.h>
#include <Aurora/System/SystemManager.h>
#include <Aurora/System/Clock.h>

using namespace Aurora;
using namespace Aurora::Graphics;
using namespace Aurora::Utils;
using namespace Aurora::System;
using namespace Aurora::Math;

class SpriteTest : public GameState
{
private:

	TrueTypeFont* font;
	Sprite* itemSprite;
	
	RenderManager* _renderManager;
	SystemManager* _systemManager;

	Clock _clock;
	float dt;

	float widthPercent;
	float heightPercent;

	int viewLeft;
	int viewRight;
	int viewUp;
	int viewDown;

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

