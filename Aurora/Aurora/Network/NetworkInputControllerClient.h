#ifndef NetworkInputControllerClient_H
#define NetworkInputControllerClient_H

#include <Aurora/Network/IPAddress.hpp>
#include <Aurora/Network/SocketUDP.hpp>
#include <Aurora/Network/SocketTCP.hpp>
#include <Aurora/Network/Packet.hpp>

#include <string>
#include <vector>

namespace Aurora
{
	namespace Network
	{
		enum NetworControllerState
		{
			IDLE,
			SEARCHING,
			CONNECTING,
			CONNECTED,
			NOTCONNECTED,
			WAITING,
			NOTBIND
		};

		class NetworkInputServerInfo
		{
		public:

			std::string Name;
			Network::IPAddress Address;
		};

		class NetworkInputControllerClient
		{
		private:

			NetworControllerState _controllerState;

			std::vector<NetworkInputServerInfo> _foundServers;

			int _serverListeningPort;
			Network::SocketUDP _serverSearcherSocket;

			std::string _clientName;
			Network::SocketTCP _clientSocket;
			Network::IPAddress _defaulServerAddress;

		public:

			NetworkInputControllerClient(std::string clientName,int listenPort);

			int NumberOfFoundServers();
			NetworControllerState GetControllerState();

			std::string GetServerNameAtPos(int pos);
			void ConnectToServer(std::string serverName);
			bool IsConnectedToServer();

			void Start();
			void Stop();
			void Update(float dt);
		};
	}
}

#endif
