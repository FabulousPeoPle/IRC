/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azouiten <azouiten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/07 12:12:16 by azouiten          #+#    #+#             */
/*   Updated: 2022/02/07 14:00:23 by azouiten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _CHANNEL_
#define _CHANNEL_

#include <iostream>
#include <vector>

#define LOCAL_CHAN '&'
#define NETWORKWIDE_CHAN '#'

class Channel
{
private:
	std::string			m_name; // cant be 7 ' ' or ',' anything else id fair game
	int					m_mode; // change
	char				m_type; // the scope of the channel local/networkwide
	std::vector<int>	m_operators; // this vector stores the clientfds of the ops
	
public:
	Channel(void);
	Channel(int mode, int op, std::string name, char type);
	~Channel(void);

	std::string 		getName(void) const;
	int					getMode(void) const;
	std::vector<int> 	getOps(void) const;
	char			 	getType(void) const;
	bool				isOp(int client_fd) const;
};


#endif