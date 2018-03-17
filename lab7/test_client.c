#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>

#define SERVER_PORT 6666

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

    if(argc != 4) {
        printf("usage: serverIP name age\n");
        exit(0);
    }

    wVersionRequested = MAKEWORD(2, 2);
    ret = WSAStartup(wVersionRequested, &wsaData);
    if(ret != 0) {
        printf("WSAStart failed!");
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

    strcpy(stu.name, argv[2]);
    stu.age = atoi(argv[3]);
    ret = send(sClient, (char*)&stu, sizeof(stu), 0);
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
    WSACleanup();

    system("pause");
    return 0;
}