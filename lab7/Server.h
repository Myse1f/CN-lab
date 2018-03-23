#ifndef _SERVER_H_
#define _SERVER_H_

#include <winsock2.h>
#include <vector>
#include <string>

struct socketAndInfo {
    socketAndInfo(SOCKET s, struct sockaddr_in sa) { socket = s; saClient = sa; }
    SOCKET socket;
    struct sockaddr_in saClient;
};

class Server {
public:
    Server();
    Server(std::string name);
    int init();
    void run();
    void clientThread();
    int clear(); 

private:
    std::string name;
    WORD wVersionRequested;
    WSADATA wsaData;
    SOCKET sListen;
    std::vector<socketAndInfo> sServer;
    std::vector<std::thread> clientSet;
};

#endif
