#pragma once

#include <winsock2.h>
#include <windows.h>
#include <process.h>

class KeyboardThread
{
public:
	//KeyboardThread();
	KeyboardThread(bool *b);
	~KeyboardThread();
	static unsigned __stdcall ThreadStaticEntryPoint(void * pThis);
	HANDLE htn;
private:
	//Server *server;
	bool* keepGoing;
	void ThreadEntryPoint();
};

