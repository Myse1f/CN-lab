#include "KeyboardThread.h"
#include <conio.h>

#pragma comment(lib, "Ws2_32.lib")

KeyboardThread::KeyboardThread(bool *b)
{
	this->keepGoing = b;
}


KeyboardThread::~KeyboardThread()
{
}

unsigned __stdcall KeyboardThread::ThreadStaticEntryPoint(void * pThis)
{
	KeyboardThread * pth = (KeyboardThread*)pThis;   // the tricky cast
	pth->ThreadEntryPoint();           // now call the true entry-point-function

	return 1;          // the thread exit code
}

void KeyboardThread::ThreadEntryPoint() {
	char key;
	SOCKET sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in tmp;
	tmp.sin_family = AF_INET;
	tmp.sin_port = htons(1170);
	tmp.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	do {
		key = _getch();
		if (key == 'q' || key == 'Q') {
			*(this->keepGoing) = false;
			connect(sClient, (struct sockaddr*)&tmp, sizeof(tmp));
			break;
		}
	} while (1);
}