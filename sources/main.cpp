/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ohachim <ohachim@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/11 16:41:54 by ohachim           #+#    #+#             */
/*   Updated: 2022/02/23 17:18:27 by ohachim          ###   ########.fr       */
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
    
    (void)argc;
    (void)argv;
    Server myServer("6667", "", "FabulousPP", 1);

    myServer.initializeCmdFuncs();
    myServer.setOperPassword("meh");
    std::cout << "Server name: " << myServer.getServName() << std::endl;
    
    // myServer.setServerHints(AF_INET6, SOCK_STREAM, AI_PASSIVE);
    // myServer.setServerInfo();
    myServer.setSockfd_in6();
    myServer.listen();
    myServer.startServer();
}
