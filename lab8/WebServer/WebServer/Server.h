#pragma once

#include <winsock2.h>
#include <vector>
#include "ServerThread.h"
#include "KeyboardThread.h"

#define SERVER_PORT 1170

class Server
{
public:
	Server();
	~Server();
	bool init();
	void run();
	void close();
	void setKeepGoing(bool b);
	std::vector<SOCKET>& getSServer();

private:
	WORD wVersionRequested;
	WSADATA wsaData;
	SOCKET sListen, sClient;
	struct sockaddr_in saServer;
	std::vector<SOCKET> sServer;
	std::vector<ServerThread*> clientSet;
	KeyboardThread *keyboardThread;
	bool keepGoing;
};

