#include "ThreadTest.h"
#include <Aurora/Utils/Logger.h>

void ThreadsTest::Init()
{
	_renderManager = RenderManager::Instance();
	_systemManager = SystemManager::Instance();
	
	font = new TrueTypeFont("Assets/Minecraft/font.ttf",16);
	
	dt = 0.0f;
	Utils::Logger::Instance()->LogMessage("Logger test\n");
	
	test = 0;
	
	_thread = new Thread(&ThreadsTest::ThreadFunc,this);
	_thread->launch();
}

void ThreadsTest::ThreadFunc()
{
	while (1)
	{
		test++;
		Aurora::Threads::sleep(1000);
	}
}

void ThreadsTest::Enter()
{
	RenderManager::Instance()->SetOrtho();
	_clock.Reset();
}

void ThreadsTest::CleanUp()
{
//delete font;
}

void ThreadsTest::Pause()
{

}

void ThreadsTest::Resume()
{

}

void ThreadsTest::GamePause()
{
    
}

void ThreadsTest::GameResume()
{
    
}

void ThreadsTest::HandleEvents(GameManager* sManager)
{
	_systemManager->Update();

}

void ThreadsTest::Update(GameManager* sManager)
{
	//delta time
	dt = _clock.getTime();
	_clock.Reset();
}
void ThreadsTest::Draw(GameManager* sManager)
{
	_renderManager->StartFrame();
	_renderManager->ClearScreen();

	_renderManager->SetOrtho();

	//draw fps
	char deltaTime[30];
	sprintf(deltaTime,"dt: %f test: %i",dt,test);
	font->DrawText(2,13,deltaTime,Aurora::Graphics::ALIGN_LEFT,Aurora::Graphics::RenderManager::RGBA(0x00, 0x00, 0x00, 0xff));

	font->DrawText(_renderManager->GetWidth()/2,13,"Thread test",Aurora::Graphics::ALIGN_CENTER,Aurora::Graphics::RenderManager::RGBA(0x00, 0x00, 0x00, 0xff));


	_renderManager->EndFrame();
}

