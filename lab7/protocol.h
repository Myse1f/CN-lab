#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#define PACKAGE_DATA_SIZE 5000

#include <string.h>

struct Header {
    Header() { delimiter = 0xffff; }
    unsigned short delimiter;
    unsigned short dataSize; //Data size
    unsigned char isOver;  //1--the info is over  
    unsigned char type;    
    //0x0? -- require,      0x00--time 0x01--servername 0x02--list 0x03--message(thr first short in data is target NO.)
    //0x1? -- response,     0x10--time 0x11--servername 0x12--list 
    //0x2? -- instruct,     0x20--message, 0x21--exit  
    //0x30 -- error message
};

struct DataPackage {
    DataPackage() { memset(data, 0, PACKAGE_DATA_SIZE); }
    Header header;
    char data[PACKAGE_DATA_SIZE];
};

struct ClientInfo {
    char IPaddress[16];
    unsigned short port;
};

#endif