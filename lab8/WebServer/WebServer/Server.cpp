#include "Server.h"
#include <iostream>
#include <cstdio>
#include <process.h>

#pragma comment(lib, "Ws2_32.lib")

Server::Server()
{
	if (init() == false) {
		std::cout << "Server Initalize failed!" << std::endl;
		exit(-1);
	}
}


Server::~Server()
{
}

bool Server::init() {
	int ret;

	wVersionRequested = MAKEWORD(2, 2);
	ret = WSAStartup(wVersionRequested, &wsaData);
	if (ret != 0) {
		printf("WSAStart failed!\n");
		return false;
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		WSACleanup();
		printf("Invalid Winsock version!\n");
		return false;
	}
	sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sListen == INVALID_SOCKET) {
		WSACleanup();
		printf("socket() failed!\n");
		return false;
	}
	saServer.sin_family = AF_INET;
	saServer.sin_port = htons(SERVER_PORT);
	saServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	ret = bind(sListen, (struct sockaddr*)&saServer, sizeof(saServer));
	if (ret == SOCKET_ERROR) {
		printf("bind() failed!\n");
		closesocket(sListen);
		WSACleanup();
		return false;
	}
	ret = listen(sListen, 5);
	if (ret == SOCKET_ERROR) {
		printf("listen() failed!\n");
		closesocket(sListen);
		WSACleanup();
		return false;
	}

	return true;
}

void Server::run() {
	printf("Server Start!\n");

	this->setKeepGoing(true);
	SOCKET s;
	struct sockaddr_in sa;
	int length = sizeof(sa);
	unsigned id;
	KeyboardThread *keyboardThread = new KeyboardThread(&keepGoing);
	HANDLE htn = (HANDLE)_beginthreadex(NULL, 0, KeyboardThread::ThreadStaticEntryPoint, keyboardThread, CREATE_SUSPENDED, &id);
	keyboardThread->htn = htn;
	ResumeThread(htn);

	while (1) {
		printf("Waiting for client to connect!\n");
		s = accept(sListen, (struct sockaddr*)&sa, &length);
		if (keepGoing == false)
			break;
		if (s == INVALID_SOCKET) {
			printf("Accept() failed!\n");
			continue;
		}

		printf("Accept client: %s: %d\n", inet_ntoa(sa.sin_addr), ntohs(sa.sin_port));

		//save client socket and start a new thread to deal with this client's requestments
		ServerThread *th = new ServerThread(&sServer, s);
		HANDLE htn = (HANDLE)_beginthreadex(NULL, 0, ServerThread::ThreadStaticEntryPoint, th, CREATE_SUSPENDED, &id);
		th->htn = htn;
		ResumeThread(htn);
		sServer.push_back(s);
		clientSet.push_back(th);

		//printf("debug1\n");
	}
	close();
}

void Server::close() {
	//close all socket
	for (std::vector<SOCKET>::iterator it = sServer.begin(); it != sServer.end(); ++it) {
		closesocket(*it);
	}
	sServer.clear();

	//join all thread
	for (std::vector<ServerThread*>::iterator it = clientSet.begin(); it != clientSet.end(); ++it) {
		//WaitForSingleObject((*it)->htn, INFINITE);
		//CloseHandle((*it)->htn);
		delete *it;
	}
	clientSet.clear();
	//WaitForSingleObject(keyboardThread->htn, INFINITE);
	//CloseHandle(keyboardThread->htn);

	closesocket(sListen);
	WSACleanup();
	printf("Server Stop!\n");
}

void Server::setKeepGoing(bool b) {
	this->keepGoing = b;
}

std::vector<SOCKET>& Server::getSServer() {
	return sServer;
}