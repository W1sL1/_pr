#define _CRT_SECURE_NO_WARNINGS     
#define _WINSOCK_DEPRECATED_NO_WARNINGS  // разрешает использовать устаревшие winsock-функции
#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")   // подключаем библиотеку сокетов Windows
#define Error -1
#define Port 1234                     

int main() {
    WSADATA Ws;
    char Res[50] = { 0 };             // буфер для ответа от клиента (успех/ошибка удаления)
    char FileName[100] = { 0 };       // буфер для имени файла, который введёт пользователь

    // Инициализация Winsock (версия 2.2)
    if (WSAStartup(MAKEWORD(2, 2), &Ws)) {
        printf("Error WinSock version initializaion");
        return Error;
    }

    // Создание TCP-сокета
    SOCKET Socket = socket(AF_INET, SOCK_STREAM, 0); 
    if (Socket == INVALID_SOCKET) {
        printf("Error initialization socket");
        closesocket(Socket);
        return Error;
    }

    // Настройка адреса сервера: IPv4, порт 1234, любой доступный IP
    SOCKADDR_IN SockAddr;
    memset(&SockAddr, 0, sizeof(SockAddr));
    SockAddr.sin_family = AF_INET; 
    SockAddr.sin_port = htons(Port);
    // bind привязан к сокету
    bind(Socket, (struct sockaddr*)&SockAddr, sizeof(SockAddr));

    // Переводим сокет в режим ожидания подключений (очередь до 1 клиента)
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

    // Основной цикл общения с клиентом
    while (1) {
        memset(FileName, 0, sizeof(FileName));
        printf("Enter the file name. If you want to exit the program, write \"Exit\":\n");
        
        if(gets(FileName) == 0) return EXIT_FAILURE;
        
        if (strcmp(FileName, "Exit") == 0) {
            // Отправляем команду завершения (5 байт: 'E','x','i','t','\0')
            send(ClientSocket, "Exit", 5, 0);
            break;
        }
        
        // Отправляем имя файла (всегда 100 байт, даже если строка короче)
        send(ClientSocket, FileName, sizeof(FileName), 0);
        
        // Ждём ответ от клиента (успех/ошибка удаления)
        recv(ClientSocket, Res, sizeof(Res), 0);
        printf("%s\n", Res);
    }

    // Закрываем оба сокета (серверный и клиентский)
    closesocket(Socket);
    closesocket(ClientSocket);
    return 0;
}