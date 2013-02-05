#ifndef NetworkManager_H
#define NetworkManager_H

namespace Aurora
{
	namespace Network
	{
		class NetworkManager
		{
		protected:

			bool _connectionActive;

			static NetworkManager *_networkManager;
			
		public:

			NetworkManager();

			static NetworkManager* Instance();

			bool isConnectionActive();

		public:

			virtual bool Init() = 0;
		};
	}
}

#endif