/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azouiten <azouiten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/14 16:51:38 by azouiten          #+#    #+#             */
/*   Updated: 2022/02/16 13:18:44 by azouiten         ###   ########.fr       */
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

namespace UserModes
{
    enum
    {
        away,
        invisible,
        wallops,
        restricted,
        oper,
        server_notices
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
        int                         _sock_fd;
        struct sockaddr_storage     _addr;
        socklen_t                   _addr_size;
        std::string                 _nickname;
        std::string                 _username;
        std::string                 hostname;
        std::string                 _ip_address;
        t_messageDQeue              messages;
        std::string                 _realname;
        bool                        _nickAuth;
        bool                        _userAuth;
        bool                        _isServerOp;
        bool                        _away;
        bool                        _authenticated;
        std::uint8_t                modes; // need to give em default modes
        static std::uint8_t         modeBitMasks[NUM_MODES];
        static std::string          potentialModes;
        std::string                 awayMessage;
        
        std::vector<std::string>    _channels;
    
    public:


        
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
        void                            turnOffMode(int modeNum);
        void                            pushChannel(std::string channelName);
        void                            popChannel(std::string channelName);
        
        
        
        Client & operator=(Client const & rhs);


        void        partChannel(std::string channelName);
};

#endif
