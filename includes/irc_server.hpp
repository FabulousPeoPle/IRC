#ifndef IRC_SERVER_HPP
# define IRC_SERVER_HPP

#include <sstream>
#include <string>
#include <unistd.h>

#include <vector>

#include <poll.h>


#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <fcntl.h>

#define DEFAULT_HOSTNAME NULL
#define DEFAULT_PORT "6667"

#define MAX_CONNECTIONS 10

#define BUFFER_SIZE 512

typedef struct addrinfo         t_addrinfo;
typedef struct sockaddr         t_sockaddr;
typedef struct sockaddr_storage t_sockaddr_storage;
typedef struct sockaddr_in      t_sockaddr_in;
typedef struct sockaddr_in6     t_sockaddr_in6;
typedef struct pollfd           t_pollfd;  

class Client;
class Message;
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
class Server {
    public:
                                // constructors are probably going to be usless
                                Server(void);
                                Server(char* hostname, char* port);
                                Server(const Server& serverRef);
                                ~Server();
        Server&              operator=(const Server& serverRef);

        char*                   getPort(void) const;
        char*                   getHostname(void) const;
        t_addrinfo              getHints(void) const;
        t_addrinfo              getServInfo(void) const;
        int                     getSockfd(void) const;
        t_sockaddr_in6          getAddr_in6(void) const;
        t_sockaddr_in           getAddr_in(void) const;

        //  Might wanna set protection for multiple IP adresses

        int                     setServerInfo(void);
        void                    setServerHints(int family, int sockType, int flags);

        //  might upgrade it to taking socktype and protocol?
        int                     setSockfd(int family);

        int                     setSockfd_in(void);
        int                     setSockfd_in6(void);


        int                     listen(void);

        int                     startServer(void);


    private:
        //  Maybe this is usless since we are always going to connect to the same thing
        int                     m_setSocket(t_socketInfo socketInfo, t_sockaddr* addr, socklen_t addrlen);
        void                    m_poll(void);
        void*                   m_getInAddr(t_sockaddr* addr) const;
        //void                    m_fillSocketInfo(t_socketInfo& socketInfo, int family, int socktype, int protocol);
        void                    m_parse(std::string& str);
        void                    m_addToClientList(int fd, t_sockaddr_storage& remoteAddr,
                                        socklen_t addrlen);
        // void                    m_addrCompare(struct in_addr& addr, struct in_addr& addr2) const;



        const char*                   m_port;
        // Maybe this is usless since we are always going to connect to the same thing
        char*                   m_hostname;
        t_addrinfo*             m_servinfo;
        t_addrinfo              m_hints;
        // in case we wanted to do it manually
        t_sockaddr_in           m_addr_in;
        t_sockaddr_in6          m_addr_in6;
        
        int                     m_sockfd;
        // this might be totally usless
        t_socketInfo            m_socketInfo;

        int                     m_poll_count;
        std::vector<t_pollfd>   m_pfds;

        std::vector<Client>     m_clients;
};

#endif