/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ohachim <ohachim@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/14 16:51:35 by azouiten          #+#    #+#             */
/*   Updated: 2022/03/08 15:55:12 by ohachim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include "Client.hpp"

Client::Client(void) : _nickAuth(false), _userAuth(false), _passAuth(false), _isServerOp(false), _away(false), _authenticated(false)
{
	this->modes = 0;
	this->lastJoinedChannel = "";
	this->turnOnMode(UserModes::restricted);
}

Client::Client(Client const & src)
{
	if (&src != this)
		*this = src;
}

Client::Client(int sock_fd, struct sockaddr_storage addr, socklen_t len) : _sock_fd(sock_fd),
_addr(addr), _addr_size(len), _nickAuth(false), _userAuth(false), _passAuth(false), _isServerOp(false), _away(false), _authenticated(false)
{
	this->modes = 0;
	this->lastJoinedChannel = "";
	this->turnOnMode(UserModes::restricted);
}

Client::~Client(void)
{}

int		Client::getFd(void) const
{
	return (_sock_fd);
}

struct sockaddr_storage	Client::getRawAddress(void) const
{
	return (_addr);
}

socklen_t	Client::getAddrLen(void) const
{
	return (_addr_size);
}

t_messageDQeue	&Client::getMessageQueue(void)
{
	return (messages);
}

std::string	Client::getNickname(void) const
{
	return (_nickname);
}

std::string	Client::getUsername(void) const
{
	return (_username);
}

std::string	Client::getHostname(void) const
{
	return (hostname);
}

std::string	Client::getIp(void) const
{
	return (_ip_address);
}

std::string	Client::getRealname(void) const
{
	return (_realname);
}

std::string	Client::getAwayMsg(void) const
{
	return (awayMessage);
}

std::vector<t_fileData>	&Client::getFiles(void)
{
	return (recievedFiles);
}

bool		Client::isNickAuth(void) const
{
	return(_nickAuth);
}

bool		Client::isUserAuth(void) const
{
	return(_userAuth);
}

bool		Client::isPassAuth(void) const
{
	return(_passAuth);
}

bool		Client::isServerOp(void) const
{
	return(_isServerOp);
}

bool		Client::isAuthComplete(void) const
{
	return(_authenticated);
}

bool		Client::isAway(void) const
{
	return(_away);
}

void	Client::setFd(int fd)
{
	_sock_fd = fd;
}

void	Client::setRawAddress(struct sockaddr_storage addr)
{
	_addr = addr;
}

void	Client::setAddrLen(socklen_t len)
{
	_addr_size = len;
}

void	Client::pushMsg(Message &message)
{
	messages.push_back(message);
}

void	Client::pushFile(t_fileData &filedata)
{
	recievedFiles.push_back(filedata);
}

void	Client::setNickname(std::string nickname)
{
	_nickname = nickname;	
}

void	Client::setUsername(std::string username)
{
	_username = username;	
}

void	Client::setIp(std::string ip)
{
	_ip_address = ip;	
}

void	Client::setHostname(std::string hostname)
{
	this->hostname = hostname;	
}

void	Client::setRealname(std::string realname)
{
	_realname = realname;	
}

void	Client::setAwayMsg(std::string awayMessage)
{
	this->awayMessage = awayMessage;	
}

void	Client::setNickAuth()
{
	_nickAuth = true;
}

void	Client::setPassAuth()
{
	_passAuth = true;
}

void	Client::setUserAuth()
{
	_userAuth = true;
}

void	Client::setAsServerOp()
{
	_isServerOp = true;
}

void	Client::setAway()
{
	_away = true;
}

void	Client::setAuthComplete()
{
	_authenticated = true;
}

void	Client::pushChannel(std::string channelName, std::uint8_t modes)
{
	_channels.push_back(channelName);
	_channelModes[channelName] = modes;
	
}

void	Client::popChannel(std::string channelName)
{
	std::vector<std::string>::iterator it = _channels.begin();
	std::vector<std::string>::iterator end = _channels.end();

	while (it != end)
	{
		if (*it == channelName)
		{
			_channels.erase(it);
			break ;
		}
		it++;
	}
}

std::vector<std::string>        &Client::getChannels(void)
{
	return (_channels);
}

Client & Client::operator=(Client const & rhs)
{
	_sock_fd = rhs._sock_fd;
	_addr = rhs._addr;
	_addr_size = rhs._addr_size;
	_nickname = rhs._nickname;
	_username = rhs._username;
	_realname = rhs._realname;
	_userAuth = rhs._userAuth;
	_nickAuth = rhs._nickAuth;
	_ip_address = rhs._ip_address;
	_authenticated = rhs._authenticated;
	return (*this);
}

void		Client::partChannel(std::string name)
{
	std::vector<std::string>::iterator it = _channels.begin();
	std::vector<std::string>::iterator end = _channels.end();

	while (it != end)
	{
		if (*it == name)
		{
			_channels.erase(it);
			break ;
		}
		it++;
	}
}

void		Client::turnOffMode(int modeNum)
{
	this->modes &= ~(this->modeBitMasks[modeNum]);
}

void		Client::turnOnMode(int modeNum)
{
	this->modes |= this->modeBitMasks[modeNum];
}

bool		Client::getModeValue(int modeNum) const
{
	return (this->modeBitMasks[modeNum] & this->modes);
}

void		Client::turnOffMode(int modeNum, std::string channelName)
{
	this->_channelModes[channelName] &= ~(this->modeBitMasks[modeNum]);
}

void		Client::turnOnMode(int modeNum, std::string channelName)
{
	this->_channelModes[channelName] |= this->modeBitMasks[modeNum];
}

int			Client::findChanMode(char mode) const
{
	switch (mode)
    {
        case 'O':
			return (ChannelModes::O_Creator);
		case 'o':
			return (ChannelModes::o_OperatorPrivilege);
		case 'v':
			return (ChannelModes::v_voicePrivilege);
        default:
            return (-1);
    }
}

void	Client::deleteFiles(void)
{
	for (int i = 0; i < recievedFiles.size(); ++i)
	{
		free(recievedFiles[i].content);
		recievedFiles[i].content = NULL;
	}
	recievedFiles.clear();
}

int     Client::findMode(char c)
{
    switch (c)
    {
        case 'a':
            return (UserModes::away);
        case 'i':
            return (UserModes::invisible);
        case 'w':
            return (UserModes::wallops);
        case 'r':
            return (UserModes::restricted);
        case 'o':
            return (UserModes::oper);
        case 's':
            return (UserModes::serverNotices);           
        default:
            return (-1);
    }
}
bool		Client::getModeValue(int modeNum,  const std::string channelName)
{
	return (this->modeBitMasks[modeNum] & this->_channelModes[channelName]);
}

std::string                     Client::getLastJoinedChannel(void)
{
	if (this->lastJoinedChannel.empty())
		return ("*");
	return (this->lastJoinedChannel);
}


std::uint8_t        Client::modeBitMasks[NUM_MODES] = {1 << 0, 1 << 1, 1 << 2, 1 << 3, 1 << 4, 1 << 5};
std::string         Client::potentialModes = "aiwros";
std::string			Client::potentialChannelModes = "Oov";