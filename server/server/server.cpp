#define WIN32_LEAN_AND_MEAN
#pragma comment(lib, "Ws2_32.lib")

#include <iostream>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

int main()
{
    WSADATA wsaData;
    ADDRINFO* addResult;
    ADDRINFO hints;
    SOCKET ClientSocket = INVALID_SOCKET;
    SOCKET ListentSocket = INVALID_SOCKET;

    const char* sendBuffer = "Карандаш";
    char recvBuffer[512];

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result != 0) {
        std::cout << "WSAStartup failed with result " << result << std::endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    result = getaddrinfo(NULL, "666", &hints, &addResult);
    if (result != 0) {
        std::cout << "Getaddrinfo failed with error " << result << std::endl;
        WSACleanup();
        return 1;
    }

    ListentSocket = socket(addResult->ai_family, addResult->ai_socktype, addResult->ai_protocol);
    if (ListentSocket == INVALID_SOCKET) {
        std::cout << "Socket creation failed with error " << WSAGetLastError() << std::endl;
        freeaddrinfo(addResult);
        WSACleanup();
        return 1;
    }

    result = bind(ListentSocket, addResult->ai_addr, (int)addResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        std::cout << "Binding failed with error " << WSAGetLastError() << std::endl;
        closesocket(ListentSocket);
        freeaddrinfo(addResult);
        WSACleanup();
        return 1;
    }

    result = listen(ListentSocket, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        std::cout << "Listening failed with error " << WSAGetLastError() << std::endl;
        closesocket(ListentSocket);
        freeaddrinfo(addResult);
        WSACleanup();
        return 1;
    }

    ClientSocket = accept(ListentSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        std::cout << "Accepting connection failed with error " << WSAGetLastError() << std::endl;
        closesocket(ListentSocket);
        freeaddrinfo(addResult);
        WSACleanup();
        return 1;
    }

    closesocket(ListentSocket);

    do {
        ZeroMemory(recvBuffer, 512);
        result = recv(ClientSocket, recvBuffer, 512, 0);
        if (result > 0) {
            std::cout << "Received " << result << " bytes" << std::endl;
            std::cout << "Received data: " << recvBuffer << std::endl;

            result = send(ClientSocket, sendBuffer, (int)strlen(sendBuffer), 0);
            if (result == SOCKET_ERROR) {
                std::cout << "Failed to send data back with error " << WSAGetLastError() << std::endl;
                closesocket(ClientSocket);
                freeaddrinfo(addResult);
                WSACleanup();
                return 1;
            }
        }
        else if (result == 0)
            std::cout << "Connection closing..." << std::endl;
        else {
            std::cout << "Recv failed with error " << WSAGetLastError() << std::endl;
            closesocket(ClientSocket);
            freeaddrinfo(addResult);
            WSACleanup();
            return 1;
        }
    } while (result > 0);

    result = shutdown(ClientSocket, SD_SEND);
    if (result == SOCKET_ERROR) {
        std::cout << "Shutdown failed with error " << WSAGetLastError() << std::endl;
        closesocket(ClientSocket);
        freeaddrinfo(addResult);
        WSACleanup();
        return 1;
    }

    closesocket(ClientSocket);
    freeaddrinfo(addResult);
    WSACleanup();
    return 0;
}
