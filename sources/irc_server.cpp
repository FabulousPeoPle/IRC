#include "irc_server.hpp"

IRCServer::IRCServer(void) : m_port(DEFAULT_PORT), m_hostname(DEFAULT_HOSTNAME)
{
    std::cout << "Default IRCServer constructor called." << std::endl;
    this->m_servinfo = NULL;
    this->m_sockfd = -1;
    memset(&(this->m_hints), 0, sizeof(this->m_hints));
    memset(&(this->m_addr_in), 0, sizeof(this->m_addr_in));
    memset(&(this->m_addr_in6), 0, sizeof(this->m_addr_in6));
}

IRCServer::~IRCServer(void)
{
    std::cout << "Default IRCServer destructor called." << std::endl;
}

IRCServer::IRCServer(std::string port, std::string hostname) : m_port(port), m_hostname(hostname)
{
    std::cout << "Port/Hostname constructor called." << std::endl;
    this->m_servinfo = NULL;
    this->m_sockfd = -1;
    memset(&(this->m_hints), 0, sizeof(this->m_hints));
    memset(&(this->m_addr_in), 0, sizeof(this->m_addr_in));
    memset(&(this->m_addr_in6), 0, sizeof(this->m_addr_in6));
}

IRCServer::IRCServer(const IRCServer& serverRef)
{
    std::cout << "Copy constructor called." << std::endl;
    *this = serverRef;
}

IRCServer&      IRCServer::operator=(const IRCServer& serverRef)
{
    this->m_port = serverRef.getPort();
    this->m_hostname = serverRef.getHostname();
    this->m_hints = serverRef.getHints();
    // should it be a deep copy?
    *(this->m_servinfo) = serverRef.getServInfo();
    this->m_sockfd = serverRef.getSockfd();
    this->m_addr_in6 = serverRef.getAddr_in6();
    this->m_addr_in = serverRef.getAddr_in();
    return (*this);
}

t_sockaddr_in   IRCServer::getAddr_in(void) const
{
    return (this->m_addr_in);
}

t_sockaddr_in6  IRCServer::getAddr_in6(void) const
{
    return (this->m_addr_in6);
}

int             IRCServer::getSockfd(void) const
{
    return (this->m_sockfd);
}

std::string     IRCServer::getPort(void)  const
{
    return (this->m_port);
}

std::string     IRCServer::getHostname(void) const
{
    return (this->m_hostname);
}

t_addrinfo      IRCServer::getHints(void) const
{
    return (this->m_hints);
}

t_addrinfo      IRCServer::getServInfo(void) const
{
    return (*(this->m_servinfo));
}

void            IRCServer::setServerHints(int family, int sockType, int flags)
{
    this->m_hints.ai_family = family;
    this->m_hints.ai_socktype = sockType;
    this->m_hints.ai_flags = flags;
}

int         IRCServer::setServerInfo(void)
{
    int ret;

    if ((ret = getaddrinfo(this->m_hostname.c_str(), this->m_port.c_str(), &this->m_hints, &this->m_servinfo)))
    {
        std::cerr << "getaddrinfo error: " << gai_strerror(ret) << std::endl;
        return (-2);
    }
    return (0);
}


int             IRCServer::m_setSocket(t_addrinfo* p)
{
    int yes = 1;

    if ((this->m_sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
    {
        perror("socket:");
        return (-1);
    }
    // allowing a port to be reused, unless there is a socket already listening to the port
    if (setsockopt(this->m_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)))
    {
        perror("setsocketopt:");
        // is freeing necessary before exit()?
        freeaddrinfo(this->m_servinfo);
        exit(1);
    }
    if ((bind(this->m_sockfd, p->ai_addr, p->ai_addrlen)))
    {
        perror("bind:");
        return (-2);
    }
    return (0);
}

int             IRCServer::setSockfd(int family)
{
    t_addrinfo* p;
    int         ret;

    for (p = this->m_servinfo; p != NULL; p = p->ai_next)
    {
        if ((p->ai_family && family) || family == AF_UNSPEC) // to check
        {
            if ((ret = this->m_setSocket(p)))
                continue ;
        }
        break ;
    }

    if (!p)
    {
        // Might add an enum for errors
        std::cerr << "Couldn't find a socket." << std::endl;
        return (-1);
    }

    return (0);
}

int             IRCServer::setSockfd_in(void)
{
    
}