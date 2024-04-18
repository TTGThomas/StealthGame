#pragma once

#include <iostream>
#include <array>
#include <thread>

#include "Server.h"
#include "Client.h"

#include "NetworkConfig.h"

// 0   - Player index
// > 0 - User data

class NetworkManager
{
public:
	enum class Role
	{
		NONE, SERVER, CLIENT
	};
public:
	NetworkManager() = default;

	void InitServer(int port);
	void InitClient(int port, const char* ip);
	void Cleanup();

	// Server
	void ServerUpdateSendData(std::array<std::array<unsigned char, PATCHSIZE>, MAXPLAYERS>& datas);

	// Client
	void ClientUpdateSendData(std::array<unsigned char, PATCHSIZE>& datas);
	int ClientGetIndex() { return m_clientIndex; }

	// Both
	void TerminateSessions();
	std::array<std::array<unsigned char, PATCHSIZE>, MAXPLAYERS>& GetRecvData();

	Role GetRole() { return m_role; }
private:
	Role m_role = Role::NONE;
	// recvData is same for server and client
	std::array<std::array<unsigned char, PATCHSIZE>, MAXPLAYERS> m_recvDatas;
	// endData is in n dimension as a server while is is 1 dimension as a client
	std::array<std::array<unsigned char, PATCHSIZE>, MAXPLAYERS> m_sendDatas;

	Server m_server;
	Client m_client;

	int m_clientIndex = 0;
};

