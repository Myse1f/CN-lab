#ifndef _SERVER_H_
#define _SERVER_H_

#include <winsock2.h>
#include <vector>
#include <thread>
#include <string>

struct socketAndInfo {
    socketAndInfo(SOCKET s, struct sockaddr_in sc) { socket = s; saClient = sc; }
    SOCKET socket;
    struct sockaddr_in saClient;
};

class Server {
public:
    Server();
    Server(std::string name);
    void init();
    void run();
    void clientThread();
    static unsigned __stdcall ThreadStaticEntryPoint(void *pThis);

private:
    std::string name;
    WORD wVersionRequested;
    WSADATA wsaData;
    SOCKET sListen;
    struct sockaddr_in saServer;
    std::vector<socketAndInfo> sServer;
    std::vector<std::thread> clientSet;
};

#endif
