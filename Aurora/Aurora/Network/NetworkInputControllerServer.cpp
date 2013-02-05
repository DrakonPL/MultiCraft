#include <Aurora/Network/NetworkInputControllerServer.h>

namespace Aurora
{
	namespace Network
	{
		NetworkInputClientInfo::NetworkInputClientInfo(std::string name,Network::IPAddress ip,Network::SocketTCP socket)
		{
			_clinetName = name;
			_clientIp = ip;
			_clientSocket = socket;
			_packetNew = false;
			_logged = false;
		}

		void NetworkInputClientInfo::MarkPackedReaded()
		{
			_packetNew = false;
		}

		void NetworkInputClientInfo::MarkPackedNew()
		{
			_packetNew = true;
		}

		Network::Packet NetworkInputClientInfo::GetPacket()
		{
			return _packet;
		}

		void NetworkInputClientInfo::SetPacket(Network::Packet packet)
		{
			_packet = packet;

			MarkPackedNew();
		}

		Network::IPAddress NetworkInputClientInfo::GetAddress()
		{
			return _clientIp;
		}

		std::string NetworkInputClientInfo::GetName()
		{
			return _clinetName;
		}

		void  NetworkInputClientInfo::SetName(std::string name)
		{
			_clinetName = name;
		}

		Network::SocketTCP NetworkInputClientInfo::GetSocket()
		{
			return _clientSocket;
		}

		NetworkInputControllerServer::NetworkInputControllerServer(std::string name,int serverPort)
		{
			//broadcast data
			_broadcastName = name;
			_broadcastPort = serverPort;
			_broadcastTimer = 0;
			_serverRunning = false;

			//here i must get address of local network and make from it broadcast address
			_broadcadtAddress = Network::IPAddress("192.168.1.255");//last 255 to broadcast to whole sub network

			//server TCP socket and selector
			if (!_serverSocket.Listen(serverPort))
			{
				int erroro = 1;
			}

			//add server socket to selector
			_selector.Add(_serverSocket);

			//clear all clients
			_connectedClients.clear();
		}

		void NetworkInputControllerServer::Start()
		{
			_serverRunning = true;
		}

		void NetworkInputControllerServer::Stop()
		{
			_serverRunning = false;
		}

		bool NetworkInputControllerServer::ContainsClientWithAddress(Network::IPAddress address)
		{
			bool addressFound = false;

			for (unsigned int i = 0;i < _connectedClients.size();i++)
			{
				if (_connectedClients[i]->GetAddress() == address)
				{
					addressFound = true;
				}
			}

			return addressFound;
		}

		void NetworkInputControllerServer::RemoveClientWithSocket(Network::SocketTCP socket)
		{
			int foundNumber = -1;

			for (unsigned int i = 0;i < _connectedClients.size();i++)
			{
				if (_connectedClients[i]->GetSocket() == socket)
				{
					foundNumber = i;
				}
			}

			if (foundNumber != -1)
			{
				delete _connectedClients[foundNumber];
				_connectedClients.erase(_connectedClients.begin() + foundNumber);
			}
		}

		void NetworkInputControllerServer::Update(float dt)
		{
			if (_serverRunning)
			{
				//server info broadcasting broadcasting
				{
					_broadcastTimer += dt;
					if (_broadcastTimer > 2)//every 2 seconds
					{
						Network::Packet sendPacket;
						sendPacket << 20 << _broadcastName;//20 is packet number for server broadcasting

						if(_broadcastSocket.Send(sendPacket,_broadcadtAddress,_broadcastPort) != Network::Socket::Done)
						{
							//error
							int errorSending = 1;
						}
						_broadcastTimer = 0.0f;
					}
				}


				//clients support
				{
					// Get the sockets ready for reading
					unsigned int readySockets = _selector.Wait(0.01f);//10 milliseconds of timeout

					for (unsigned int i = 0; i < readySockets; i++)
					{
						//get ready socket
						Network::SocketTCP socket = _selector.GetSocketReady(i);

						if (socket == _serverSocket)//means that there is new connection to server
						{
							Network::IPAddress clientAddress;
							Network::SocketTCP clientSocket;

							//accept new connection
							_serverSocket.Accept(clientSocket, &clientAddress);

							//add to list of clients - default state is not logged
							NetworkInputClientInfo* newClient = new NetworkInputClientInfo("",clientAddress,clientSocket);
							_connectedClients.push_back(newClient);

							//add this socket to selector
							_selector.Add(clientSocket);

						}else //there is message from client
						{
							Network::Packet receivedPacket;
							if(socket.Receive(receivedPacket) == Network::Socket::Done)
							{
								//check message type
								int packetType = -1;
								receivedPacket >> packetType;

								//21 - client login attempt with name
								//22 - send client that he's welcome
								//23 - send client to fuck off

								if(packetType == 21)//client login attempt
								{
									//check if there is client with the same socket
									NetworkInputClientInfo *client = GetClientBySocket(socket);
									if (client != 0)
									{
										//now lets check if there is already player with the same name
										std::string newName = "";
										receivedPacket >> newName;

										NetworkInputClientInfo *client2 = GetClientByName(newName);
										if (client2 == 0)//no clients with the same name
										{
											//we leave this client 
											client->SetName(newName);
											client->SetLogged(true);

											//send login on message
											Network::Packet sendPacket;
											sendPacket << 22;
											socket.Send(sendPacket);
										}else
										{
											//there is already client with the same name

											//send fuck off message
											Network::Packet sendPacket;
											sendPacket << 23;
											socket.Send(sendPacket);

											RemoveClientWithSocket(socket);

											_selector.Remove(socket);
										}
									}
								}
							}
						}
					}
					//end client support

				}
			}
		}

		int NetworkInputControllerServer::GetClientsCount()
		{
			return _connectedClients.size();
		}

		NetworkInputClientInfo *NetworkInputControllerServer::GetClientByNumber(int i)
		{
			return _connectedClients[i];
		}

		NetworkInputClientInfo *NetworkInputControllerServer::GetClientByName(std::string name)
		{
			for (unsigned int i = 0;i < _connectedClients.size();i++)
			{
				if (_connectedClients[i]->GetName() == name)
				{
					return _connectedClients[i];
				}
			}

			return 0;
		}

		NetworkInputClientInfo* NetworkInputControllerServer::GetClientBySocket(Network::SocketTCP socket)
		{
			for (unsigned int i = 0;i < _connectedClients.size();i++)
			{
				if (_connectedClients[i]->GetSocket() == socket)
				{
					return _connectedClients[i];
				}
			}

			return 0;
		}

		NetworkInputClientInfo* NetworkInputControllerServer::GetClientByAddress(Network::IPAddress address)
		{
			for (unsigned int i = 0;i < _connectedClients.size();i++)
			{
				if (_connectedClients[i]->GetAddress() == address)
				{
					return _connectedClients[i];
				}
			}

			return 0;
		}
	}
}

