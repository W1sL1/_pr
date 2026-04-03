#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>
#include <dos.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 5454///номер порта (любой не занятый порт)


int main() {
    WSADATA wsaData; // структура для хранения информации о конкретной реализации интерфейса Windows Sockets
    WORD DLLVersion = MAKEWORD(2, 2);
    if (WSAStartup(DLLVersion, &wsaData))  //Функция WSAStartup инициализирует WinSock. Эта функция всегда самая первая при начале работы с WinSock
        return -1;

    SOCKET start = socket(AF_INET, SOCK_STREAM, 0); // создание сокета, AF_INET для протокола  IP, для передачи данных через канал связи с TCP
    if (start == INVALID_SOCKET) {
        printf("Invalid socket\n");
        wprintf(L"ERROR: %ld\n", WSAGetLastError());
        return -1;
    }

    SOCKADDR_IN sockaddr; // Структура sockaddr_in описывает сокет для работы с протоколами IP
    memset(&sockaddr, 0, sizeof(sockaddr));///заполнение нулями
    sockaddr.sin_family = AF_INET;//указываем что работаем только с адресами типа Ipv4
    sockaddr.sin_port = htons(PORT);///номер порта который намерен занять процесс

    bind(start, (struct sockaddr*)&sockaddr, sizeof(sockaddr)); // привязка адреса к сокету
    listen(start, 1); // перевод созданного сокета в состояние, в котором он ожидает входящего соединения
    if (listen(start, 1) == SOCKET_ERROR) {
        printf("listen failed\n");
        closesocket(start);
        return -1;
    }

    SOCKADDR_IN klient_adr;
    int ka_size = sizeof(klient_adr);
    SOCKET klient_socket;

    printf("Waiting for connection...\n");

    char file_name[50];//массив для имени файла
    for (int i = 0; i < 50; i++)
        file_name[i] = 0;

    if (klient_socket = accept(start, (struct sockaddr*)&klient_adr, &ka_size))// ожидается входящее соединение
    {
        printf("Connection established\nEnter file name:\n");
        scanf("%s", file_name);
        send(klient_socket, file_name, sizeof(file_name), 0); // отправка имени файла клиенту 
        getch();
    }
    getch();
    printf("File delited\n");
    return EXIT_SUCCESS;
}
