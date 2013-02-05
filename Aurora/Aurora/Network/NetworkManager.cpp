#include <Aurora/Network/NetworkManager.h>
#include <Aurora/Utils/PlatformConfigurator.h>

#ifdef AURORA_PC
#include <Aurora/Network/pc/PcNetworkManager.h>
#endif

#ifdef AURORA_PSP
#include <Aurora/Network/psp/PspNetworkManager.h>
#endif

#ifdef AURORA_IOS
#include <Aurora/Network/ios/IOSNetworkManager.h>
#endif

namespace Aurora
{
	namespace Network
	{
		NetworkManager* NetworkManager::_networkManager = 0;

		NetworkManager::NetworkManager()
		{


		}

		NetworkManager* NetworkManager::Instance()
		{
			if(_networkManager == 0)
			{
#ifdef AURORA_PC
				_networkManager = new PcNetworkManager();
#endif

#ifdef AURORA_PSP
				_networkManager = new PspNetworkManager();
#endif
                
#ifdef AURORA_IOS
                _networkManager = new IOSNetworkManager();
#endif                

			}

			return _networkManager;
		}

		bool NetworkManager::isConnectionActive()
		{
			return _connectionActive;
		}
	}
}

