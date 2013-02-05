#include <Aurora/Utils/pc/WinGameLoader.h>
#include <Aurora/Graphics/RenderManager.h>
#include <Aurora/System/SystemManager.h>
#include <Aurora/System/pc/PcSystemManager.h>

#include <SFML/Window.hpp>

namespace Aurora
{
	namespace Utils
	{
		WinGameLoader::WinGameLoader(GameManager* gameManager):GameLoader(gameManager)
		{

		}

		void WinGameLoader::Run()
		{
			sf::Window *_window = &((Aurora::System::PcSystemManager*)Aurora::System::SystemManager::Instance())->_window;

			//_window->Create(sf::VideoMode(Aurora::Graphics::RenderManager::Instance()->GetWidth(), Aurora::Graphics::RenderManager::Instance()->GetHeight(), 32), "Starter");
			_window->Create(sf::VideoMode(640, 480, 32), "Starter");
			_window->UseVerticalSync(Aurora::Graphics::RenderManager::Instance()->GetVsync());
			_window->ShowMouseCursor(false);

			_gameManager->Configure();

			Aurora::Graphics::RenderManager::Instance()->Init();
			((Aurora::System::PcSystemManager*)Aurora::System::SystemManager::Instance())->SetInput();

			_gameManager->Init();

			while (_window->IsOpened())
			{
				sf::Event Event;
				while (_window->GetEvent(Event))
				{
					if (Event.Type == sf::Event::Closed)
						_window->Close();

					if (Event.Type == sf::Event::Resized)
						Aurora::Graphics::RenderManager::Instance()->SetSize(Event.Size.Width,Event.Size.Height);
				}

				_gameManager->HandleEvents();
				_gameManager->Update();
				_gameManager->Draw();

				_window->Display();
			}

			_gameManager->CleanUp();
		}

	}
}
