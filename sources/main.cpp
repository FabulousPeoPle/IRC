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
    
    Server myServer("6667", "", "My Server");



    myServer.setServerHints(AF_INET6, SOCK_STREAM, AI_PASSIVE);
    myServer.setServerInfo();
    myServer.setSockfd(AF_INET6);
    myServer.listen();
    myServer.startServer();
}
