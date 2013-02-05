#ifndef PcNetworkManager_H
#define PcNetworkManager_H

#include <Aurora/Network/NetworkManager.h>

namespace Aurora
{
	namespace Network
	{
		class  PcNetworkManager : public NetworkManager
		{
		public:

			PcNetworkManager();

			bool Init();
		};
	}
}

#endif