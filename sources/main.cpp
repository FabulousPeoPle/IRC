/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ohachim <ohachim@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/11 16:41:54 by ohachim           #+#    #+#             */
/*   Updated: 2022/02/14 19:12:19 by ohachim          ###   ########.fr       */
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

int main(void)
{
    
    Server myServer("6667", "", "Server@1337.ma", 150);

    std::cout << "Server name: " << myServer.getServName() << std::endl;
    
    myServer.setServerHints(AF_INET6, SOCK_STREAM, AI_PASSIVE);
    myServer.setServerInfo();
    myServer.setSockfd(AF_INET6);
    myServer.listen();
    myServer.startServer();
}
