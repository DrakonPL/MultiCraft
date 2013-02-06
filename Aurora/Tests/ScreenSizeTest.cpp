#include "ScreenSizeTest.h"
#include "ThreadTest.h"

void ScreenSizeTest::Init()
{
	_renderManager = RenderManager::Instance();
	_systemManager = SystemManager::Instance();
	
	font = new TrueTypeFont("Assets/Minecraft/font.ttf",16);

	itemSprite = new Sprite("Assets/Minecraft/items.png",0,0,16,16);
	itemSprite->Scale(4.0f,4.0f);

	dt = 0.0f;
}

void ScreenSizeTest::Enter()
{
	RenderManager::Instance()->SetOrtho();
	_clock.Reset();
}

void ScreenSizeTest::CleanUp()
{
//delete font;
}

void ScreenSizeTest::Pause()
{

}

void ScreenSizeTest::Resume()
{

}

void ScreenSizeTest::GamePause()
{

}

void ScreenSizeTest::GameResume()
{

}

void ScreenSizeTest::HandleEvents(GameManager* sManager)
{
	_systemManager->Update();

	if (_systemManager->keyPressed(Key::Space))
	{
		ThreadsTest* threadsTest = new ThreadsTest();
		threadsTest->Init();

		sManager->ChangeState(threadsTest);
	}
}

void ScreenSizeTest::Update(GameManager* sManager)
{
	//delta time
	dt = _clock.getTime();
	_clock.Reset();
}
void ScreenSizeTest::Draw(GameManager* sManager)
{
	RenderManager::Instance()->StartFrame();
	RenderManager::Instance()->ClearScreen();

	RenderManager::Instance()->SetOrtho();


	RenderManager::Instance()->DrawImage(TextureManager::Instance()->getImage("Assets/Minecraft/items.png"));

	RenderManager::Instance()->DrawSprite(itemSprite,_renderManager->GetWidth()/2 + _renderManager->GetWidth()/4,_renderManager->GetHeight()/2);

	font->DrawText(_renderManager->GetWidth()/2,13,"Sprite test",Aurora::Graphics::ALIGN_CENTER,Aurora::Graphics::RenderManager::RGBA(0x00, 0x00, 0x00, 0xff));
	
	font->DrawText(_renderManager->GetWidth()/2,_renderManager->GetHeight() - 2,"Press Space/Select to change test.",Aurora::Graphics::ALIGN_CENTER,Aurora::Graphics::RenderManager::RGBA(0x00, 0x00, 0x00, 0xff));


	RenderManager::Instance()->EndFrame();
}

