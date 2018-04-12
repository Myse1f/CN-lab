//Created By Yufan Wu
//2018 3 27

#include "Server.h"
#include "protocol.h"
#include <stdio.h>
#include <windows.h>
#include <algorithm>
#include <time.h>
#include <conio.h>
#include "mingw.thread.h"

#define SERVER_PORT 1170

void listenFromClient(Server *s, socketAndInfo &si) {
    s->clientThread(si);
}

void listenFromKeyboard(Server *s) {
    s->stop();
}

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
        printf("WSAStart failed!\n");
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

    return 0;
}

void Server::clear() {
    
    //close all socket
    for(std::vector<socketAndInfo>::iterator it = sServer.begin(); it != sServer.end(); ++it) {
        //send exit to all clients
        DataPackage dp;
        dp.header.isOver = (unsigned char)0x01;
        dp.header.type = (unsigned char)0x21; //exit
        dp.header.dataSize = 0;
        send(it->socket, (char*)&dp, sizeof(dp), 0);

        closesocket(it->socket);
    }
    sServer.clear();

    //join all thread
    for(std::vector<std::thread*>::iterator it = clientSet.begin(); it != clientSet.end(); ++it) {
        (*it)->join();
    }
    clientSet.clear();
    keyboardThread->join();

    closesocket(sListen);
    WSACleanup();
    printf("%s Server Stop!\n", getTime());
}

void Server::clientThread(socketAndInfo &si) {
    //printf("debug2\n");

    int nLeft, ret;
    char *ptr;
    DataPackage dp;

    while(keepGoing) {
        nLeft = sizeof(DataPackage);
        ptr = (char*)&dp;
        bool end = false;

        while(nLeft > 0) {
            ret = recv(si.socket, ptr, nLeft, 0);
            //printf("debug ret=%d\n", ret);
            if(ret == SOCKET_ERROR || ret == 0) {
                printf("%s Client %s:%d has closed the connection!\n", getTime(), si.client.IPaddress, si.client.port);
                //close socket
                std::vector<socketAndInfo>::iterator it =  find(sServer.begin(), sServer.end(), si);
                closesocket(it->socket);
                sServer.erase(it);
                end = true;
                break;
            }

            // if(ret == 0) {
            //     printf("%s Client has closed the connection!\n", getTime());
            //     //close socket
            //     std::vector<socketAndInfo>::iterator it =  find(sServer.begin(), sServer.end(), si);
            //     closesocket(it->socket);
            //     sServer.erase(it);
            //     end = true;
            //     break;
            // }

            nLeft -= ret;
            ptr += ret;
        }
        if(end)
            break;

        DataPackage dpSend; //dataPackage to be sent
        switch(dp.header.type) {
            //printf("1\n");
            case 0x00:  //get time
            {
                printf("%s Client %s:%d require for the server time!\n", getTime(), si.client.IPaddress, si.client.port);
                time_t t;
                //static int k = 0;
                struct tm *pTime;
                /******************************************************************************
                * struct tm
                * {
                *     int tm_sec;   // seconds after the minute - [0, 60] including leap second
                *     int tm_min;   // minutes after the hour - [0, 59]
                *     int tm_hour;  // hours since midnight - [0, 23]
                *     int tm_mday;  // day of the month - [1, 31]
                *     int tm_mon;   // months since January - [0, 11]
                *     int tm_year;  // years since 1900
                *     int tm_wday;  // days since Sunday - [0, 6]
                *     int tm_yday;  // days since January 1 - [0, 365]
                *     int tm_isdst; // daylight savings time flag
                * };
                *******************************************************************************/
                time(&t);
                pTime = localtime(&t);
                dpSend.header.isOver = (unsigned char)1;
                dpSend.header.type = (unsigned char)0x10;
                memcpy(dpSend.data, pTime, sizeof(struct tm));
                dpSend.header.dataSize = (unsigned short)sizeof(struct tm);
                //printf("%lu %d %d\n", dpSend.header.dataSize, dpSend.header.isOver, dpSend.header.type);
                send(si.socket, (char*)&dpSend, sizeof(dpSend), 0); //send time infomation
                //printf("%d sent!\n", ++k);
                break;
            }
            case 0x01:  //get server name
            {
                printf("%s Client %s:%d require for the server name!\n", getTime(), si.client.IPaddress, si.client.port);
                dpSend.header.isOver = (unsigned char)1;
                dpSend.header.type = (unsigned char)0x11;
                memcpy(dpSend.data, name.c_str(), name.length());
                dpSend.header.dataSize = (unsigned short)(name.length());
                send(si.socket, (char*)&dpSend, sizeof(dpSend), 0); //send server name
                break;
            }
            case 0x02:  //get client list
            {
                printf("%s Client %s:%d require for the client list!\n", getTime(),  si.client.IPaddress, si.client.port);
                char *ptr = dpSend.data;
                int total = 0;
                std::vector<socketAndInfo>::iterator it = sServer.begin();
                
                while(it != sServer.end()) {
                    total = 0;
                    for( ; it!=sServer.end(); ++it) {
                        if(total + sizeof(ClientInfo) <= PACKAGE_DATA_SIZE) {
                            memcpy(ptr, (char*)&(it->client), sizeof(ClientInfo));
                            ptr += sizeof(ClientInfo);
                            total += sizeof(ClientInfo);
                        }
                        else 
                            break;
                    }
                    if(it == sServer.end()) 
                        dpSend.header.isOver = 1;
                    else    //the package is not over
                        dpSend.header.isOver = 0;
                    dpSend.header.type = (unsigned char)0x12;
                    dpSend.header.dataSize = (unsigned short)total;
                    send(si.socket,(char*)&dpSend, sizeof(dpSend), 0); //send client list
                }
                break;
            }
            case 0x03:  //send message to another client
            {
                printf("%s Client %s:%d require to send message!\n", getTime(), si.client.IPaddress, si.client.port);
                do {
                    unsigned short clientNo;
                    memcpy((char*)&clientNo, (char*)dp.data, sizeof(unsigned short));
                    if(clientNo > sServer.size()-1) {  //target client isn't exist, send error message
                        dpSend.header.type = (unsigned short)0x30;
                        dpSend.header.isOver = 1;
                        printf("%s Target client is not exist!\n", getTime());
                        strcpy(dpSend.data, "Target client is not exist!");
                        dpSend.header.dataSize = strlen(dp.data);
                        send(si.socket, (char*)&dpSend, sizeof(dpSend), 0);

                        break;
                    }

                    //printf("Debug: recieve a package!\n");

                    std::vector<socketAndInfo>::iterator it =  find(sServer.begin(), sServer.end(), si);
                    unsigned short senderNo = (unsigned short)(it - sServer.begin());
                    memcpy((char*)dpSend.data, &senderNo, sizeof(senderNo)); //set sender NO.

                    char *ptr = (char*)dp.data + sizeof(unsigned short);    //get message
                    memcpy(dpSend.data+sizeof(senderNo), ptr, dp.header.dataSize-sizeof(unsigned short));    //copy message
                    dpSend.header.isOver = dp.header.isOver;
                    dpSend.header.type = (unsigned short)0x20;
                    dpSend.header.dataSize = dp.header.dataSize;
                    send(sServer[clientNo].socket, (char*)&dpSend, sizeof(dpSend), 0);  //send message

                }while(!dp.header.isOver);
                
                break;
            }
            default: 
            { 
                printf("Error Data Package Format!\n");
                break;
            }
        }
    }
    
}

void Server::run() {
    //need to response "exit" option
    printf("%s Server Start! Press 'q' to stop!\n", getTime());
    
    keepGoing = true;
    SOCKET s;
    struct sockaddr_in sa;
    int length = sizeof(sa);
    keyboardThread = new std::thread(listenFromKeyboard, this);
    
    while(1) {
        printf("%s Waiting for client to connect!\n", getTime());
        s = accept(sListen, (struct sockaddr*)&sa, &length);
        if(keepGoing == false)
            break;
        if(s == INVALID_SOCKET) {
            printf("Accept() failed!\n");
            continue;
        }
        
        printf("%s Accept client: %s: %d\n", getTime(), inet_ntoa(sa.sin_addr), ntohs(sa.sin_port));
        
        //save client info and start a new thread to deal with this client's requestments
        socketAndInfo *tmp = new socketAndInfo(s, inet_ntoa(sa.sin_addr), ntohs(sa.sin_port));
        sServer.push_back(*tmp);
        std::thread *th = new std::thread(listenFromClient, this, *tmp);    //new thread
        clientSet.push_back(th);
    
        //printf("debug1\n");
    }
    clear();
}

void Server::stop() {
    char key;
    sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in tmp;
    tmp.sin_family = AF_INET;
    tmp.sin_port = htons(SERVER_PORT);
    tmp.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    do {
        key = getch();
        if(key == 'q' || key == 'Q') {
            keepGoing = false;
            connect(sClient, (struct sockaddr*)&tmp, sizeof(tmp));
            break;
        }
    }while(1);
}

char* Server::getTime() {
    time_t t;
    time(&t);
    struct tm *tmp_time = localtime(&t);
    strftime(timeStamp, sizeof(timeStamp), "%Y/%m/%d %H:%M:%S", tmp_time);

    return timeStamp;
}
