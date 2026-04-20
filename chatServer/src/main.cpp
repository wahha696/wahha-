#include"../header/chatServer.h"

int main(int argc,const char* argv[])
{
    if(argc != 3)
    {
        printf("Usage: %s <IP_ADDRESS> <PORT>\n", argv[0]);
        return -1;
    }
    ChatServer server(argv[1],atoi(argv[2]));
    server.run();
    return 0;
}