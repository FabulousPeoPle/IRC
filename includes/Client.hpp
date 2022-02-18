/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ohachim <ohachim@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/14 16:51:38 by azouiten          #+#    #+#             */
/*   Updated: 2022/02/18 20:05:39 by ohachim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include  <vector>
#include <map>
#include <sys/socket.h>
#include <deque>
#include "Message.hpp"
#include <cstdint>
#include <cstring>

class Message;

#define NUM_MODES 6

#define OWNER_MODES 0x7

#define PEASEANT_MODES 0x0

namespace UserModes
{
    enum
    {
        away,
        invisible,
        wallops,
        restricted,
        oper,
        serverNotices
    };
};

typedef std::deque<Message> t_messageDQeue;
class Client
{

    // TODO: must add variable representing unique identifier
    // TODO: variable representing server that the client is connected too
    // TODO: if the client is a USER, we must know the name of the host it's connected top and the username of the user?? wtf is that
    // TODO: if it's a service we must know the name of the service, which is composed of the nickname and the servername
    // TODO: if it's a service, we must know the type of the service
    // services and users do not have the same privelages, example: services can request part of the global state information(if not all of it), they cannot access channels, users can access channels
    // TODO: what the fuck is the flood control mechanism?
    // TODO: generator QUIT message if the connection breaks
    private:
        int                                 _sock_fd;
        struct sockaddr_storage             _addr;
        socklen_t                           _addr_size;
        std::string                         _nickname;
        std::string                         _username;
        std::string                         hostname;
        std::string                         _ip_address;
        t_messageDQeue                      messages;
        std::string                         _realname;
        bool                                _nickAuth;
        bool                                _userAuth;
        bool                                _isServerOp;
        bool                                _away;
        bool                                _authenticated;
        std::uint8_t                        modes; // need to give em default modes
        static std::uint8_t                 modeBitMasks[NUM_MODES];
        static std::string                  potentialModes;
        static std::string			        potentialChannelModes;
        std::string                         awayMessage;
        
        std::vector<std::string>            _channels; // TODO: Maybe remove later
    
    public:

        std::map<std::string, std::uint8_t> _channelModes; // TODO: put it in private and make a getter
        Client(void);
        Client(Client const & src);
        Client(int, struct sockaddr_storage, socklen_t);
        ~Client(void);

        /////////////////
        //// GETTERS ////
        /////////////////
        int         getFd(void) const;
        struct sockaddr_storage         getRawAddress(void) const;
        socklen_t                       getAddrLen(void) const;
        t_messageDQeue                  &getMessageQueue(void);
        std::string                     getNickname(void) const;
        std::string                     getUsername(void) const;
        std::string                     getHostname(void) const;
        std::string                     getIp(void) const;
        std::string                     getRealname(void) const;
        std::string                     getAwayMsg(void) const;
        bool                            isNickAuth(void) const;
        bool                            isUserAuth(void) const;
        bool                            isServerOp(void) const;
        bool                            isAway(void) const;
        bool                            isAuthComplete(void) const;
        bool                            getModeValue(int modeNum) const;
        bool		                    getModeValue(int modeNum, std::string channelName);

        std::vector<std::string>        &getChannels(void);
        
        /////////////////
        //// SETTERS ////
        /////////////////
        void                            setFd(int fd);
        void                            setRawAddress(struct sockaddr_storage addr);
        void                            setAddrLen(socklen_t addrLen);
        void                            pushMsg(Message & message);
        void                            setNickname(std::string nickname);
        void                            setUsername(std::string username);
        void                            setIp(std::string ip);
        void                            setHostname(std::string hostname);
        void                            setRealname(std::string realname);
        void                            setAwayMsg(std::string awayMsg);
        void                            setNickAuth(void);
        void                            setUserAuth(void);
        void                            setAsServerOp(void);
        void                            setAway(void);
        void                            setAuthComplete(void);
        void                            turnOnMode(int modeNum);
        void		                    turnOnMode(int modeNum, std::string channelName);
        void                            turnOffMode(int modeNum);
        void		                    turnOffMode(int modeNum, std::string channelName);
        void                            pushChannel(std::string channelName, std::uint8_t modes);
        void                            popChannel(std::string channelName);
        void                            manageChannelModes(char mode, char prefix, std::vector<std::string> arguments, int paramToUseIndex);

        Client & operator=(Client const & rhs);

    ///////////////
    /// METHODS ///
    ///////////////
        void        partChannel(std::string channelName);
};

#endif
