/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azouiten <azouiten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/07 12:12:16 by azouiten          #+#    #+#             */
/*   Updated: 2022/02/21 15:36:12 by azouiten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#ifndef _CHANNEL_
#define _CHANNEL_

#include <iostream>
#include <vector>

#include <Client.hpp>

#define LOCAL_CHAN '&'
#define NETWORKWIDE_CHAN '#'
#define CHANNEL_NUMBER_LIMIT 10

//IDK IF THIS IS HOW WE ARE HANDLING THIS, THIS IS JUST SO I CAN WORK ON THE JOIN COMMAND 

#define NUM_MODES_CHANNEL 9

class Client;

namespace ChannelModes
{
    enum
    {
		/******-UserSpecific-****/
		O_Creator,
        o_OperatorPrivilege, // we can keep these here, but make sure they don't mix with the other ones
        v_voicePrivilege,
		/************************/

		/******-ChannelSpecific-*/
        a_annonymous = 0,
        i_inviteOnly,
        m_moderated,
        n_noOutsideMessage,
        q_quiet,
        p_private,
        s_secret,
        r_reop,
        t_topicOperatorOnly,
		/************************/

		total
    };
};
class Channel
{
private:
	std::string					m_name; // cant be 7 ' ' or ',' anything else id fair game
	int							m_mode; // change
	int							m_type; // the scope of the channel local/networkwide
	std::vector<int>			m_operators; // this vector stores the clientfds of the ops
	std::string					m_topic;
	std::vector<int>			m_banned; // probably useless
	std::vector<int>			m_members;
	std::vector<int>			m_invited;
	std::string					m_password;
	std::uint16_t               modes; // need to give em default modes

	std::vector<std::string>	m_banMasks;
	std::vector<std::string>	m_exceptionBanMasks;
	std::vector<std::string>	m_inviteMasks;

	int							m_userLimit;

	

public:
	Channel(void);
	Channel(int mode, int opFd, std::string name, char type, std::string password);
	~Channel(void);


	std::string 		getName(void) const;
	std::string 		getPassword(void) const;
	int					getMode(void) const;
	std::vector<int> 	&getOps(void);
	std::vector<int> 	&getMembers(void);
	std::vector<int> 	&getInvited(void);
	int			 		getType(void) const;
	std::string			getTopic(void) const;
	
	void					setTopic(std::string topic);
	void					setMode(int mode);
	void					setName(std::string name);
	void					setPassword(std::string password);
	
	bool				isOp(int clientFd) const;
	bool				checkPassword(std::string password);
	void				addMember(int clientFd);
	void				addInvited(int clientFd);
	void				removeMember(int clientFd);
	void				removeOp(int clientFd);
	void				removeInvited(int clientFd);
	void				addOp(int clientFd);
	bool				isBanned(int clientFd) const;
	bool				isMember(int clientFd) const;
	bool				isInvited(int clientFd) const;
	// std::string			m_composeMask(Client & client) const;
	void					Ban(int clinetFd);

	bool        			getModeValue(int modeNum) const;

	int 	    			findMode(char c) const;

    void        			turnOnMode(int modeNum);
    void        			turnOffMode(int modeNum);

	static std::uint16_t	modeBitMasks[NUM_MODES_CHANNEL];
	static std::string		potentialModes;
};

#endif