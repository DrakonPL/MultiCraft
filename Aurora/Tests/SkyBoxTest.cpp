#include "SkyBoxTest.h"
#include "ThreadTest.h"

void SkyBoxTest::Init()
{
	_renderManager = RenderManager::Instance();
	_systemManager = SystemManager::Instance();
	
	font = new TrueTypeFont("Assets/Minecraft/font.ttf",16);

	_skyBox = new SkyBox();
	_skyBox->LoadImage("Assets/Skybox/0Panorama_back.png",Back);
	_skyBox->LoadImage("Assets/Skybox/0Panorama_front.png",Front);
	_skyBox->LoadImage("Assets/Skybox/0Panorama_left.png",Left);
	_skyBox->LoadImage("Assets/Skybox/0Panorama_right.png",Right);
	_skyBox->LoadImage("Assets/Skybox/0Panorama_up.png",Up);
	_skyBox->LoadImage("Assets/Skybox/0Panorama_down.png",Down);

	_cam = new Camera();
	_cam->PositionCamera(0,0,5,0,0,0,0,1,0);
	_renderManager->SetCurrentCam(_cam);
	
	dt = 0.0f;
}

void SkyBoxTest::Enter()
{
	RenderManager::Instance()->SetOrtho();
	_clock.Reset();
}

void SkyBoxTest::CleanUp()
{
	
}

void SkyBoxTest::Pause()
{

}

void SkyBoxTest::Resume()
{

}

void SkyBoxTest::GamePause()
{
    
}

void SkyBoxTest::GameResume()
{
    
}

void SkyBoxTest::HandleEvents(GameManager* sManager)
{
	_systemManager->Update();

	mouseX = _systemManager->getMouseX();
	mouseY = _systemManager->getMouseY();

	if(!escaped)
	{
		if (_systemManager->platformUseMouse())
		{
			pith = ((RenderManager::Instance()->GetHeight()/2) - mouseY) * 0.5f *dt;
			heading = ((RenderManager::Instance()->GetWidth()/2) - mouseX) * 0.25f *dt;

			if (heading != 0.0f)
			{
				_cam->RotateView(heading,0,1,0);
			}

			if (pith != 0.0f)
			{
				_cam->PitchView(pith);
			}
		}
	}

	if(_systemManager->keyHold(Key::Left))
	{
		_cam->RotateView(1.0f * dt,0,1,0);
	}
	if(_systemManager->keyHold(Key::Right))
	{
		_cam->RotateView(-(1.0f * dt),0,1,0);
	}
	if(_systemManager->keyHold(Key::Up))
	{
		_cam->PitchView(1.0f * dt);
	}
	if(_systemManager->keyHold(Key::Down))
	{
		_cam->PitchView(-(1.0f * dt));
	}

	if(_systemManager->keyPressed(Key::Escape))
	{
		escaped = !escaped;
	}

	//move
	if(_systemManager->keyHold(Key::W))
	{
		_cam->Move(4.0f * dt);
	}
	if(_systemManager->keyHold(Key::S))
	{
		_cam->Move(-(4.0f * dt));
	}
	if(_systemManager->keyHold(Key::A))
	{
		_cam->Strafe(-(4.0f * dt));
	}
	if(_systemManager->keyHold(Key::D))
	{
		_cam->Strafe(4.0f * dt);
	}

	if (_systemManager->keyPressed(Key::Space))
	{
		ThreadsTest* threadsTest = new ThreadsTest();
		threadsTest->Init();

		sManager->ChangeState(threadsTest);
	}

	if(!escaped)
	{
		_systemManager->SetMousePosition(RenderManager::Instance()->GetWidth()/2,RenderManager::Instance()->GetHeight()/2);
	}
}

void SkyBoxTest::Update(GameManager* sManager)
{
	//delta time
	dt = _clock.getTime();
	_clock.Reset();
}
void SkyBoxTest::Draw(GameManager* sManager)
{
	_renderManager->StartFrame();
	_renderManager->ClearScreen();

	_renderManager->SetPerspective();
	_renderManager->UpdateCurrentCamera();

	_skyBox->SetPositionSize(_cam->m_vPosition,Vector3(1,1,1));
	_skyBox->Draw();

	_renderManager->SetOrtho();

	//draw fps
 	font->DrawText(_renderManager->GetWidth()/2,13,"SkyBox test",Aurora::Graphics::ALIGN_CENTER,Aurora::Graphics::RenderManager::RGBA(0x00, 0x00, 0x00, 0xff));
	
	font->DrawText(_renderManager->GetWidth()/2,_renderManager->GetHeight() - 2,"Press Space/Select to change test.",Aurora::Graphics::ALIGN_CENTER,Aurora::Graphics::RenderManager::RGBA(0x00, 0x00, 0x00, 0xff));
	
	_renderManager->EndFrame();
}

