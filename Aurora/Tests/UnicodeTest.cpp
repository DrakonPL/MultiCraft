#include "UnicodeTest.h"
#include "ThreadTest.h"

void UnicodeTest::Init()
{
	_renderManager = RenderManager::Instance();
	_systemManager = SystemManager::Instance();

	font2 = new TrueTypeFont("Assets/Minecraft/font.ttf",16);
	
	font = new UnicodeFont(256,256);

	regular = font->LoadFont("Assets/Fonts/DroidSerif-Regular.ttf");
	italic = font->LoadFont("Assets/Fonts/DroidSerif-Italic.ttf");
	bold = font->LoadFont("Assets/Fonts/DroidSerif-Bold.ttf");
	japanese = font->LoadFont("Assets/Fonts/DroidSansJapanese.ttf");
	
	dt = 0.0f;
}

void UnicodeTest::Enter()
{
	RenderManager::Instance()->SetOrtho();
	_clock.Reset();
}

void UnicodeTest::CleanUp()
{

}

void UnicodeTest::Pause()
{

}

void UnicodeTest::Resume()
{

}

void UnicodeTest::GamePause()
{
    
}

void UnicodeTest::GameResume()
{
    
}

void UnicodeTest::HandleEvents(GameManager* sManager)
{
	_systemManager->Update();

	if (_systemManager->keyPressed(Key::Space))
	{
		ThreadsTest* threadsTest = new ThreadsTest();
		threadsTest->Init();

		sManager->ChangeState(threadsTest);
	}

}

void UnicodeTest::Update(GameManager* sManager)
{
	//delta time
	dt = _clock.getTime();
	_clock.Reset();
}

void UnicodeTest::Draw(GameManager* sManager)
{
	_renderManager->StartFrame();
	_renderManager->ClearScreen();

	_renderManager->SetOrtho();

	_renderManager->SetColor(0xffffffff);

	sx = 10; sy = 50;

	font->BeginText();

	dx = sx; dy = sy;

	font->DrawText( 1,24.0f, dx,dy,"The quick ",&dx);
	font->DrawText( 2,48.0f, dx,dy,"brown ",&dx);
	font->DrawText( 1,24.0f, dx,dy,"fox ",&dx);

	dx = sx;
	dy += 40;

	font->DrawText( 2,24.0f, dx,dy,"jumps over ",&dx);
	font->DrawText( 3,24.0f, dx,dy,"the lazy ",&dx);
	font->DrawText( 1,24.0f, dx,dy,"dog.",&dx);

	dx = sx;
	dy += 30;
	font->DrawText( 1,15.0f, dx,dy,"aąbcćdeęfghijklłmnńoóprsśtuwyzźżXxVvQq0123456789~!@#$%^&*",&dx);

	dx = sx;
	dy += 20;
	font->DrawText( 2,18.0f, dx,dy,"Ég get etið gler án þess að meiða mig.",&dx);

	dx = sx;
	dy += 30;
	font->DrawText( 4,24.0f, dx,dy,"私はガラスを食べられます。それは私を傷つけません。",&dx);

	font->EndText();


	//draw text using old class
	font2->DrawText(_renderManager->GetWidth()/2,_renderManager->GetHeight() - 2,"Press Space/Select to change test.",Aurora::Graphics::ALIGN_CENTER,Aurora::Graphics::RenderManager::RGBA(0x00, 0x00, 0x00, 0xff));
	

	_renderManager->EndFrame();
}

