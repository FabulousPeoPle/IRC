#ifndef IRC_SERVER_HPP
# define IRC_SERVER_HPP

#include <sstream>

#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#define DEFAULT_PORT "6667"
#define DEFAULT_HOSTNAME "127.0.0.1"

typedef struct addrinfo     t_addrinfo;
typedef struct sockaddr     t_sockaddr;
typedef struct sockaddr_in  t_sockaddr_in;
typedef struct sockaddr_in6 t_sockaddr_in6;

typedef struct sockaddr_storage t_sockaddr_storage;

typedef struct      s_m_socketInfo
{
    int family;
    int socktype;
    int protocol;
}                   t_m_socketInfo;

class IRCServer {
    public:
                            // constructors are probably going to be usless
                            IRCServer(void);
                            IRCServer(std::string hostname, std::string port);
                            IRCServer(const IRCServer& serverRef);
                            ~IRCServer();
        IRCServer&          operator=(const IRCServer& serverRef);

        std::string         getPort(void) const;
        std::string         getHostname(void) const;
        t_addrinfo          getHints(void) const;
        t_addrinfo          getServInfo(void) const;
        int                 getSockfd(void) const;
        t_sockaddr_in6      getAddr_in6(void) const;
        t_sockaddr_in       getAddr_in(void) const;

        //  Might wanna set protection for multiple IP adresses

        int                 setServerInfo(void);
        void                setServerHints(int family, int sockType, int flags);

        //  might upgrade it to taking socktype and protocol?
        //  Need to set m_servinfo first
        //  must check the condition for fd
        int                 setSockfd(int family);
        int                 setSockfd_in(void);
        int                 setSockfd_in6(void);


        int                 IRCListen(void);

        int                 IRCAccept(void);


    private:
        //  Maybe this is usless since we are always going to connect to the same thing
        int                 m_setSocket(t_m_socketInfo socketInfo, t_sockaddr* addr, socklen_t addrlen);
        void                m_fillSocketInfo(t_m_socketInfo& socketInfo, int family, int socktype, int protocol);


        std::string         m_port;
        //  Maybe this is usless since we are always going to connect to the same thing
        std::string         m_hostname;
        t_addrinfo*         m_servinfo;
        t_addrinfo          m_hints;
        // in case we wanted to do it manually
        t_sockaddr_in       m_addr_in;
        t_sockaddr_in6      m_addr_in6;
        int                 m_sockfd;
        // this might be totally usless
        t_m_socketInfo      socketInfo;
};

#endif