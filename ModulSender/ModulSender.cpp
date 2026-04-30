#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iphlpapi.h>
#include <conio.h>

#define DEFAULT_PORT "35428";
#define DEFAULT_BUFLEN 512;
#pragma comment(lib, "Ws2_32.lib")
;
SOCKET ConnectSocket = INVALID_SOCKET;
int iResult;
WSADATA wsaData;

int Connect() {

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cout << WSAGetLastError() << std::endl;
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    char a[255] = "192.168.0.131";
    /*std::cin >> a;*/

    iResult = getaddrinfo(a, "35428", &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    ptr = result;
    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
        return 1;
    }

    freeaddrinfo(result);
    if (ConnectSocket == INVALID_SOCKET) {
        std::cout << WSAGetLastError() << std::endl;
        //printf("Не получилось подключится к серверу!\n");
        int a = _getch();
        return 1;
    }
    std::cout << "SuperOK" << std::endl;
    int timeout = 100;
    setsockopt(ConnectSocket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
}


int main()
{
    setlocale(0, "rus");
    if (Connect() == 1) {
        std::cout << WSAGetLastError() << std::endl;
        _getch();
    };
    std::cout << "OK" << std::endl;
    char line[2048] = "test,host=pc1 value=\"123456asd\"\n";
    iResult = send(ConnectSocket, line, (int)strlen(line), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }


    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    closesocket(ConnectSocket);
    WSACleanup();
    _getch();
    return 0;
}
