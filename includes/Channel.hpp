/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ohachim <ohachim@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/07 12:12:16 by azouiten          #+#    #+#             */
/*   Updated: 2022/02/15 18:01:59 by ohachim          ###   ########.fr       */
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

#define NUM_MODES_CHANNEL 17

class Client;

namespace ChannelModes
{
    enum
    {
		O_Creator,
        o_OperatorPrivilege,
        v_voicePrivilege,

        a_annonymous,
        i_inviteOnly,
        m_moderated,
        n_noOutsideMessage,
        q_quiet,
        p_private,
        s_secret,
        r_reop,
        t_topicOperatorOnly,
        
        k_password,
        l_userLimit,
        
        b_banMask,
		e_exceptionMask,
		I_inviteMask,
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
	std::vector<int>			m_banned;
	std::vector<int>			m_members;
	// std::vector<std::string>	m_banned;
	std::string					m_password;
	std::uint32_t               modes; // need to give em default modes

public:
	Channel(void);
	Channel(int mode, int opFd, std::string name, char type, std::string password);
	~Channel(void);

	std::string 			getName(void) const;
	std::string 			getPassword(void) const;
	int						getMode(void) const;
	std::vector<int> 		getOps(void) const;
	int			 			getType(void) const;
	std::string				getTopic(void) const;
	void					setTopic(std::string topic);
	void					setMode(int mode);
	void					setName(std::string name);
	void					setPassword(std::string password);
	bool					isOp(int clientFd) const;
	bool					checkPassword(std::string password);
	void					addMember(int clientFd);
	void					addOp(int clientFd);
	// very wrong, banning should be done with the ip address or maybe it depends on the kind of the ban (permanent or not)
	bool					isBanned(int clientFd) const;
	// std::string			m_composeMask(Client & client) const;
	void					Ban(int clinetFd);

	bool        			getModeValue(int modeNum) const;

    void        			turnOnMode(int modeNum);
    void        			turnOffMode(int modeNum);

	static std::uint32_t	modeBitMasks[NUM_MODES_CHANNEL];
	static std::string		potentialModes;
};

#endif