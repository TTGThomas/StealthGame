#include "NetworkManager.h"

#include <winsock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")

void NetworkManager::InitServer(int port)
{
	m_role = Role::SERVER;

	WSAData wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) == -1)
		std::cerr << "Fail Initialize WSA" << std::endl;

	for (auto& vec : m_sendDatas)
		for (unsigned char& val : vec)
			val = 0;

	m_server.Exec(port,
		[this](unsigned char* buffer)
		{
			// Send
			int index = 1; // index0 is clientIndex
			for (std::array<unsigned char, PATCHSIZE>& vec : m_sendDatas)
			{
				for (int i = 0; i < PATCHSIZE; i++)
				{
					buffer[index] = vec[i];
					index++;
				}
			}
		},

		[this](unsigned char* buffer)
		{
			// Recv
			for (int i = 0; i < PATCHSIZE; i++)
				m_recvDatas[(int)buffer[0]][i] = buffer[i + 1];
		}
		);
}

void NetworkManager::InitClient(int port, const char* ip)
{
	m_role = Role::CLIENT;

	WSAData wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) == -1)
		std::cerr << "Fail Initialize WSA" << std::endl;

	for (unsigned char& val : m_sendDatas[0])
			val = 0;

	m_client.Exec(port, ip,
		[this](unsigned char* buffer)
		{
			// Send
			for (int i = 0; i < PATCHSIZE; i++)
				buffer[i] = m_sendDatas[0][i];
		},

		[this](unsigned char* buffer)
		{
			// Recv

			m_clientIndex = buffer[0];
			for (int j = 0; j < MAXPLAYERS; j++)
			{
				for (int i = 0; i < PATCHSIZE; i++)
				{
					m_recvDatas[j][i] = buffer[j * PATCHSIZE + i + 1];// 0 is clientIndex
				}
			}
		}
	);
}

void NetworkManager::Cleanup()
{
	WSACleanup();
}

void NetworkManager::ServerUpdateSendData(std::array<std::array<unsigned char, PATCHSIZE>, MAXPLAYERS>& datas)
{
	if (m_role == Role::SERVER)
		m_sendDatas = datas;
}

void NetworkManager::ClientUpdateSendData(std::array<unsigned char, PATCHSIZE>& datas)
{
	if (m_role == Role::CLIENT)
		m_sendDatas[0] = datas;
}

std::array<std::array<unsigned char, PATCHSIZE>, MAXPLAYERS>& NetworkManager::GetRecvData()
{
	return m_recvDatas;
}

void NetworkManager::TerminateSessions()
{
	if (m_role == Role::SERVER)
		m_server.Terminate();
	else if (m_role == Role::CLIENT)
		m_client.Terminate();
}
