#include "../header/chatClient.h"

int main(int argc,const char* argv[])
{
    if(argc != 4)
    {
        printf("Usage: %s <IP_ADDRESS> <PORT> <NAME>\n", argv[0]);
        return -1;
    }
    ChatClient client(argv[1],atoi(argv[2]),argv[3]);
    client.run();
}