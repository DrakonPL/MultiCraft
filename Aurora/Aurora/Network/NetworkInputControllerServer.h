#ifndef NetworkInputControllerServer_H
#define NetworkInputControllerServer_H

#include <Aurora/Network/IPAddress.hpp>
#include <Aurora/Network/SocketUDP.hpp>
#include <Aurora/Network/SocketTCP.hpp>
#include <Aurora/Network/Selector.hpp>
#include <Aurora/Network/Packet.hpp>

#include <string>
#include <vector>

namespace Aurora
{
	namespace Network
	{
		class NetworkInputClientInfo
		{
		private:

			std::string _clinetName;
			Network::IPAddress _clientIp;
			Network::SocketTCP _clientSocket;
			Network::Packet _packet;

			bool _packetNew;
			bool _logged;

			void MarkPackedNew();

		public:

			NetworkInputClientInfo(std::string name,Network::IPAddress ip,Network::SocketTCP socket);

			std::string GetName();
			void SetName(std::string name);
			Network::Packet GetPacket();
			Network::IPAddress GetAddress();
			Network::SocketTCP GetSocket();

			void SetPacket(Network::Packet packet);
			void MarkPackedReaded();

			bool IsLogged() { return _logged;}
			void SetLogged(bool login) { _logged = login; }
		};

		class NetworkInputControllerServer
		{

		private:

			int _broadcastPort;
			float _broadcastTimer;
			std::string _broadcastName;

			Network::SocketUDP _broadcastSocket;
			Network::IPAddress _broadcadtAddress;

			Network::SocketTCP _serverSocket;
			Network::SelectorTCP _selector;

			std::vector<NetworkInputClientInfo*> _connectedClients;

			bool _serverRunning;

		public:

			NetworkInputControllerServer(std::string name,int serverPort);

			void Start();
			void Stop();
			void Update(float dt);

			bool ContainsClientWithAddress(Network::IPAddress address);
			int GetClientsCount();

			void RemoveClientWithSocket(Network::SocketTCP socket);

			NetworkInputClientInfo* GetClientByNumber(int i);
			NetworkInputClientInfo* GetClientByName(std::string name);
			NetworkInputClientInfo* GetClientBySocket(Network::SocketTCP socket);
			NetworkInputClientInfo* GetClientByAddress(Network::IPAddress address);
		};
	}
}

#endif