#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#define MAX_PACKAGE_SIZE 65535

struct Header {
    Header() { delimiter = 0xffff; }
    unsigned short delimiter;
    unsigned short packageSize;
    unsigned char end;
    unsigned char type;    
    //0x0? -- require,      0x00--time 0x01--servername 0x02--list 0x03--message
    //0x1? -- response,     0x10--time 0x11--servername 0x12--list 
    //0x2? -- instruct,     0x20--message, 0x21--exit     
};

struct DataPackage {
    Header header;
    char *data;
};

struct TimeFormat {
    unsigned short year;
    unsigned short month;
    unsigned short day;
    unsigned short hh;
    unsigned short mm;
    unsigned short ss; 
};

struct ClientInfo {
    unsigned short clientNo;
    char IPaddress[12];
    unsigned short port;
};

#endif