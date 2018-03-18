#include <winsock2.h>
#include <stdio.h>
#include <windows.h>
#include <process.h>
#include <vector>

#define SERVER_PORT 1170

// restore socket and information
class socketAndInfo{
public:
    socketAndInfo(SOCKET s, struct sockaddr_in sc) { socket = s; saClient = sc; }
    SOCKET socket;
    struct sockaddr_in saClient;
};

WORD wVersionRequested;
WSADATA wsaData;
SOCKET sListen;
struct sockaddr_in saServer, saClient;
std::vector<socketAndInfo> sServer;

void init();
void clientThread(void *arg);

int main {
    init();
    SOCKET s;
    int length = sizeof(saClient);
    HANDLE handle;
    
    while(1) {
        s = accpet(sListen, (struct sockaddr*)&saClient, length);
        if(s == INVALID_SOCKET) {
            printf("accept() failed!\n");
            continue;
        }
        printf("Accept client: %s: %d\n", inet_ntoa(saClient.sin_addr), ntohs(saClient.sin_port));
        sServer.push_back(new socketAndInfo(s, saClient));
        handler = (HANDLE)_beginThead();
    }

    return 0;
}

void init() {
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

