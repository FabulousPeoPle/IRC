/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azouiten <azouiten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/14 16:51:38 by azouiten          #+#    #+#             */
/*   Updated: 2021/12/17 13:30:21 by azouiten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include  <vector>
#include <sys/socket.h>
#include "Message.hpp"

class Client
{
public:
    int                      _sock_fd;
    struct sockaddr_storage  _addr;
    socklen_t                _addr_size;
    std::string              _nickname;
    std::string              _username;
    std::string              _ip_address;
    Message                  * msg;
    bool                     _authenticated;

    Client(void);
    Client(Client const & src);
    Client(int, struct sockaddr_storage, socklen_t);
    ~Client(void);

    Client & operator=(Client const & rhs);
};

#endif