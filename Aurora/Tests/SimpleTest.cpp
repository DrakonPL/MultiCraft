#include "SimpleTest.h"
#include <Aurora/Utils/Logger.h>


void SimpleTest::Init()
{
	_renderManager = RenderManager::Instance();
	_systemManager = SystemManager::Instance();
	
	font = new TrueTypeFont("Assets/Minecraft/font.ttf",16);
	
	dt = 0.0f;
}

void SimpleTest::Enter()
{
	RenderManager::Instance()->SetOrtho();
	_clock.Reset();
}

void SimpleTest::CleanUp()
{
//delete font;
}

void SimpleTest::Pause()
{

}

void SimpleTest::Resume()
{

}

void SimpleTest::GamePause()
{
    
}

void SimpleTest::GameResume()
{
    
}

void SimpleTest::HandleEvents(GameManager* sManager)
{
	_systemManager->Update();

}

void SimpleTest::Update(GameManager* sManager)
{
	//delta time
	dt = _clock.getTime();
	_clock.Reset();
}
void SimpleTest::Draw(GameManager* sManager)
{
	_renderManager->StartFrame();
	_renderManager->ClearScreen();

	_renderManager->SetOrtho();

	//draw fps
	char deltaTime[30];
	sprintf(deltaTime,"dt: %f",dt);
	font->DrawText(_renderManager->GetWidth()/2,13,deltaTime,Aurora::Graphics::ALIGN_CENTER,Aurora::Graphics::RenderManager::RGBA(0x00, 0x00, 0x00, 0xff));

	_renderManager->EndFrame();
}

