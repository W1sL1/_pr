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
		printf("Error WinSock version initializaion");
		return Error;
	}
	SOCKET Socket=socket(AF_INET, SOCK_STREAM,0); 
	if (Socket == INVALID_SOCKET) {
		printf("Error initialization socket");
		closesocket(Socket);
		return Error;
	}
	SOCKADDR_IN SockAddr;
	memset(&SockAddr, 0, sizeof(SockAddr));
	SockAddr.sin_family = AF_INET; 
	SockAddr.sin_port = htons(Port);
	bind(Socket, (struct sockaddr*)&SockAddr, sizeof(SockAddr));
	if (listen(Socket, 1) == SOCKET_ERROR) {
		printf("Listen failed\n");
		closesocket(Socket);
		return Error;
	}
	SOCKET ClientSocket;
	SOCKADDR_IN ClientSockAddr;
	int ClientSASize = sizeof(ClientSockAddr);
	printf("Waiting for connection...\n");
	if (ClientSocket = accept(Socket, (struct sockaddr*)&ClientSockAddr, &ClientSASize)) {
		printf("The connection is established\n");
	}
	while (1) {
		memset(FileName, 0, sizeof(FileName));
		printf("Enter the file name. If you want to exit the program, write \"Exit\":\n");
		if(gets(FileName)==0) return EXIT_FAILURE;
		if (strcmp(FileName, "Exit") == 0) {
			send(ClientSocket, "Exit", 5, 0);
			break;
		}
		send(ClientSocket, FileName, sizeof(FileName), 0);
		recv(ClientSocket, Res, sizeof(Res), 0);
		printf("%s\n", Res);
	}
	closesocket(Socket);
	closesocket(ClientSocket);
	return 0;
}
