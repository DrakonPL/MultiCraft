#include <Aurora/Network/NetworkInputControllerClient.h>

namespace Aurora
{
	namespace Network
	{
		NetworkInputControllerClient::NetworkInputControllerClient(std::string clientName,int listenPort)
		{
			//name
			_clientName = clientName;

			//listening port
			_serverListeningPort = listenPort;

			//set searching servers socket to non blocking
			_serverSearcherSocket.SetBlocking(false);

			//bind it to correct port
			if(!_serverSearcherSocket.Bind(_serverListeningPort))
			{
				_controllerState = NOTBIND;
			}else
			{
				_controllerState = IDLE;
			}

			//clear servers info
			_foundServers.clear();
		}

		int NetworkInputControllerClient::NumberOfFoundServers()
		{
			return _foundServers.size();
		}

		NetworControllerState NetworkInputControllerClient::GetControllerState()
		{
			return _controllerState;
		}

		bool NetworkInputControllerClient::IsConnectedToServer()
		{
			return _controllerState == CONNECTED;
		}

		void NetworkInputControllerClient::Start()
		{
			//clear servers info
			_foundServers.clear();

			//switch to search mode
			_controllerState = SEARCHING;
		}

		void NetworkInputControllerClient::Stop()
		{

		}

		std::string NetworkInputControllerClient::GetServerNameAtPos(int pos)
		{
			return _foundServers[pos].Name;
		}

		void NetworkInputControllerClient::ConnectToServer(std::string serverName)
		{
			for (unsigned int i = 0; i < _foundServers.size();i++)
			{
				if (_foundServers[i].Name == serverName)
				{
					_defaulServerAddress = _foundServers[i].Address;
				}		
			}

			_controllerState = CONNECTING;
		}

		void NetworkInputControllerClient::Update(float dt)
		{
			if (_controllerState == SEARCHING)
			{
				Network::IPAddress senderIP;
				unsigned short senderPort;
				Network::Packet receivedPacket;

				if (_serverSearcherSocket.Receive(receivedPacket,senderIP,senderPort) == Network::Socket::Done)
				{
					//check if it's "Hello World" message from broadcasting server
					int packetType = -1;
					receivedPacket >> packetType;

					if (packetType == 20)//server broadcast
					{
						std::string serverName = "";
						receivedPacket >> serverName;

						//if there is no servers make first one default
						if (_foundServers.size() == 0)
						{
							_defaulServerAddress = senderIP;
						}

						//check if we found already this server
						bool serverFound = false;
						for (unsigned int i = 0; i < _foundServers.size();i++)
						{
							if (_foundServers[i].Name == serverName)
							{
								serverFound = true;
							}
						}

						if (!serverFound)
						{
							//add new one
							NetworkInputServerInfo serverInfo;
							serverInfo.Address = senderIP;
							serverInfo.Name = serverName;

							_foundServers.push_back(serverInfo);
						}
					}
				}
			}
			else if (_controllerState == CONNECTING)
			{
				//try to connect to server
				if (_clientSocket.Connect(_serverListeningPort, _defaulServerAddress) != Network::Socket::Done)
					_controllerState = NOTCONNECTED;
				else
				{
					_controllerState = WAITING;

					//send our name there :>
					Network::Packet namePacket;
					namePacket << 21 << _clientName;//21 login client with name packet

					_clientSocket.Send(namePacket);

					_clientSocket.SetBlocking(false);
				}
			}
			else if (_controllerState == WAITING)
			{
				//we are waiting for acknowledge message of fuck off message
				Network::Packet messagePacket;
				if (_clientSocket.Receive(messagePacket) == Network::Socket::Done)
				{
					int packetType = -1;
					messagePacket >> packetType;

					if (packetType == 22)//welcome  on server client
					{
						_controllerState = CONNECTED;
					}
					else if (packetType == 23)//fuck off
					{
						_controllerState = NOTCONNECTED;
						_clientSocket.Close();
						_serverSearcherSocket.Close();
					}else
					{
						//??
						_controllerState = NOTCONNECTED;
						_clientSocket.Close();
						_serverSearcherSocket.Close();
					}
				}
			}
			else if (_controllerState == CONNECTED)
			{

			}
		}
	}
}

