/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azouiten <azouiten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/07 12:10:43 by azouiten          #+#    #+#             */
/*   Updated: 2022/02/07 13:57:50 by azouiten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(void){}

Channel::Channel(int mode, int op, std::string name, char type) : m_name(name), m_mode(mode), m_type(type)
{
	this->m_operators.push_back(op);
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

char  Channel::getType(void) const
{
	return (this->m_type);
}

std::vector<int> Channel::getOps(void) const
{
	return (this->m_operators);
}

bool Channel::isOp(int client_fd) const
{
	return (((std::find(m_operators.begin(), m_operators.end(), client_fd) != m_operators.end()) ? true : false));
}