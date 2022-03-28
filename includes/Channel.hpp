
#ifndef _CHANNEL_
#define _CHANNEL_

#include <iostream>
#include <vector>

#include <Client.hpp>

#define LOCAL_CHAN '&'
#define NETWORKWIDE_CHAN '#'
#define CHANNEL_NUMBER_LIMIT 10


#define NUM_MODES_CHANNEL 0xa

class Client;

namespace ChannelModes
{
    enum
    {
		/******-UserSpecific-****/
		O_Creator,
        o_OperatorPrivilege,
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
		k_passProtected,
		l_limitUser,
        t_topicOperatorOnly,
		/************************/

		total
    };
};

class Channel
{
private:
	std::string					m_name;
	int							m_mode;
	int							m_type;
	std::vector<int>			m_operators;
	std::string					m_topic;
	std::vector<int>			m_members;
	std::vector<int>			m_invited;
	std::uint16_t               modes;

	std::string					m_creatorNick;

	std::vector<std::string>	m_banMasks;
	std::vector<std::string>	m_exceptionBanMasks;
	std::vector<std::string>	m_inviteMasks;

	std::string					m_password;
	int							m_userLimit;

	std::string				m_extractTLD(std::string mask);

public:
						Channel(void);
						Channel(int mode, int opFd, std::string name, char type, std::string password);
						Channel(const Channel& channelRef);
						~Channel(void);

	Channel&			operator=(const Channel& channelRef);

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
	
	bool						isOp(int clientFd) const;
	bool						checkPassword(std::string password);
	void						addMember(int clientFd);
	void						addInvited(int clientFd);
	void						removeMember(int clientFd);
	void						removeOp(int clientFd);
	void						removeInvited(int clientFd);
	void						addOp(int clientFd);
	bool						isBanned(Client &client);
	bool						isMember(int clientFd) const;
	bool						isInvited(int clientFd) const;
	
	int							m_getUserLimit(void) const;

	bool        				getModeValue(int modeNum) const;

	int 	    				findMode(char c) const;
	void							m_addToMaskVector(std::vector<std::string>& dst, std::string src);
	int							manageAttribute(char mode, char prefix,
								std::vector<std::string> arguments, std::string& modeChanges);
	void						manageSimpleMode(char mode, char prefix, std::string& modeChanges);
    void        			turnOnMode(int modeNum);
    void        			turnOffMode(int modeNum);
	bool					isInMaskVector(Client &client, std::vector<std::string>& maskVector);
	bool            		m_isMaskUserMatch(std::string hostname, std::string TLD);
	
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