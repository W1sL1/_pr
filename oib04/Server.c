#define _CRT_SECURE_NO_WARNINGS     
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#define Error -1
#define Port 1234                     

int main() {
    WSADATA Ws;
    char Res[50] = { 0 };
    char FileName[100] = { 0 };

    if (WSAStartup(MAKEWORD(2, 2), &Ws)) {
        printf("Error WinSock version initialization\n");
        return Error;
    }

    SOCKET Socket = socket(AF_INET, SOCK_STREAM, 0);
    if (Socket == INVALID_SOCKET) {
        printf("Error initialization socket. Error: %d\n", WSAGetLastError());
        closesocket(Socket);
        WSACleanup();
        return Error;
    }

    SOCKADDR_IN SockAddr;
    memset(&SockAddr, 0, sizeof(SockAddr));
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_port = htons(Port);
    SockAddr.sin_addr.s_addr = INADDR_ANY;  // вместо 0
    
    if (bind(Socket, (struct sockaddr*)&SockAddr, sizeof(SockAddr)) == SOCKET_ERROR) {
        printf("Bind failed. Error code: %d\n", WSAGetLastError());
        closesocket(Socket);
        WSACleanup();
        return Error;
    }
    
    printf("Bind successful on port %d\n", Port);

    if (listen(Socket, 1) == SOCKET_ERROR) {
        printf("Listen failed. Error code: %d\n", WSAGetLastError());
        closesocket(Socket);
        WSACleanup();
        return Error;
    }
    
    printf("Listen successful. Waiting for connection...\n");

    SOCKET ClientSocket;
    SOCKADDR_IN ClientSockAddr;
    int ClientSASize = sizeof(ClientSockAddr);

    ClientSocket = accept(Socket, (struct sockaddr*)&ClientSockAddr, &ClientSASize);
    if (ClientSocket == INVALID_SOCKET) {
        printf("Accept failed. Error code: %d\n", WSAGetLastError());
        closesocket(Socket);
        WSACleanup();
        return Error;
    }
    
    printf("The connection is established\n");

    while (1) {
        memset(FileName, 0, sizeof(FileName));
        printf("Enter the file name. If you want to exit the program, write \"Exit\":\n");
        
        if(fgets(FileName, sizeof(FileName), stdin) == NULL) return EXIT_FAILURE;
        FileName[strcspn(FileName, "\n")] = 0;  // убираем \n
        
        if (strcmp(FileName, "Exit") == 0) {
            send(ClientSocket, "Exit", 5, 0);
            break;
        }
        
        send(ClientSocket, FileName, sizeof(FileName), 0);
        recv(ClientSocket, Res, sizeof(Res), 0);
        printf("%s\n", Res);
    }

    closesocket(ClientSocket);
    closesocket(Socket);
    WSACleanup();
    return 0;
}