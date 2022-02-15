/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ohachim <ohachim@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/07 12:10:43 by azouiten          #+#    #+#             */
/*   Updated: 2022/02/15 13:06:37 by azouiten         ###   ########.fr       */
/*   Updated: 2022/02/15 17:46:31 by ohachim          				          */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(void) : m_topic("") {}

Channel::Channel(int mode, int opFd, std::string name, char type, std::string password) : m_name(name), m_mode(mode), m_type(type), m_password(password)
{
	this->m_operators.push_back(opFd);
	this->m_topic = "";
}

Channel::~Channel(void)
{}

std::string  Channel::getName(void) const
{
	return (this->m_name);
}

std::string  Channel::getPassword(void) const
{
	return (this->m_password);
}

std::string  Channel::getTopic(void) const
{
	return (this->m_topic);
}

int  Channel::getMode(void) const
{
	return (this->m_mode);
}

int  Channel::getType(void) const
{
	return (this->m_type);
}

std::vector<int> &Channel::getOps(void)
{
	return (this->m_operators);
}

std::vector<int> &Channel::getMembers(void)
{
	return (this->m_members);
}

void	Channel::setName(std::string name)
{
	m_name = name;
}

void	Channel::setPassword(std::string password)
{
	m_password = password;
}

void	Channel::setTopic(std::string topic)
{
	m_topic = topic;
}

void	Channel::setMode(int mode)
{
	m_mode = m_mode | mode;
}

void	Channel::addMember(int clientFd)
{
	m_members.push_back(clientFd);
}

void	Channel::addOp(int clientFd)
{
	m_operators.push_back(clientFd);
}


bool	Channel::checkPassword(std::string password)
{
	if (m_password == password)
		return (true);
	return (false);
}

bool Channel::isOp(int clientFd) const
{
	return (((std::find(m_operators.begin(), m_operators.end(), clientFd) != m_operators.end()) ? true : false));
}

bool Channel::isMember(int clientFd) const
{
	return (((std::find(m_members.begin(), m_members.end(), clientFd) != m_members.end()) ? true : false));
}

// std::string	Channel::m_composeMask(Client & client) const
// {
// 	return (client._nickname + "!" + client._username + "@" + client._ip_address);
// }

bool Channel::isBanned(int clientFd) const
{
	return (((std::find(m_banned.begin(), m_banned.end(), clientFd) != m_banned.end()) ? true : false));
}

void Channel::Ban(int clientFd)
{
	this->m_banned.push_back(clientFd);
}

void	Channel::removeMember(int clientFd)
{
	std::vector<int>::iterator it = m_members.begin();
	std::vector<int>::iterator end = m_members.end();
	
	while (it != end)
	{
		if (*it == clientFd)
			m_members.erase(it);
		it++;
	}
}

void	Channel::removeOp(int clientFd)
{
	std::vector<int>::iterator it = m_operators.begin();
	std::vector<int>::iterator end = m_operators.end();
	
	while (it != end)
	{
		if (*it == clientFd)
			m_operators.erase(it);
		it++;
	}
}
void		Channel::turnOffMode(int modeNum)
{
	this->modes &= ~(this->modeBitMasks[modeNum]);
}

void		Channel::turnOnMode(int modeNum)
{
	this->modes |= this->modeBitMasks[modeNum];
}

bool		Channel::getModeValue(int modeNum) const
{
	return (this->modeBitMasks[modeNum] & this->modes);
}

std::uint32_t       Channel::modeBitMasks[NUM_MODES_CHANNEL] = {1 << 0, 1 << 1, 1 << 2, 1 << 3, 1 << 4, 1 << 5, 1 << 6,
																	1 << 7, 1 << 8, 1 << 9, 1 << 10, 1 << 11, 1 << 12,
																		1 << 13, 1 << 14, 1 << 15, 1 << 16};
std::string         Channel::potentialModes = "OovaimnqpsrtklbeI";
