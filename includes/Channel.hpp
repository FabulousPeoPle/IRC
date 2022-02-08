/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azouiten <azouiten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/07 12:12:16 by azouiten          #+#    #+#             */
/*   Updated: 2022/02/08 19:05:29 by azouiten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _CHANNEL_
#define _CHANNEL_

#include <iostream>
#include <vector>

#define LOCAL_CHAN '&'
#define NETWORKWIDE_CHAN '#'
#define CHANNEL_NUMBER_LIMIT 10

//IDK IF THIS IS HOW WE ARE HANDLING THIS, THIS IS JUST SO I CAN WORK ON THE JOIN COMMAND 

#define CHAN_MODE_O 1
#define CHAN_MODE_P 2
#define CHAN_MODE_S 4
#define CHAN_MODE_I 8
#define CHAN_MODE_T 16
#define CHAN_MODE_N 32
#define CHAN_MODE_M 64
#define CHAN_MODE_L 128
#define CHAN_MODE_B 256
#define CHAN_MODE_V 512
#define CHAN_MODE_K 1024

class Channel
{
private:
	std::string			m_name; // cant be 7 ' ' or ',' anything else id fair game
	int					m_mode; // change
	int					m_type; // the scope of the channel local/networkwide
	std::vector<int>	m_operators; // this vector stores the clientfds of the ops
	std::vector<int>	m_banned;
	std::string			m_password;
public:
	Channel(void);
	Channel(int mode, int opFd, std::string name, char type, std::string password);
	~Channel(void);

	std::string 		getName(void) const;
	int					getMode(void) const;
	std::vector<int> 	getOps(void) const;
	int			 		getType(void) const;
	bool				isOp(int clientFd) const;
	// very wrong, banning should be done with the ip address or maybe it depends on the kind of the ban (permanent or not)
	bool				isBanned(int clientFd) const; 
	void				Ban(int clientFd);
};

#endif