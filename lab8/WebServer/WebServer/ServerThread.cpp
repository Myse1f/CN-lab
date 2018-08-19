#include "ServerThread.h"
#include <algorithm>
#include <string>
#include <cstring>
#include <cstdio>

#pragma comment(lib, "Ws2_32.lib")

ServerThread::ServerThread(std::vector<SOCKET>* sServer, SOCKET s)
{
	this->sServer = sServer;
	socket = s;
}


ServerThread::~ServerThread()
{
}

unsigned __stdcall ServerThread::ThreadStaticEntryPoint(void * pThis)
{
	ServerThread * pth = (ServerThread*)pThis;   // the tricky cast
	pth->ThreadEntryPoint();           // now call the true entry-point-function

	return 1;          // the thread exit code
}

void ServerThread::ThreadEntryPoint() {
	bool end = false;
	int ret, num=0; 
	char buf[204800] = { 0 };
	char *ptr = buf;

	//read header into buffer
	while (!end) {
		ret = recv(socket, ptr, 1, 0);
		if (ret == SOCKET_ERROR || ret == 0) {
			printf("Client has closed the connection!\n");
			//close socket
			std::vector<SOCKET>::iterator it = std::find(sServer->begin(), sServer->end(), socket);
			closesocket(socket);
			sServer->erase(it);
			return;
		}
		if (*ptr == '\r' && (num == 0 || num == 2))
			num++;
		else if (*ptr == '\n' && num == 1)
			num++;
		else if (*ptr == '\n' && num == 3)
			end = true;
		else
			num = 0;
		ptr++;
	}

	//get the first line
	char *p = strstr(buf, "\r\n");
	int len = p - buf;
	char *line = new char[len + 1];
	strncpy(line, buf, len);
	line[len] = '\0';
	std::string line1(line);
	//find content-length
	p = strstr(buf, "Content-Length");
	if (p != NULL) {
		char *tmp1 = strstr(p, "\r\n");
		char *tmp2 = strchr(p, ' ');
		char length[10];
		strncpy(length, tmp2+1, tmp1-tmp2-1);
		len = atoi(length);
	}

	if (line1.find("GET") != std::string::npos) {
		int pos1 = line1.find(' ');
		int pos2 = line1.find(' ', pos1+1);
		std::string filename = line1.substr(pos1 + 1, pos2 - pos1 - 1);
		std::string type = "Content-Type: ";
		//remapping file path
		if (filename.find("/myhtml") != std::string::npos) {
			filename = filename.replace(0, 7, "/html"); 
			type += "text/html\r\n";
		}
		if (filename.find("/mytxt") != std::string::npos) {
			filename = filename.replace(0, 6, "/txt");
			type += "text/plain\r\n";
		}
		if (filename.find("/img") != std::string::npos) {
			//filename = filename.replace(0, 6, "/img");
			type += "img/jpg\r\n";
		}
		std::string path = ".";
		path = path + filename;

		memset(buf, 0, 204800);
		FILE *fp = fopen(path.c_str(), "rb");
		if (fp == NULL) {
			strcpy(buf, "HTTP/1.0 404 Not Found\r\n");
			char *content = "<html><body>404 Not Found</body></html>";
			strcat(buf, "Content-Length: ");
			char size[10]; 
			_itoa(strlen(content), size, 10);
			strcat(buf, size);
			strcat(buf, "\r\n");
			strcat(buf, "Content-Type: text/html\r\n\r\n");
			strcat(buf, content);			
			send(socket, buf, strlen(buf), 0);
		}
		else {
			strcpy(buf, "HTTP/1.0 200 OK\r\n");
			fseek(fp, 0L, SEEK_END);
			int len = ftell(fp);
			fseek(fp, 0L, SEEK_SET);
			char size[10];
			_itoa(len, size, 10);
			strcat(buf, "Content-Length: ");
			strcat(buf, size);
			strcat(buf, "\r\n");
			strcat(buf, type.c_str());
			strcat(buf, "\r\n");
			int offset = strlen(buf);
			fread(buf+offset, 1, len, fp);
			send(socket, buf, offset+len, 0);
		}
		
	}
	else if (line1.find("POST") != std::string::npos) {
		int pos1 = line1.find(' ');
		int pos2 = line1.find(' ', pos1 + 1);
		std::string action = line1.substr(pos1 + 1, pos2 - pos1 - 1);

		memset(buf, 0, 204800);
		if (action == "/myhtml/dopost") {
			std::string status = "HTTP/1.0 200 OK\r\n";
			std::string type = "Content-Type: text/html\r\n";
			ptr = buf;
			while (len) {
				int ret = recv(socket, ptr, len, 0);
				len -= ret;
				ptr += ret;
			}
			char *p = strchr(buf, '&');
			char *p1 = strchr(buf, '=');
			char *p2 = strchr(p + 1, '=');
			*p = '\0';
			//password error
			if (strcmp(p1 + 1 + strlen(p1 + 1) - 4, p2 + 1) != 0) {
				char* content = "<html><body>Username or Password Error!</body></html>";
				char size[10];
				_itoa(strlen(content), size, 10);
				strcpy(buf, status.c_str());
				strcat(buf, "Content-Length: ");
				strcat(buf, size);
				strcat(buf, "\r\n");
				strcat(buf, type.c_str());
				strcat(buf, "\r\n");
				strcat(buf, content);
			}
			else {
				char* content = "<html><body>Login Success!</body></html>";
				char size[10];
				_itoa(strlen(content), size, 10);
				strcpy(buf, status.c_str());
				strcat(buf, "Content-Length: ");
				strcat(buf, size);
				strcat(buf, "\r\n");
				strcat(buf, type.c_str());
				strcat(buf, "\r\n");
				strcat(buf, content);
			}
		}
		else {
			strcpy(buf, "HTTP/1.0 404 Not Found\r\n");
			char *content = "<html><body>404 Not Found</body></html>";
			strcat(buf, "Content-Length: ");
			char size[10];
			_itoa(strlen(content), size, 10);
			strcat(buf, size);
			strcat(buf, "\r\n");
			strcat(buf, "Content-Type: text/html\r\n\r\n");
			strcat(buf, content);
		}
		send(socket, buf, strlen(buf), 0);
	}
	
	std::vector<SOCKET>::iterator it = std::find(sServer->begin(), sServer->end(), socket);
	sServer->erase(it);
	closesocket(socket);
}