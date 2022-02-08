/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azouiten <azouiten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/07 12:10:43 by azouiten          #+#    #+#             */
/*   Updated: 2022/02/08 17:18:52 by azouiten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(void){}

Channel::Channel(int mode, int opFd, std::string name, char type, std::string password) : m_name(name), m_mode(mode), m_type(type), m_password(password)
{
	this->m_operators.push_back(opFd);
}

Channel::~Channel(void)
{}

std::string  Channel::getName(void) const
{
	return (this->m_name);
}

int  Channel::getMode(void) const
{
	return (this->m_mode);
}

int  Channel::getType(void) const
{
	return (this->m_type);
}

std::vector<int> Channel::getOps(void) const
{
	return (this->m_operators);
}

bool Channel::isOp(int clientFd) const
{
	return (((std::find(m_operators.begin(), m_operators.end(), clientFd) != m_operators.end()) ? true : false));
}

bool Channel::isBanned(int clientFd) const
{
	return (((std::find(m_banned.begin(), m_banned.end(), clientFd) != m_banned.end()) ? true : false));
}

void Channel::Ban(int clientFd)
{
	this->m_banned.push_back(clientFd);
}
