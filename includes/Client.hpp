/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azouiten <azouiten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/14 16:51:38 by azouiten          #+#    #+#             */
/*   Updated: 2022/01/07 19:19:37 by azouiten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include  <vector>
#include <sys/socket.h>
#include "Message.hpp"

class Message;

class Client
{

    // TODO: must add variable representing unique identifier
    // TODO: variable representing server that the client is connected too
    // TODO: if the client is a USER, we must know the name of the host it's connected top and the username of the user?? wtf is that
    // TODO: if it's a service we must know the name of the service, which is composed of the nickname and the servername
    // TODO: if it's a service, we must know the type of the service
    // services and users do not have the same privelages, example: services can request part of the global state information(if not all of it), they cannot access channels, users can access channels
    // TODO: what the fuck is the flood control mechanism?
    // TODO: generator QUIT message if the connection breaks
    public:
        int                      _sock_fd;
        struct sockaddr_storage  _addr;
        socklen_t                _addr_size;
        std::string              _nickname;
        std::string              _username;
        std::string              _realname;
        std::string              _ip_address;
        Message                  msg;
        bool                     _authenticated;

        Client(void);
        Client(Client const & src);
        Client(int, struct sockaddr_storage, socklen_t);
        ~Client(void);

        Client & operator=(Client const & rhs);
};

#endif
