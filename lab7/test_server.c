#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>

#define SERVER_PORT 6666

typedef struct _student {
    char name[32];
    int age;
}student;

int main() {
    WORD wVersionRequested;
    WSADATA wsaData;
    int ret, nLeft, length;
    SOCKET sListen, sServer;
    struct sockaddr_in saServer, saClient;
    student stu;
    char *ptr;

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
    sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sListen == INVALID_SOCKET) {
        WSACleanup();
        printf("socket() failed!\n");
        exit(-1);
    }
    saServer.sin_family = AF_INET;
    saServer.sin_port = htons(SERVER_PORT);
    saServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

    ret = bind(sListen, (struct sockaddr*)&saServer, sizeof(saServer));
    if(ret == SOCKET_ERROR) {
        printf("bind() failed!\n");
        closesocket(sListen);
        WSACleanup();
        exit(-1);
    }
    ret = listen(sListen, 5);
    if(ret == SOCKET_ERROR) {
        printf("listen() failed!\n");
        closesocket(sListen);
        WSACleanup();
        exit(-1);
    }

    printf("Waiting for client connecting.\n");
    // printf("IP:%s, Port:%d\n", inet_ntoa(saServer.sin_addr), ntohs(saServer.sin_port));
    printf("Tips: Ctrl+c to quit!\n");
    
    length = sizeof(saClient);
    sServer = accept(sListen, (struct sockaddr*)&saClient, &length);
    if(sServer == INVALID_SOCKET) {
        printf("accept() failed!\n");
        closesocket(sListen);
        WSACleanup();
        exit(-1);
    }
    printf("Accept client: %s: %d\n", inet_ntoa(saClient.sin_addr), ntohs(saClient.sin_port));

    nLeft = sizeof(stu);
    ptr = (char*)&stu;
    while(nLeft > 0) {
        ret = recv(sServer, ptr, nLeft, 0);
        if(ret == SOCKET_ERROR) {
            printf("recv() failed!\n");
            break;
        }

        if(ret == 0) {
            printf("Client has closed the connection!\n");
            break;
        }
        
        nLeft -= ret;
        ptr += ret;
    }

    if(!nLeft) {
        printf("name: %s\t age:%d\n", stu.name, stu.age);
    }

    closesocket(sServer);
    closesocket(sListen);
    WSACleanup();

    system("pause");
    return 0;
}