#ifndef _SERVER_H_
#define _SERVER_H_

#include <winsock2.h>
#include <vector>
#include <string>
#include <cstring>
#include "protocol.h"

struct socketAndInfo {
    socketAndInfo() {}
    socketAndInfo(SOCKET s,  char *ip, unsigned short p) { socket = s;  strcpy_s(client.IPaddress, ip); client.port=p;}
    SOCKET socket;
    ClientInfo client;
};

class Server {
public:
    Server();
    Server(std::string name);
    int init();
    void run();
    void clientThread(socketAndInfo si);
    int clear(); 

private:
    std::string name;
    WORD wVersionRequested;
    WSADATA wsaData;
    SOCKET sListen;
    struct sockaddr_in saServer;
    std::vector<socketAndInfo> sServer; 
    std::vector<std::thread> clientSet;
    bool keepGoing;
};

#endif
