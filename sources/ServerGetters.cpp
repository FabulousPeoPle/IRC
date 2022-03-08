
#include "Server.hpp"

std::string     Server::getServName(void) const
{
    return (this->m_serverName);
}

t_sockaddr_in   Server::getAddr_in(void) const
{
    return (this->m_addr_in);
}

t_sockaddr_in6  Server::getAddr_in6(void) const
{
    return (this->m_addr_in6);
}

int             Server::getSockfd(void) const
{
    return (this->m_sockfd);
}

std::string          Server::getPort(void)  const
{
    return ((this->m_port));
}

std::string           Server::getHostname(void) const
{
    return (this->m_hostname);
}

t_addrinfo      Server::getHints(void) const
{
    return (this->m_hints);
}

t_addrinfo      Server::getServInfo(void) const
{
    return (*(this->m_servinfo));
}
