#include <stdio.h>
#include <windows.h>
#include "Server.h"

#define SERVER_PORT 1170

Server::Server() {
    name = "MyServer";
    init();
}

Server::Server(std::string name) {
    this->name = name;
    init();
}

void Server::init() {
    int ret;

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
    printf("Waiting for client to connect!\n");
}

void Server::clientThread() {
    while(1) {

    }
}

void Server::run() {
    int length;
    SOCKET s;
    struct sockaddr_in sa;
    
    while(1) {
        length = sizeof(saClient);
        s = accept(sListen, (struct sockaddr*)&sa, &length);
        if(s == INVALID_SOCKET) {
            printf("Accept() failed!\n");
            continue;
        }
        printf("Accept client: %s: %d\n", inet_ntoa(sa.sin_addr), ntohs(sa.sin_port));
    }
}