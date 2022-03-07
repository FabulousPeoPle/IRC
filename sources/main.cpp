/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ohachim <ohachim@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/11 16:41:54 by ohachim           #+#    #+#             */
/*   Updated: 2022/03/07 20:24:56 by ohachim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Server.hpp"
#include <cstring>
#include <iostream>

char* strdup(const char *s)
{
    char *d = (char*)malloc(strlen(s) + 1);
    if (d == NULL)
        return (NULL);
    strcpy(d, s);
    return (d);
}

int main(int argc, char **argv)
{
    
    if (argc != 3)
    {
        std::cout << "Usage: ./ircserv [port] [password]\n";
        return (-1);
    }
    Server myServer(argv[1], "", "FabulousPP@1337.ma", 200);

    myServer.initializeCmdFuncs();
    myServer.setOperPassword("password");
    myServer.setServPassword(argv[2]);

    //std::cout << "Server name: " << myServer.getServName() << std::endl;
    
    myServer.setServerHints(AF_INET, SOCK_STREAM, AI_PASSIVE);
    myServer.setServerInfo();
    myServer.setSockfd(AF_INET);
    myServer.listen();
    myServer.startServer();
    
}
