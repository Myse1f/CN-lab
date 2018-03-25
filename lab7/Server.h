#ifndef _SERVER_H_
#define _SERVER_H_

#include <winsock2.h>
#include <vector>
#include <string>
#include <cstring>
#include "mingw.thread.h"
#include "protocol.h"

struct socketAndInfo {
    socketAndInfo() {}
    socketAndInfo(SOCKET s,  char *ip, unsigned short p) { socket = s;  strcpy(client.IPaddress, ip); client.port=p;}
    bool operator==(const socketAndInfo &si) { return this->socket == si.socket; }
    SOCKET socket;
    ClientInfo client;
};

class Server {
public:
    Server();
    Server(std::string name);
    int init();
    void run();
    void clientThread(socketAndInfo &si);
    void clear(); 
    void stop();

private:
    std::string name;
    WORD wVersionRequested;
    WSADATA wsaData;
    SOCKET sListen, sClient;
    struct sockaddr_in saServer;
    std::vector<socketAndInfo> sServer; 
    std::vector<std::thread*> clientSet;
    std::thread *keyboardThread;
    bool keepGoing;
};

#endif
