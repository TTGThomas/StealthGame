#pragma once

#include <iostream>
#include <array>
#include <thread>
#include <functional>

#include "NetworkConfig.h"

class Server
{
public:
	Server() = default;

	void Exec(int port, std::function<void(unsigned char*)> sendFunc, std::function<void(unsigned char*)> recvFunc);
	void Terminate();
private:
	void MainThread(int port);
	void SendThread(int clientSocket, int index);
	void RecvThread(int clientSocket, int index);
private:
	int m_socket;
	std::function<void(unsigned char*)> m_sendFunc;
	std::function<void(unsigned char*)> m_recvFunc;
	bool m_shouldTerminate = false;

	int m_clientIndex = 0;

	std::thread m_mainThread;
	std::array<int, MAXPLAYERS> m_clientSockets;
	std::array<std::thread, MAXPLAYERS> m_sendThread;
	std::array<std::thread, MAXPLAYERS> m_recvThread;
};