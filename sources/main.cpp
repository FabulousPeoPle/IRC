#include "Server.hpp"
#include <iostream>

char* strdup(const char *s)
{
    char *d = (char*)malloc(strlen(s) + 1);
    if (d == NULL)
        return (NULL);
    strcpy(d, s);
    return (d);
}

int main(void)
{
    
    // Server myServer("6667", "", "Server@1337.ma");

    // std::cout << "Server name: " << myServer.getServName() << std::endl;
    // myServer.setServerHints(AF_INET6, SOCK_STREAM, AI_PASSIVE);
    // myServer.setServerInfo();
    // myServer.setSockfd(AF_INET6);
    // // myServer.setSockfd_in6();
    // myServer.listen();
    // myServer.startServer();
    std::string message;
    Client clt;
    while (getline(std::cin, message))
    {
        Message mssg(message, clt);
        mssg.parse();
    }
}
