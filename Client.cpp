/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azouiten <azouiten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/14 16:51:35 by azouiten          #+#    #+#             */
/*   Updated: 2021/12/16 15:55:09 by azouiten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(void) : _authenticated(false)
{}

Client::Client(Client const & src)
{
	if (&src != this)
		*this = src;
}

Client::Client(int sock_fd, struct sockaddr_storage addr, socklen_t len) : _sock_fd(sock_fd),
_addr(addr), _addr_size(len), _authenticated(false)
{}

Client::~Client(void)
{}

Client & Client::operator=(Client const & rhs)
{
	_sock_fd = rhs._sock_fd;
	_addr = rhs._addr;
	_addr_size = rhs._addr_size;
	_nickname = rhs._nickname;
	_username = rhs._username;
	_ip_address = rhs._ip_address;
	_authenticated = _authenticated;
}