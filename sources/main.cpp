#include "irc_server.hpp"
#include <iostream>

int main(void)
{
    Server myServer;

    myServer.setServerHints(AF_INET, SOCK_STREAM, AI_PASSIVE);
    myServer.setServerInfo();
    myServer.setSockfd(AF_INET);
    myServer.listen();
    myServer.startServer();
}