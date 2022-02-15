/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ohachim <ohachim@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/14 16:51:35 by azouiten          #+#    #+#             */
/*   Updated: 2022/02/15 17:52:09 by ohachim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(void) : _nickAuth(false), _userAuth(false), _isServerOp(false), _away(false), _authenticated(false)
{
	this->modes = 0;
	this->turnOnMode(UserModes::restricted);
}

Client::Client(Client const & src)
{
	if (&src != this)
		*this = src;
}

Client::Client(int sock_fd, struct sockaddr_storage addr, socklen_t len) : _sock_fd(sock_fd),
_addr(addr), _addr_size(len), _nickAuth(false), _userAuth(false), _isServerOp(false), _away(false), _authenticated(false)
{
	this->modes = 0;
	this->turnOnMode(UserModes::restricted);
}

Client::~Client(void)
{}

Client & Client::operator=(Client const & rhs)
{
	_sock_fd = rhs._sock_fd;
	_addr = rhs._addr;
	_addr_size = rhs._addr_size;
	_nickname = rhs._nickname;
	_username = rhs._username;
	_realname = rhs._realname;
	_userAuth = rhs._userAuth;
	_nickAuth = rhs._nickAuth;
	_ip_address = rhs._ip_address;
	_authenticated = rhs._authenticated;
	return (*this);
}


void		Client::turnOffMode(int modeNum)
{
	this->modes &= ~(this->modeBitMasks[modeNum]);
}

void		Client::turnOnMode(int modeNum)
{
	this->modes |= this->modeBitMasks[modeNum];
}

bool		Client::getModeValue(int modeNum) const
{
	return (this->modeBitMasks[modeNum] & this->modes);
}

std::uint8_t        Client::modeBitMasks[NUM_MODES] = {1 << 0, 1 << 1, 1 << 2, 1 << 3, 1 << 4, 1 << 5};
std::string         Client::potentialModes = "aiwros";