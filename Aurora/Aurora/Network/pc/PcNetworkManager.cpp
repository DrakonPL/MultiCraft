#include <Aurora/Network/pc/PcNetworkManager.h>

namespace Aurora
{
	namespace Network
	{
		PcNetworkManager::PcNetworkManager()
		{
			_connectionActive = true;
		}

		bool PcNetworkManager::Init()
		{
			return true;
		}
	}
}

