#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include "protocol.h"

#define SERVER_PORT 1170

typedef struct _student {
    char name[32];
    int age;
}student;

int main(int argc, char* argv[]) {
    WORD wVersionRequested;
    WSADATA wsaData;
    int ret;
    SOCKET sClient;
    struct sockaddr_in saServer;
    student stu;
    char* ptr = (char*)&stu;
    BOOL fSuccess = TRUE;

    if(argc != 2) {
        printf("usage: serverIP\n");
        exit(0);
    }

    wVersionRequested = MAKEWORD(2, 2);
    ret = WSAStartup(wVersionRequested, &wsaData);
    if(ret != 0) {
        printf("WSAStart failed!\n");
        exit(-1);
    }
    if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        WSACleanup();
        printf("Invalid Winsock version!\n");
        exit(-1);
    }
    sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sClient == INVALID_SOCKET) {
        WSACleanup();
        printf("socket() failed!\n");
        exit(-1);
    }
    saServer.sin_family = AF_INET;
    saServer.sin_port = htons(SERVER_PORT);
    saServer.sin_addr.S_un.S_addr = inet_addr(argv[1]);

    ret = connect(sClient, (struct sockaddr*)&saServer, sizeof(saServer));
    if(ret == SOCKET_ERROR) {
        printf("connect() failed!\n");
        closesocket(sClient);
        WSACleanup();
        exit(-1);
    }

    DataPackage dp;
    dp.header.type = (unsigned char)0x01;
    dp.header.isOver = (unsigned char)1;
    dp.header.dataSize = 0;
    ret = send(sClient, (char*)&dp, sizeof(dp), 0);
    if(ret == SOCKET_ERROR) {
        printf("send() failed!\n");
        closesocket(sClient);
        WSACleanup();
        exit(-1);
    }
    else {
        printf("student info has been sent!\n");
    }
    closesocket(sClient);
    system("pause");

    
    WSACleanup();

    
    return 0;
}