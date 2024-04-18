#include "Client.h"

#include <winsock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")

void Client::Exec(int port, const char* ip, std::function<void(unsigned char*)> sendFunc, std::function<void(unsigned char*)> recvFunc)
{
    m_shouldTerminate = false;
    m_sendFunc = sendFunc;
    m_recvFunc = recvFunc;

    MainThread(port, ip);
}

void Client::Terminate()
{
    m_shouldTerminate = true;
    m_sendThread.join();
    m_recvThread.join();
    closesocket(m_serverSocket);
}

void Client::MainThread(int port, const char* ip)
{
    m_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_serverSocket == -1)
        std::cerr << "Socket creation failed." << std::endl;
    else
        std::cout << "Socket create success" << std::endl;

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server_addr.sin_addr.s_addr);
    int ret = connect(m_serverSocket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (ret == -1)
        std::cerr << "Socket connect failed" << std::endl;
    else
        std::cerr << "Socket connect success" << std::endl;

    m_sendThread = std::thread(&Client::SendThread, this);
    m_recvThread = std::thread(&Client::RecvThread, this);
}

void Client::SendThread()
{
    while (!m_shouldTerminate)
    {
        unsigned char buffer[1024];
        m_sendFunc(buffer);
        char out[1024];
        for (int i = 0; i < 1024; i++)
            out[i] = buffer[i] - 128;
        send(m_serverSocket, out, sizeof(buffer), 0);
    }
}

void Client::RecvThread()
{
    while (!m_shouldTerminate)
    {
        char buffer[1025];
        int len = recv(m_serverSocket, buffer, sizeof(buffer), 0);
        if (len > 0)
        {
            unsigned char in[1025];
            for (int i = 0; i < 1025; i++)
                in[i] = buffer[i] + 128;
            m_recvFunc(in);
        }
        else if (len == 0)
        {
            std::cout << "Server disconnected" << std::endl;
            return;
        }
        else
        {
            std::cout << "Recv failed: " << WSAGetLastError() << std::endl;
            return;
        }
    }
}