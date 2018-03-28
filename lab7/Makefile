Server: main.o Server.o
	g++ -g main.o Server.o -o Server

main.o: main.c Server.h
	g++ -g -c main.c -lws2_32 -std=c++0x 

Server.o: Server.c Server.h protocol.h mingw.thread.h
	g++ -g -c Server.c -std=c++0x -lws2_32

clean: 
	del Server main.o Server.o