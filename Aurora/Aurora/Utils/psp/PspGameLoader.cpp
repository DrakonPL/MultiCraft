#include <Aurora/Utils/psp/PspGameLoader.h>
#include <Aurora/Utils/psp/PspCommon.h>
#include <Aurora/Graphics/RenderManager.h>


#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspgu.h>
#include <pspgum.h>
#include <psppower.h>

namespace Aurora
{
	namespace Utils
	{
		PspGameLoader::PspGameLoader(GameManager* gameManager):GameLoader(gameManager)
		{

		}

		void PspGameLoader::Run()
		{
			//setup psp exit callback
			SetupCallbacks();
			
			scePowerSetClockFrequency(333, 333, 166);
			
			//configure
			_gameManager->Configure();

			//create window
			
			Aurora::Graphics::RenderManager::Instance()->Init();

			_gameManager->Init();

			while (_gameManager->Running())
			{
				_gameManager->HandleEvents();
				_gameManager->Update();
				_gameManager->Draw();
			}
		}
	}
}
