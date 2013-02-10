#include "PlayState.h"
#include "Utils/MathHelper.h"

void PlayState::Init()
{
	_fileManager =  FileManager::Instance();
	_renderManager = RenderManager::Instance();
	_systemManager = SystemManager::Instance();

	font = new TrueTypeFont("Assets/Minecraft/font.ttf",16);

	_skyBox = new SkyBox();
	_skyBox->LoadImage("Assets/SkyBox/0Panorama_back.png",Back);
	_skyBox->LoadImage("Assets/SkyBox/0Panorama_front.png",Front);
	_skyBox->LoadImage("Assets/SkyBox/0Panorama_left.png",Left);
	_skyBox->LoadImage("Assets/SkyBox/0Panorama_right.png",Right);
	_skyBox->LoadImage("Assets/SkyBox/0Panorama_up.png",Up);
	_skyBox->LoadImage("Assets/SkyBox/0Panorama_down.png",Down);

	cubeModel = new ModelObj2();
	cubeModel->Load("Assets/Models/Obj/textureCube.objb");
	cubeModel->UseFrustumCheck(false);
	//cubeModel->Optimize();
	//cubeModel->Save("textureCube.objb");
	cubeModel->CreateVertexBuffers();

	//game
	_playerCam = new Camera();
	_playerCam->PositionCamera(20,67,20,20,67,15,0,1,0);
	_renderManager->SetCurrentCam(_playerCam);

	_playerFrustum = new Frustum();

	_player = new Player("TestPlayer");
	_player->SetPosition(0,75,0);
	_player->SetSize(Vector3(0.2f,1.6f,0.2f),Vector3(0.2f,0.1f,0.2f));
	_player->SetPlayerCamera(_playerCam);
	_player->SetPlayerFrustum(_playerFrustum);
	_player->SetFlyState(true);

	//_chunkProvider = new ChunkProvider("./Save");

	_world = new World("TestWorld",666);
	_world->SetTextureSize(256,16);
	_world->SetChukSize(16,16);
	_world->SetPlayer(_player);
	//_world->SetChunkProvider(_chunkProvider);

	escaped = true;

	_renderTime = _updateTime = _inputTime = 0.0f;
	dt = 0.0f;
}

void PlayState::Enter()
{
	RenderManager::Instance()->SetOrtho();
	_clock.Reset();
}

void PlayState::CleanUp()
{
	delete _world;
}

void PlayState::Pause()
{

}

void PlayState::Resume()
{

}

void PlayState::GamePause()
{

}

void PlayState::GameResume()
{
	_world->SetAllChunksDirty();
}

void PlayState::HandleEvents(GameManager* sManager)
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
				_playerCam->RotateView(heading,0,1,0);
			}

			if (pith != 0.0f)
			{
				_playerCam->PitchView(pith);
			}
		}
	}

	if(_systemManager->keyHold(Key::Left))
	{
		_playerCam->RotateView(1.0f * dt,0,1,0);
	}
	if(_systemManager->keyHold(Key::Right))
	{
		_playerCam->RotateView(-(1.0f * dt),0,1,0);
	}
	if(_systemManager->keyHold(Key::Up))
	{
		_playerCam->PitchView(1.0f * dt);
	}
	if(_systemManager->keyHold(Key::Down))
	{
		_playerCam->PitchView(-(1.0f * dt));
	}

	_player->UpdateViewDirection();

	//move
	if(_systemManager->keyHold(Key::W))
	{
		_player->MoveForward();
	}
	if(_systemManager->keyHold(Key::S))
	{
		_player->MoveBackward();
	}
	if(_systemManager->keyHold(Key::A))
	{
		_player->StrafeLeft();
	}
	if(_systemManager->keyHold(Key::D))
	{
		_player->StrafeRight();
	}

	//jump
	if(_systemManager->keyPressed(Key::Space))
	{
		_player->Jump();
	}

	if(_systemManager->keyPressed(Key::Escape))
	{
		escaped = !escaped;
	}

	if(_systemManager->keyPressed(Key::F2))
	{
		_player->SetFlyState(!_player->GetFlyState());
	}

	//remove block
	if (_systemManager->keyPressed(Key::Q))
	{
		if (blockPicked)
		{
			_world->RemoveBlock(pickPos.x - 0.5f,pickPos.y - 0.5f,pickPos.z - 0.5f);
		}
	}

	//add block
	if (_systemManager->keyPressed(Key::E))
	{
		if (blockPicked)
		{
			_world->AddBlock(pickPos.x - 0.5f,pickPos.y - 0.5f,pickPos.z - 0.5f,Blocks::Stone);
		}
	}

	if(!escaped)
	{
		_systemManager->SetMousePosition(RenderManager::Instance()->GetWidth()/2,RenderManager::Instance()->GetHeight()/2);
	}

}

void PlayState::Update(GameManager* sManager)
{
	//delta time
	dt = _clock.getTime();
	_clock.Reset();

	_world->UpdateWorld(dt);
	_player->Update(dt);
}
void PlayState::Draw(GameManager* sManager)
{

	_renderManager->StartFrame();
	_renderManager->ClearScreen();

	_renderManager->SetPerspective();
	_renderManager->UpdateCurrentCamera();
	
	if (_playerCam->NeedUpdate())
	{
		_renderManager->ExtractFrustumPlanes(_playerFrustum);
		blockPicked = _world->CubePick(_playerCam->m_vPosition,_playerCam->GetViewDirection(),5.0f,0.25f,pickPos);
	}

	_skyBox->SetPositionSize(_playerCam->m_vPosition,Vector3(1,1,1));
	_skyBox->Draw();

	_world->FetchVisibleChunks();
	_world->DrawWorld(false);

	_renderManager->SetBlending(true);
	_world->DrawWorld(true);
	
	if (blockPicked)
	{
		cubeModel->SetPosition(pickPos);
		cubeModel->Draw();
	}

	_renderManager->SetBlending(false);


	//set text matrix
	_renderManager->SetOrtho();

	//name
	font->DrawText(_renderManager->GetWidth()/2,_renderManager->GetHeight() - 2,"MultiCraft test.",Aurora::Graphics::ALIGN_CENTER,Aurora::Graphics::RenderManager::RGBA(0x00, 0x00, 0x00, 0xff));

	//draw fps
	char deltaTime[30];
	sprintf(deltaTime,"dt: %f ", dt );
	font->DrawText(1,13,deltaTime,Aurora::Graphics::ALIGN_LEFT,Aurora::Graphics::RenderManager::RGBA(0x00, 0x00, 0x00, 0xff));

	//player chunk position
	char playerPos[30];
	sprintf(playerPos,"chunk position: %d %d",(int)_player->GetPosition().x / 16,(int)_player->GetPosition().z / 16);
	font->DrawText(1,26,playerPos,Aurora::Graphics::ALIGN_LEFT,Aurora::Graphics::RenderManager::RGBA(0x00, 0x00, 0x00, 0xff));

	//visible chunks
	char visibleChunks[30];
	sprintf(visibleChunks,"visible chunks: %d",_world->GetVisibleChunksNumber());
	font->DrawText(1,39,visibleChunks,Aurora::Graphics::ALIGN_LEFT,Aurora::Graphics::RenderManager::RGBA(0x00, 0x00, 0x00, 0xff));

	//on ground
	char groundText[30];
	bool onGround = _player->OnGround();
	onGround == true ? sprintf(groundText,"On ground") : sprintf(groundText,"In air");
	font->DrawText(1,52,groundText,Aurora::Graphics::ALIGN_LEFT,Aurora::Graphics::RenderManager::RGBA(0x00, 0x00, 0x00, 0xff));

	RenderManager::Instance()->EndFrame();
}

