#include <Aurora/Utils/GameLoader.h>
#include <Aurora/Utils/PlatformConfigurator.h>

#ifdef AURORA_PC
#include <Aurora/Utils/pc/WinGameLoader.h>
#endif

#ifdef AURORA_UNIX_MAC
#include <Aurora/Utils/pc/WinGameLoader.h>
#endif

#ifdef AURORA_PSP
#include <Aurora/Utils/psp/PspGameLoader.h>
#endif

#ifdef AURORA_IOS
#include <Aurora/Utils/ios/IOSGameLoader.h>
#endif

#ifdef AURORA_PS3
#include <Aurora/Utils/ps3/PS3GameLoader.h>
#endif

namespace Aurora
{
	namespace Utils
	{
		GameLoader::GameLoader(GameManager* gameManager)
		{
			_gameManager = gameManager;
		}

		GameLoader* GameLoader::getGameLoader(GameManager* gameManager)
		{
			GameLoader* _loader = 0;

			#ifdef AURORA_PC
			_loader = new WinGameLoader(gameManager);
			#endif

			#ifdef AURORA_UNIX_MAC
			_loader = new WinGameLoader(gameManager);
			#endif

			#ifdef AURORA_PSP
			_loader = new PspGameLoader(gameManager);
			#endif
            
            #ifdef AURORA_IOS
			_loader = new IOSGameLoader(gameManager);
            #endif
			
			 #ifdef AURORA_PS3
			_loader = new PS3GameLoader(gameManager);
            #endif
			
			return _loader;
		}
	}
}

