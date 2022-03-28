
#include "Channel.hpp"

Channel::Channel(void) : m_topic(""), m_password(""), m_userLimit(-1){}
Channel::Channel(int mode, int opFd, std::string name, char type, std::string password) : m_name(name), m_mode(mode), m_type(type), m_password(password)
{
	modes = 0;
	m_operators.push_back(opFd);
	m_topic = "";
	m_userLimit = -1;
	turnOnMode(ChannelModes::t_topicOperatorOnly);
}

Channel::Channel(const Channel& channelRef)
{
	*this = channelRef;
}

Channel&	Channel::operator=(const Channel& channelRef)
{
	m_name = channelRef.m_name;
	m_mode = channelRef.m_mode;
	m_type = channelRef.m_type;
	m_operators = channelRef.m_operators;
	m_topic = channelRef.m_topic;
	m_members = channelRef.m_members;
	m_invited = channelRef.m_invited;
	modes = channelRef.modes;
	m_banMasks = channelRef.m_banMasks;
	m_exceptionBanMasks = channelRef.m_exceptionBanMasks;
	m_inviteMasks = channelRef.m_inviteMasks;
	m_password = channelRef.m_password;
	m_userLimit = channelRef.m_userLimit;
	m_creatorNick = "URMOM";
	return (*this);
}

Channel::~Channel(void)
{}

std::string  Channel::getName(void) const
{
	return (m_name);
}

std::string  Channel::getPassword(void) const
{
	return (m_password);
}

std::string&  Channel::getTopic(void)
{
	return (m_topic);
}

int  Channel::getMode(void) const
{
	return (m_mode);
}

int  Channel::getType(void) const
{
	return (m_type);
}

std::vector<int> &Channel::getOps(void)
{
	return (m_operators);
}

std::vector<int> &Channel::getMembers(void)
{
	return (m_members);
}

std::vector<int> &Channel::getInvited(void)
{
	return (m_invited);
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

void	Channel::addInvited(int clientFd)
{
	m_invited.push_back(clientFd);
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

bool Channel::isInvited(int clientFd) const
{
	return (((std::find(m_invited.begin(), m_invited.end(), clientFd) != m_invited.end()) ? true : false));
}

std::string	Channel::m_extractTLD(std::string mask)
{
	if (mask == "*")
		return ("");
	else
		return (mask.erase(0, 5));
}

int Channel::m_getUserLimit(void) const
{
	return (m_userLimit);
}

bool            		Channel::m_isMaskUserMatch(std::string hostname, std::string TLD)
{
    hostname = hostname.erase(0, hostname.size() - TLD.size());
    return (hostname == TLD || TLD.empty());
}

bool Channel::isInMaskVector(Client &client, std::vector<std::string>& maskVector)
{
	std::vector<std::string>::const_iterator it = maskVector.begin();
	std::vector<std::string>::const_iterator end = maskVector.end();
	while (it != end)
	{
		if (m_isMaskUserMatch(client.getHostname(), m_extractTLD(*it)))
			return (true);
		it++;
	}
	return (false);
}

bool Channel::isBanned(Client &client)
{
	std::vector<std::string>::const_iterator it = m_banMasks.begin();
	std::vector<std::string>::const_iterator end = m_banMasks.end();
	while (it != end)
	{
		if (m_isMaskUserMatch(client.getHostname(), m_extractTLD(*it)))
			return (true);
		it++;
	}
	return (false);
}

void	Channel::removeMember(int clientFd)
{
	std::vector<int>::iterator it = m_members.begin();
	std::vector<int>::iterator end = m_members.end();
	while (it != end)
	{
		if (*it == clientFd)
		{
			m_members.erase(it);
			break;
		}
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
		{
			m_operators.erase(it);
			break;
		}
		it++;
	}
}

void	Channel::removeInvited(int clientFd)
{
	std::vector<int>::iterator it = m_invited.begin();
	std::vector<int>::iterator end = m_invited.end();
	
	while (it != end)
	{
		if (*it == clientFd)
			m_invited.erase(it);
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

int 	    Channel::findMode(char mode) const
{
    switch (mode)
    {
        case 'a':
            return (ChannelModes::a_annonymous);
        case 'i':
            return (ChannelModes::i_inviteOnly);
        case 'm':
            return (ChannelModes::m_moderated);
        case 'n':
            return (ChannelModes::n_noOutsideMessage);
        case 'q':
            return (ChannelModes::q_quiet);
        case 'p':
            return (ChannelModes::p_private);
		case 's':
            return (ChannelModes::s_secret);
	    case 't':
            return (ChannelModes::t_topicOperatorOnly);
        default:
            return (-1);
    }
}

bool		Channel::getModeValue(int modeNum) const
{
	return (this->modeBitMasks[modeNum] & this->modes);
}

void                                Channel::m_addToMaskVector(std::vector<std::string>& dst, std::string src)
{
    if (src == "*!*@*" || src == "0" || src == "*")
		dst.push_back("*");
    else if (src.substr(0, 6) == "*!*@*." && src.size() > 6)
        dst.push_back(src.erase(0, 5));
}

int					Channel::manageAttribute(char mode, char prefix, std::vector<std::string> arguments, std::string& modeChanges)
{
	if (mode == 'k')
	{
		if (prefix == '-')
		{
			if (!m_password.empty())
				if (arguments[2] == m_password)
				{
					m_password = "";
					turnOffMode(ChannelModes::k_passProtected);
					modeChanges += "k";
				}
		}
		else
		{
			m_password = arguments[2];
			turnOnMode(ChannelModes::k_passProtected);
			modeChanges += "k";
		}
	}
	else
	{
		if (prefix == '-')
		{
			if (m_userLimit != -1)
			{
				modeChanges += 'l';
				turnOffMode(ChannelModes::l_limitUser);
				m_userLimit = -1;
			}
		}
		else
		{
			try
			{
				m_userLimit = std::stoi(arguments[2]);
				if (m_userLimit > 0)
				{
					modeChanges += 'l';
					turnOnMode(ChannelModes::l_limitUser);
				}
				else
					m_userLimit = -1;
			}
			catch (std::exception& e)
			{
				std::cout << "Bad argument: " << e.what() << std::endl;
				m_userLimit = -1;
				return (-1);
			}
		}
	}
	return (0);
}

std::vector<std::string>&	Channel::getBanMasks(void)
{
	return (m_banMasks);
}

std::vector<std::string>&	Channel::getExceptionBanMasks(void)
{
	return (m_exceptionBanMasks);
}

std::vector<std::string>&	Channel::getInviteMasks(void)
{
	return (m_inviteMasks);
}

std::string			Channel::getCreatorName(void) const
{
	return (m_creatorNick);
}

void						Channel::setCreatorNick(std::string nickname)
{
	m_creatorNick = nickname;
}

void				Channel::manageSimpleMode(char c, char prefix, std::string& modeChanges)
{
	int modeNum = this->findMode(c);
	bool	modeNumValue = this->getModeValue(modeNum);
	if (prefix == '+')
	{
		if (!modeNumValue)
		{
			if ((modeNum == ChannelModes::s_secret && this->getModeValue(ChannelModes::p_private))
				|| (modeNum == ChannelModes::p_private && this->getModeValue(ChannelModes::s_secret)))
				return ;
			modeChanges += c;
			this->turnOnMode(modeNum);
		}
	}
	else
	{
		if (modeNumValue)
		{
			modeChanges += c;
			this->turnOffMode(modeNum);
		}
	}
}

std::uint16_t       Channel::modeBitMasks[NUM_MODES_CHANNEL] = {1 << 0, 1 << 1, 1 << 2, 1 << 3, 1 << 4, 1 << 5, 1 << 6,
																	1 << 7, 1 << 8, 1 << 9};
std::string         Channel::potentialModes = "aimnqpsklt";
