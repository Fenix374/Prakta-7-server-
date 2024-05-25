#define WIN32_LEAN_AND_MEAN
#pragma comment(lib, "Ws2_32.lib")

#include <iostream>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

int main()
{
    
    WSADATA wsaData;
    ADDRINFO* addResult;
    ADDRINFO hints;
    SOCKET ConnectSocket = INVALID_SOCKET;
    const char* sendBuffer1 = "MiyaGi - Message 1";
    const char* sendBuffer2 = "Spotify - Message 2";
    char recvBuffer[512];

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result != 0) {
        cout << "WSAStartup failed with result " << result << endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    result = getaddrinfo("localhost", "666", &hints, &addResult);
    if (result != 0) {
        cout << "Getaddrinfo failed with error " << result << endl;
        WSACleanup();
        return 1;
    }

    ConnectSocket = socket(addResult->ai_family, addResult->ai_socktype, addResult->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Socket creation failed with error " << WSAGetLastError() << endl;
        freeaddrinfo(addResult);
        WSACleanup();
        return 1;
    }

    result = connect(ConnectSocket, addResult->ai_addr, (int)addResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        cout << "Unable to connect to server, error " << WSAGetLastError() << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addResult);
        WSACleanup();
        return 1;
    }
    result = send(ConnectSocket, sendBuffer1, (int)strlen(sendBuffer1), 0);
    if (result == SOCKET_ERROR) {
        cout << "Send failed with error " << WSAGetLastError() << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addResult);
        WSACleanup();
        return 1;
    }
    cout << "Data sent successfully: " << result << " bytes" << endl;

    // Send the second message
    result = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0);
    if (result == SOCKET_ERROR) {
        cout << "Send failed with error " << WSAGetLastError() << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addResult);
        WSACleanup();
        return 1;
    }
    cout << "Data sent successfully: " << result << " bytes" << endl;

    result = shutdown(ConnectSocket, SD_SEND);
    if (result == SOCKET_ERROR) {
        cout << "Shutdown failed with error " << WSAGetLastError() << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addResult);
        WSACleanup();
        return 1;
    }

    do {
        ZeroMemory(recvBuffer, 512);
        result = recv(ConnectSocket, recvBuffer, 512, 0);
        if (result > 0) {
            cout << "Received " << result << " bytes" << endl;
            cout << "Received data: " << recvBuffer << endl;
        }
        else if (result == 0)
            cout << "Connection closed" << endl;
        else {
            cout << "Recv failed with error " << WSAGetLastError() << endl;
            closesocket(ConnectSocket);
            freeaddrinfo(addResult);
            WSACleanup();
            return 1;
        }
    } while (result > 0);

    closesocket(ConnectSocket);
    freeaddrinfo(addResult);
    WSACleanup();
    return 0;
}
