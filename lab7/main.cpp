#include "Server.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
    if(argc == 1) 
        Server server;
    else    
        Server server(argv[1]);

    server.run();

    return 0;
}
