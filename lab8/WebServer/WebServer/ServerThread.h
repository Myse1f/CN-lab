#pragma once

#include <winsock2.h>
#include <windows.h>
#include <process.h>
#include <vector>

class ServerThread
{
public:
	//ServerThread();
	ServerThread(std::vector<SOCKET>* sServer, SOCKET s);
	~ServerThread();	
	static unsigned __stdcall ThreadStaticEntryPoint(void * pThis);
	HANDLE htn;
private:
	SOCKET socket;
	//Server *server;
	std::vector<SOCKET>* sServer;
	void ThreadEntryPoint();
};

