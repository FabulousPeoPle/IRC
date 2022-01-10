/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azouiten <azouiten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/10 15:04:36 by azouiten          #+#    #+#             */
/*   Updated: 2022/01/10 20:52:46 by azouiten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
    
    Server myServer("6667", "", "Server@1337.ma");

    std::cout << "Server name: " << myServer.getServName() << std::endl;
    
    myServer.setServerHints(AF_INET6, SOCK_STREAM, AI_PASSIVE);
    myServer.setServerInfo();
    myServer.setSockfd(AF_INET6);
    myServer.listen();
    myServer.startServer();

}
