#include <stdio.h>
#include <windows.h>
#include <thread>
#include <algorithm>
#include <time.h>
#include "Server.h"

#define SERVER_PORT 1170

Server::Server() {
    name = "MyServer";
    if(init() == -1)
        printf("Server initialization fail!\n");
}

Server::Server(std::string name) {
    this->name = name;
    if(init() == -1)
        printf("Server initialization fail!\n");
}

int Server::init() {
    int ret;

    wVersionRequested = MAKEWORD(2, 2);
    ret = WSAStartup(wVersionRequested, &wsaData);
    if(ret != 0) {
        printf("WSAStart failed!");
        return -1;
    }
    if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        WSACleanup();
        printf("Invalid Winsock version!\n");
        return -1;
    }
    sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sListen == INVALID_SOCKET) {
        WSACleanup();
        printf("socket() failed!\n");
        return -1;
    }
    saServer.sin_family = AF_INET;
    saServer.sin_port = htons(SERVER_PORT);
    saServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

    ret = bind(sListen, (struct sockaddr*)&saServer, sizeof(saServer));
    if(ret == SOCKET_ERROR) {
        printf("bind() failed!\n");
        closesocket(sListen);
        WSACleanup();
        return -1;
    }
    ret = listen(sListen, 5);
    if(ret == SOCKET_ERROR) {
        printf("listen() failed!\n");
        closesocket(sListen);
        WSACleanup();
        return -1;
    }

    printf("Waiting for client to connect!\n");
    return 0;
}

int Server::clear() {
    
    //close all socket
    for(std::vector<socketAndInfo>::iterator it = sServer.begin(); it != sServer.end(); ++it) {
        //send exit to all clients
        DataPackage dp;
        dp.isOver = (unsigned char)0x01;
        dp.type = (unsigned char)0x21; //exit
        dp.packageSize = sizeof(Header);
        send(it->socket, (char*)&dp, sizeof(dp), 0);

        closesocket(it->socket);
    }
    sServer.clear();

    //join all thread
    for(std::vector<std::thread>::iterator it = clientSet.begin(); it != clientSet.end(); ++it) {
        it->join();
    }
    clientSet.clear();

    closesocket(sListen);
    WSACleanup();
    printf("Server Stop!\n");
}

void Server::clientThread(socketAndInfo si) {
    int nLeft;
    char *ptr;
    DataPackage dp;
    while(1) {
        nLeft = sizeof(DataPackage);
        ptr = (char*)&dp;
        bool end = false;
        while(nLeft > 0) {
            ret = recv(sServer, ptr, nLeft, 0);
            if(ret == SOCKET_ERROR) {
                printf("recv() failed!\n");
                break;
            }

            if(ret == 0) {
                printf("Client has closed the connection!\n");
                //close socket
                std::vector<socketAndInfo>::iterator it =  find(sServer.begin(), sServer.end(), si);
                closesocket(it->socket);
                sSever.erase(it);
                end = true;
                break;
            }

            nLeft -= ret;
            ptr += ret;
        }
        if(end)
            break;

        switch(dp.type) {
            case 0x00:  //get time
            case 0x01:  //get server name
            case 0x02:  //get client list
            case 0x03:  //send message to another client
        }
    }
    
}

void Server::run() {
    //need to response "exit" option
    bool keepGoing = true;
    int length;
    SOCKET s;
    struct sockaddr_in sa;
    
    while(1) {
        length = sizeof(saClient);
        s = accept(sListen, (struct sockaddr*)&sa, &length);
        if(keepGoing == false)
            break;
        if(s == INVALID_SOCKET) {
            printf("Accept() failed!\n");
            continue;
        }
        printf("Accept client: %s: %d\n", inet_ntoa(sa.sin_addr), ntohs(sa.sin_port));
        
        //save client info and start a new thread to deal with this client's requestments
        socketAndInfo tmp(s, sa);
        sServer.push_back(tmp);
        std::thread th = std::thread(clientThread, tmp);    //new thread
        clientSet.push_back(th);
    }

    clear();
}