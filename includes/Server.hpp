/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azouiten <azouiten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/11 16:41:32 by ohachim           #+#    #+#             */
/*   Updated: 2022/01/12 17:56:24 by azouiten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef _SERVER_HPP_
# define _SERVER_HPP_

#include <sstream>
#include <string>
#include <iostream>

#include <unistd.h>
#include <fcntl.h>

#include <vector>
#include <map>

#include <algorithm>

#include <poll.h>

#include <Client.hpp>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>


#define DEFAULT_HOSTNAME NULL
#define DEFAULT_PORT "6667"

#define MAX_CONNECTIONS 10

#define BUFFER_SIZE 512

#define END_STRING "\r\n"

// #define BIG_NUMBER 2000000000

namespace Replies
{
    enum
    {
        RPL_WELCOME = 1,
        ERR_NICKNAMEINUSE = 433,
        ERR_ERRONEUSNICKNAME = 423,
        RPL_USERHOST = 302,
        ERR_NEEDMOREPARAMS = 461,
        ERR_ALREADYREGISTRED = 462,
    };
};


#define USER_COMMAND "USER"
#define NICK_COMMAND "NICK"
#define PASS_COMMAND "PASS"
#define USERHOST_COMMAND "USERHOST"
#define QUIT_COMMAND "QUIT"

typedef struct addrinfo         t_addrinfo;
typedef struct sockaddr         t_sockaddr;
typedef struct sockaddr_storage t_sockaddr_storage;
typedef struct sockaddr_in      t_sockaddr_in;
typedef struct sockaddr_in6     t_sockaddr_in6;
typedef struct pollfd           t_pollfd;

std::string strToken(std::string str);

class Client;
class Message;
class Server;
// Add this one to client class.
typedef struct      s_m_socketInfo 
{
    int family;
    int socktype;
    int protocol;
}                   t_socketInfo;

// TODO: SET AN ENUM FOR ERRORS
// TODO: ADD EXCEPTIONS
// TODO: THROW EXCEPTIONS INSTEAD OF EXITING?
// TODO: change inet_ntoa in startServer(), only converts from IPv4, we need to be able to convert from IPv6
// TODO: BIT masking for user modes
class Server {
    public:
        // typedef void (*commandFunc)(Client&);
                                    // constructors are probably going to be useless
                                        Server(void);
                                        Server(std::string port, std::string hostname, std::string serverName);
                                        Server(const Server& serverRef);
                                        ~Server();
        Server&                         operator=(const Server& serverRef);

        std::string                     getPort(void) const;
        std::string                     getHostname(void) const;
        t_addrinfo                      getHints(void) const;
        t_addrinfo                      getServInfo(void) const;
        int                             getSockfd(void) const;
        t_sockaddr_in6                  getAddr_in6(void) const;
        t_sockaddr_in                   getAddr_in(void) const;

        //  Might wanna set protection for multiple IP adresses

        std::string                     getServName(void) const;
        int                             setServerInfo(void);
        void                            setServerHints(int family, int sockType, int flags);

        //  might upgrade it to taking socktype and protocol?
        int                             setSockfd(int family);

        int                             setSockfd_in(void);
        int                             setSockfd_in6(void);


        int                             listen(void);

        int                             startServer(void);

    private:

        bool                            m_isAuthenticated(int clientFd);

        void                            m_managePoll(void);
        int                             m_manageServerEvent(void);
        void                            m_manageClientEvent(int pollIndex);
        //  Maybe this is usless since we are always going to connect to the same thing
        int                             m_setSocket(t_socketInfo socketInfo, t_sockaddr* addr, socklen_t addrlen);
        void                            m_poll(void);
        void*                           m_getInAddr(t_sockaddr* addr) const;
        //void                            m_fillSocketInfo(t_socketInfo& socketInfo, int family, int socktype, int protocol);
        void                            m_parse(std::string& str);
        int                             m_manageRecv(std::string message, int clientFd);
        
        bool                            m_checkStatusAuth(Client& client);
        bool                            m_tryAuthentificate(Client& client);
        
        void                            m_userCmd(Client & client);
        void                            m_nickCmd(Client & client);
        void                            m_userhostCmd(Client & client);
        
        void                            m_relay(int clientFd);
        void                            m_debugAuthentificate(int clientFd);
        int                             m_send(int toFd, std::string message);

        void                            m_reply(int clientFd, int replyCode, int extraArg);
        void                            m_setCommandFuncs(void);

        bool                            m_checkNickSyntax(Message& message);
        void                            m_eraseClientPoll(int clientFd);

        void                            m_quitCmd(int clientFd, std::string quitMessage);
        // Need to know more about channel class
        void                            m_join(int channelNum);



        const std::string               m_serverName;
        const std::string               m_port;
        // Maybe this is usless since we are always going to connect to the same thing
        const std::string               m_hostname;
        t_addrinfo*                     m_servinfo;
        t_addrinfo                      m_hints;
        // in case we wanted to do it manually
        t_sockaddr_in                   m_addr_in;
        t_sockaddr_in6                  m_addr_in6;
        
        int                             m_sockfd;
        // this might be totally usless
        t_socketInfo                    m_socketInfo;

        int                             m_poll_count;
        std::vector<t_pollfd>           m_pfds;

        std::map<int, Client>           m_clients;
        
        // the key is the nickname itself and the value is the clientfd
        std::map<std::string, int>      m_nicknames;
};

#endif