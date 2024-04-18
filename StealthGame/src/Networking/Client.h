#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <functional>

#include "NetworkConfig.h"

class Client
{
public:
	Client() = default;

	void Exec(int port, const char* ip, std::function<void(unsigned char*)> sendFunc, std::function<void(unsigned char*)> recvFunc);
	void Terminate();
private:
	void MainThread(int port, const char* ip);
	void SendThread();
	void RecvThread();
private:
	int m_serverSocket;
	std::function<void(unsigned char*)> m_sendFunc;
	std::function<void(unsigned char*)> m_recvFunc;

	std::thread m_sendThread;
	std::thread m_recvThread;

	bool m_shouldTerminate = false;
};