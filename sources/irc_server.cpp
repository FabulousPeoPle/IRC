#include "irc_server.hpp"

Server::Server(void) : m_port(DEFAULT_PORT), m_hostname(DEFAULT_HOSTNAME)
{
    std::cout << "Default Server constructor called." << std::endl;
    this->m_servinfo = NULL;
    this->m_sockfd = -1;
    this->m_poll_count = 0;
    memset(&(this->m_socketInfo), 0, sizeof(this->m_socketInfo));
    memset(&(this->m_hints), 0, sizeof(this->m_hints));
    memset(&(this->m_addr_in), 0, sizeof(this->m_addr_in));
    memset(&(this->m_addr_in6), 0, sizeof(this->m_addr_in6));
}

Server::~Server(void)
{
    std::cout << "Default Server destructor called." << std::endl;
}

Server::Server(std::string port, std::string hostname) : m_port(port), m_hostname(hostname)
{
    std::cout << "Port/Hostname constructor called." << std::endl;
    this->m_servinfo = NULL;
    this->m_sockfd = -1;
    this->m_poll_count = 0;
    memset(&(this->m_socketInfo), 0, sizeof(this->m_socketInfo));
    memset(&(this->m_hints), 0, sizeof(this->m_hints));
    memset(&(this->m_addr_in), 0, sizeof(this->m_addr_in));
    memset(&(this->m_addr_in6), 0, sizeof(this->m_addr_in6));
}

Server::Server(const Server& serverRef)
{
    std::cout << "Copy constructor called." << std::endl;
    *this = serverRef;
}

Server&      Server::operator=(const Server& serverRef)
{
    // TODO: NEED TO ADD ALL MEMBER VARIABLES
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

std::string     Server::getPort(void)  const
{
    return (this->m_port);
}

std::string     Server::getHostname(void) const
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

void            Server::setServerHints(int family, int sockType, int flags)
{
    this->m_hints.ai_family = family;
    this->m_hints.ai_socktype = sockType;
    this->m_hints.ai_flags = flags;
}

int         Server::setServerInfo(void)
{
    int ret;

    if ((ret = getaddrinfo(this->m_hostname.c_str(), this->m_port.c_str(), &this->m_hints, &this->m_servinfo)))
    {
        std::cerr << "getaddrinfo error: " << gai_strerror(ret) << std::endl;
        return (-2);
    }
    return (0);
}

int             Server::m_setSocket(t_socketInfo socketInfo, t_sockaddr* addr, socklen_t addrlen) // tested
{
    int yes = 1;

    if ((this->m_sockfd = socket(socketInfo.family, socketInfo.socktype, socketInfo.protocol)) == -1)
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
    if ((bind(this->m_sockfd, addr, addrlen)))
    {
        perror("bind");
        return (-2);
    }
    return (0);
}

// Should check if this has already been done
int             Server::setSockfd(int family)
{
    t_addrinfo*     p;
    int             ret;

    for (p = this->m_servinfo; p != NULL; p = p->ai_next)
    {
        if ((p->ai_family && family) || family == AF_UNSPEC) // to check
        {
            this->m_socketInfo.family = p->ai_family;
            this->m_socketInfo.socktype = p->ai_socktype;
            this->m_socketInfo.protocol = p->ai_protocol;
            if ((ret = this->m_setSocket(this->m_socketInfo,
                            p->ai_addr, p->ai_addrlen)))
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

// Should check if this has already been done
int             Server::setSockfd_in(void)
{
    std::stringstream   port_value(this->m_port);
    short               port_value_store;
    int                 ret;

    port_value >> port_value_store;
    this->m_socketInfo.family = this->m_addr_in.sin_family= AF_INET;
    this->m_addr_in.sin_addr.s_addr = INADDR_ANY;
    this->m_addr_in.sin_port = htons(port_value_store);
    this->m_socketInfo.socktype = SOCK_STREAM;
    this->m_socketInfo.protocol = 0;
    if ((ret = this->m_setSocket(this->m_socketInfo, (t_sockaddr*)&this->m_addr_in6,
        sizeof(this->m_addr_in6))))
        return (-1);
    return (0);
}


// Should check if this has already been done
int             Server::setSockfd_in6(void)
{
    std::stringstream   port_value(this->m_port);
    short               port_value_store;
    int                 ret;

    port_value >> port_value_store;
    this->m_socketInfo.family = this->m_addr_in6.sin6_family = AF_INET6;
    this->m_addr_in6.sin6_addr = in6addr_any;
    this->m_addr_in6.sin6_port = htons(port_value_store);
    this->m_socketInfo.socktype = SOCK_STREAM;
    this->m_socketInfo.protocol = 0;
    if ((ret = this->m_setSocket(this->m_socketInfo, (t_sockaddr*)&this->m_addr_in6,
        sizeof(this->m_addr_in6))))
        return (-1);
    return (0);
}

int             Server::listen(void)
{
    if (::listen(this->m_sockfd, MAX_CONNECTIONS))
    {
        perror("listen: ");
        exit(-1);
    }
    this->m_pfds.push_back((t_pollfd){this->m_sockfd, POLLIN, 0});
    return (0);
}

void            Server::m_poll(void)
{
    this->m_poll_count = poll(this->m_pfds.data(), this->m_pfds.size(), 5000);
    if (this->m_poll_count == -1)
    {
        perror("poll: ");
        exit(-1);
    }
}

void*           Server::m_getInAddr(t_sockaddr* addr) const
{
    if (addr->sa_family == AF_INET)
        return (&(((t_sockaddr_in*)addr)->sin_addr));
    return (&(((t_sockaddr_in6*)addr)->sin6_addr));
}

int             Server::startServer(void)
{
    while (1)
    {
        this->m_poll();
        // std::cout << "listening" << std::endl;
        unsigned long i = 0;
        while (i < this->m_pfds.size() && this->m_poll_count)
        {
            // WE GOT A CONNECTION
            std::cout << "listening" << std::endl;
            if (this->m_pfds[i].revents & POLLIN)
            {
                // IT'S OUR SERVER
                std::cout << "we got something in fd " << this->m_pfds[i].fd << std::endl;
                if (this->m_pfds[i].fd == this->m_sockfd)
                {
                    std::cout << "new connection" << std::endl;
                    t_sockaddr_storage  remoteAddr;
                    socklen_t           addrlen = sizeof(t_sockaddr_storage);
                    int                 newFd = accept(this->m_sockfd, (t_sockaddr*)&remoteAddr,
                                                        &addrlen);
                    // this might be usless
                    // find a way to check if the connection is from a client we already have
                    if (newFd == -1)
                    {
                        perror("accept: ");
                        continue ;
                    }
                    this->m_pfds.push_back((t_pollfd){newFd, POLLIN, 0});
                    // can push a pointer instead of a copy
                    // this->m_clients.push_back(Client(newFd, remoteAddr, addrlen));
                    /* for now let's assume that a newFd is a new client
                     * we shall later check if a client connected multiple times
                     * should check for protocol, port, family, address??
                     */
                    // this->m_addToClientList(newFd, remoteAddr, addrlen);
                    // might be usless, since we won't be accepting unless poll() says we can
                    // fcntl(newFd, F_SETFL, O_NONBLOCK);
                }
                else
                {
                    // IT'S A CLIENT
                    std::cout << "receiving\n";
                    char    buffer[BUFFER_SIZE];
                    int     bytesRead = recv(this->m_pfds[i].fd, buffer, BUFFER_SIZE, 0);

                    if (bytesRead <= 0)
                    {
                        if (bytesRead == 0)
                            printf("Client disconnected.\n");
                        else
                            perror("recv: ");
                        this->m_pfds.erase(this->m_pfds.begin() + i);
                        close(this->m_pfds[i].fd);
                    }
                    else
                    {
                        buffer[bytesRead] = '\0';
                        printf("Received: [%s]\n", buffer);
                        // add to client buffer, until carriage return and new line
                    }
                }
            }
            this->m_poll_count--;
        }
        //poll
        //loop on pollcount
            //check if event occured
                //check if the even occured on listener socket
                    //if yes: accept new connection(client)
                    //if no: recv msg from client
    }
}
