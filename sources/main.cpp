#include "irc_server.hpp"
#include <iostream>
 
int main(void)
{
    Server myServer;

    myServer.setServerHints(AF_INET6, SOCK_STREAM, AI_PASSIVE);
    myServer.setServerInfo();
    myServer.setSockfd(AF_INET6);
    myServer.listen();
    myServer.startServer();
}
