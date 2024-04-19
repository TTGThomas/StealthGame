#include "Server.h"

#include <winsock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")

void Server::Exec(int port, std::function<void(unsigned char*)> sendFunc, std::function<void(unsigned char*)> recvFunc)
{
    m_shouldTerminate = false;
    m_sendFunc = sendFunc;
    m_recvFunc = recvFunc;

    m_mainThread = std::thread(&Server::MainThread, this, port);
}

void Server::Terminate()
{
    m_shouldTerminate = true;

    for (int i = 0; i < m_clientIndex; i++)
    {
        m_recvThread[i].join();
        m_sendThread[i].join();
        closesocket(m_clientSockets[i]);
    }
    closesocket(m_socket);
    m_mainThread.join();
}

void Server::MainThread(int port)
{
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket == -1)
        std::cerr << "Socket creation failed." << std::endl;
    else
        std::cout << "Socket create success" << std::endl;

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(m_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        std::cerr << "Socket binding failed." << std::endl;
    else
        std::cerr << "Socket binding success." << std::endl;

    if (listen(m_socket, 128) == -1)
        std::cerr << "Socket listening failed." << std::endl;
    else
        std::cerr << "Socket listen success." << std::endl;

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    while (!m_shouldTerminate)
    {
        SOCKET clientSocket = accept(m_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (clientSocket == -1)
        {
            std::cerr << "Socket accepting failed: " << WSAGetLastError() << std::endl;
            if (WSAGetLastError() == WSAEINTR)
                return;
        }
        else
            std::cout << "Socket accepted successfully." << std::endl;

        char clientIP[32];
        std::cout << "Client IP: " << inet_ntop(AF_INET, &client_addr, clientIP, sizeof(clientIP)) << std::endl;

        m_sendThread[m_clientIndex] = std::thread(&Server::SendThread, this, clientSocket, m_clientIndex);
        m_recvThread[m_clientIndex] = std::thread(&Server::RecvThread, this, clientSocket, m_clientIndex);
        m_clientSockets[m_clientIndex] = clientSocket;
        m_clientIndex++;
    }
}

void Server::SendThread(int clientSocket, int index)
{
    while (!m_shouldTerminate)
    {
        unsigned char buffer[BUFSIZE + 1];
        buffer[0] = index;
        m_sendFunc(buffer);
        char out[BUFSIZE + 1];
        for (int i = 0; i < BUFSIZE + 1; i++)
            out[i] = buffer[i] - 128;
        send(clientSocket, out, sizeof(buffer), 0);
    }
}

void Server::RecvThread(int clientSocket, int index)
{
    while (!m_shouldTerminate)
    {
        char buffer[PATCHSIZE];
        int len = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (len > 0)
        {
            unsigned char in[PATCHSIZE + 1];
            in[0] = index;
            for (int i = 0; i < PATCHSIZE; i++)
                in[i + 1] = buffer[i] + 128;
            m_recvFunc(in);
        }
        else if (len == 0)
        {
            std::cout << "Client disconnected" << std::endl;
            return;
        }
        else
        {
            std::cout << "Recv failed: " << WSAGetLastError() << std::endl;
            return;
        }
    }
}