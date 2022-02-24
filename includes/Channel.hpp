/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ohachim <ohachim@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/07 12:12:16 by azouiten          #+#    #+#             */
/*   Updated: 2022/02/24 15:30:18 by ohachim          ###   ########.fr       */
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
//TODO: COPLIEN FORM

class Channel
{
private:
	std::string					m_name; // cant be 7 ' ' or ',' anything else id fair game
	int							m_mode; // change
	int							m_type; // the scope of the channel local/networkwide
	std::vector<int>			m_operators; // this vector stores the clientfds of the ops
	std::string					m_topic;
	std::vector<int>			m_members;
	std::vector<int>			m_invited;
	std::uint16_t               modes; // need to give em default modes

	std::string					m_creatorNick;

	std::vector<std::string>	m_banMasks;
	std::vector<std::string>	m_exceptionBanMasks;
	std::vector<std::string>	m_inviteMasks;

	std::string					m_password; // boolean
	int							m_userLimit; // initialized to -1

	std::string				m_extractTLD(std::string mask);

public:
						Channel(void);
						Channel(int mode, int opFd, std::string name, char type, std::string password);
						Channel(const Channel& channelRef);
						~Channel(void);

	Channel&			operator==(const Channel& channelRef);

	std::vector<std::string>&	getBanMasks(void);
	std::vector<std::string>&	getExceptionBanMasks(void);
	std::vector<std::string>&	getInviteMasks(void);

	std::string					getCreatorName(void) const;

	
	std::string 				getName(void) const;
	std::string 				getPassword(void) const;
	int							getMode(void) const;
	std::vector<int> 			&getOps(void);
	std::vector<int> 			&getMembers(void);
	std::vector<int> 			&getInvited(void);
	int			 				getType(void) const;
	std::string&				getTopic(void);
	
	void						setTopic(std::string topic);
	void						setMode(int mode);
	void						setName(std::string name);
	void						setPassword(std::string password);
	void						setCreatorNick(std::string nickname);
	
	bool				isOp(int clientFd) const;
	bool				checkPassword(std::string password);
	void				addMember(int clientFd);
	void				addInvited(int clientFd);
	void				removeMember(int clientFd);
	void				removeOp(int clientFd);
	void				removeInvited(int clientFd);
	void				addOp(int clientFd);
	bool				isBanned(Client &client);
	bool				isMember(int clientFd) const;
	bool				isInvited(int clientFd) const;
	// std::string			m_composeMask(Client & client) const;

	bool        			getModeValue(int modeNum) const;

	int 	    			findMode(char c) const;

	void				m_addToChanVector(std::vector<std::string>& dst, std::string src);
	int							manageAttribute(char mode, char prefix,
								std::vector<std::string> arguments);
	void						manageSimpleMode(char mode, char prefix);
    void        			turnOnMode(int modeNum);
    void        			turnOffMode(int modeNum);
	bool            		m_isMaskUserMatch(std::string hostname, std::string TLD) const // maybe it should be a friend function
		{
		    hostname = hostname.erase(0, hostname.size() - TLD.size());
		    return (hostname == TLD || TLD.empty());
		}

	static std::uint16_t	modeBitMasks[NUM_MODES_CHANNEL];
	static std::string		potentialModes;

	template <typename T>
        void    printVector(T &vector)
        {
            typename T::iterator it = vector.begin();
            typename T::iterator end = vector.end();
            std::cout << "printing vector\n";
            while (it != end)
            {
                std::cout << "|" << *it << "|" << std::endl;
                it++;
            }
        }
};

#endif