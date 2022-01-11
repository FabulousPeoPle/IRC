/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ohachim <ohachim@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/10 15:03:13 by azouiten          #+#    #+#             */
/*   Updated: 2022/01/11 16:38:31 by ohachim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

char* strdup(const char *s);


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

Server::Server(std::string port, std::string hostname, std::string name) : m_serverName(name), m_port(port), m_hostname(hostname)
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

Server&         Server::operator=(const Server& serverRef)
{
    // TODO: what should I copy
    // should it be a deep copy?
    (void)serverRef;
    return (*this);
}

void            Server::setServerHints(int family, int sockType, int flags)
{
    this->m_hints.ai_family = family;
    this->m_hints.ai_socktype = sockType;
    this->m_hints.ai_flags = flags;
}

int             Server::setServerInfo(void)
{
    int ret;
    char* hostname;

    if (this->m_hostname.empty())
        hostname = NULL;
    else
        hostname = strdup(this->m_hostname.c_str());
    if ((ret = getaddrinfo(NULL, this->m_port.c_str(), &this->m_hints, &this->m_servinfo)))
    {
        std::cerr << "getaddrinfo error: " << gai_strerror(ret) << std::endl;
        free(hostname);
        return (-1);
    }
    free(hostname);
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
        if (p->ai_family == family || family == AF_UNSPEC) // to check
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
    if ((ret = this->m_setSocket(this->m_socketInfo, (t_sockaddr*)&this->m_addr_in,
        sizeof(this->m_addr_in))))
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
    this->m_poll_count = poll(this->m_pfds.data(), this->m_pfds.size(), -1);
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

int            Server::m_manageServerEvent(void)
{
    printf("something happening\n");
    t_sockaddr_storage  remoteAddr;
    socklen_t           addrlen = sizeof(t_sockaddr_storage);
    int                 newFd = accept(this->m_sockfd, (t_sockaddr*)&remoteAddr,
                                        &addrlen);
    if (newFd == -1)
    {
        perror("accept: ");
        return (-1);
    }
    this->m_clients[newFd] = Client(newFd, remoteAddr, addrlen);
    this->m_pfds.push_back((t_pollfd){newFd, POLLIN, 0});
    return (0);
}

bool            Server::m_isAuthenticated(int clientFd)
{
    return (this->m_clients[clientFd]._authenticated);
}

void                Server::m_relay(int clientFd)
{
    Client& client = this->m_clients[clientFd];

    Message message = client.messages.front();

    message.parse();

    std::cout << message.command << " this is the command\n";
    if (message.command == "QUIT")
        this->m_quit(clientFd, message.arguments[0]);
    // a map that has command function as values and the string command as key
}

// void                Server::m_setCommandFuncs(void)
// {
//     this->m_commands["QUIT"] = &(this->m_quit);

// }

void                Server::m_manageClientEvent(int pollIndex)
{
    // I have to error management message size
    char    buffer[BUFFER_SIZE];
    int     bytesRead = recv(this->m_pfds[pollIndex].fd, buffer, BUFFER_SIZE, 0);

    if (bytesRead <= 0)
    {
        if (bytesRead == 0)
            printf("Client disconnected.\n");
        else
            perror("recv: ");
        this->m_pfds.erase(this->m_pfds.begin() + pollIndex);
        m_clients.erase(this->m_pfds[pollIndex].fd);
        close(this->m_pfds[pollIndex].fd);
    }
    else
    {
        buffer[bytesRead] = '\0';
        // send(this->m_pfds[pollIndex].fd, ":555 001 ohachim :welcome\r\n", 86, 0);
        if (this->m_manageRecv(buffer, this->m_pfds[pollIndex].fd))
        {
            if (this->m_isAuthenticated(this->m_pfds[pollIndex].fd))
            {             
                this->m_relay(this->m_pfds[pollIndex].fd);
                // this->m_reply(this->m_pfds[pollIndex].fd);
                // this->m_clients[this->m_pfds[pollIndex].fd].msg._messageQueue.pop_front();
            }
            else
            {
                std::cout << "client not authenticated" << std::endl;
                // this->m_debugAuthentificate(this->m_pfds[pollIndex].fd);
                m_tryAuthentificate(m_clients[this->m_pfds[pollIndex].fd]);
            }
        }
    }
}

bool    Server::m_checkNickSyntax(Message& message)
{
    std::string & nick = message.arguments.front();
    int len = nick.length();
    
    // pending research on the restrictions demanded
    if (len > 9 || len < 1)
        return (false);
    return (true);
}

bool    Server::m_tryAuthentificate(Client& client)
{
    std::string mode;
    Message& msg = client.messages.front();
    
    msg.parse();
    std::cout << client.messages.front().command << std::endl;
    if (msg.command == NICK_COMMAND && !client._nickAuth)
    {
        m_checkNickSyntax(msg);
        std::pair<std::map<std::string, int>::iterator , bool> nick =\
        m_nicknames.insert(std::make_pair(msg.arguments.front(), 0));
        if (nick.second)
        {
            client.messages.pop_front();
            client._nickname = msg.arguments.front();
            client._nickAuth = true;
        }
        else
            m_reply(client._sock_fd, Replies::ERR_NICKNAMEINUSE);
    }
    msg = client.messages.front();
    msg.parse();
    if (msg.command == USER_COMMAND && !client._userAuth)
    {
        client._username = msg.arguments[0];
        mode = msg.arguments[1];
        client._realname = msg._literalMsg;
        client._userAuth = true;
        client.messages.pop_front();
    }
    if (client._userAuth && client._nickAuth)
    {
        m_reply(client._sock_fd, Replies::RPL_WELCOME);
        return (client._authenticated = true);
    }
    return (false);
}
// can switch case instead
void    Server::m_reply(int clientFd, int replyCode) 
{
    if (replyCode == Replies::RPL_WELCOME)
    {
        this->m_send(clientFd, ":" + this->m_serverName + " 001 " + m_clients[clientFd]._nickname + " :Welcome bitch\r\n");
    }
    else if (replyCode == Replies::ERR_NICKNAMEINUSE)
    {
        this->m_send(clientFd, ":" + this->m_serverName + " 433 * " + m_clients[clientFd].messages.front().arguments.front() + " :Nickname is already in use bitch\r\n");
    }
}

void            Server::m_managePoll(void)
{
    unsigned long i = 0;

    // event happend on current pfds
    while (i < this->m_pfds.size() && this->m_poll_count)
    {
        if (this->m_pfds[i].revents & POLLIN)
        {
            // if server socket
            if (this->m_pfds[i].fd == this->m_sockfd)
            {
                if (this->m_manageServerEvent())
                    continue ;
            } // if client socket
            else
                this->m_manageClientEvent(i); 
            this->m_poll_count--;
        }
        i++;
    }
}
void    Server::m_debugAuthentificate(int clientFd)
{
    this->m_send(clientFd, ":" + this->m_serverName + " 001 ohachim :Welcome\r\n");
    this->m_clients[clientFd]._authenticated = true;
    this->m_clients[clientFd].messages.pop_front();
}

int             Server::startServer(void)
{
    while (1)
    {
        this->m_poll();
        this->m_managePoll();
    }
}

int Server::m_send(int toFd, std::string message)
{
    int bytesSent = 0;
    message += END_STRING;
    int size = message.size();

    while (size)
    {
        bytesSent = send(toFd, message.data() + bytesSent, size, 0);
        if (bytesSent == -1)
            return (-1);
        size -= bytesSent;
    }
    return (0);
}
// TODO: FOR DEBEGGUING WILL BE REMOVED
void    printQueue(std::deque<Message> q)
{
    std::deque<Message>::iterator ib = q.begin();
    std::deque<Message>::iterator ie = q.end();

    std::cout << "Printing queue\n";
    for (std::deque<Message>::iterator i = ib; i != ie; i++)
    {
        std::cout << "[";
        std::cout << i->message;
        std::cout << "]";
    }
    std::cout << '\n';
}

int	Server::m_manageRecv(std::string message, int clientFd)
{
    t_messageDQeue& messageQueue = this->m_clients[clientFd].messages;
    std::string token = strToken(message);

    std::cout << "Queue size == " << messageQueue.size() << std::endl;
    while (token.size())
    {
        std::cout << "this is a token: " << token << std::endl;
        if (!messageQueue.size() || messageQueue.back().message.find(END_STRING)
                                                        != std::string::npos)
            messageQueue.push_back(Message(token));
        else if (messageQueue.back().message.find(END_STRING) == std::string::npos)
            messageQueue.back().message += token;
        token = strToken("");
    }
    printQueue(messageQueue);
    if (messageQueue.back().message.find(END_STRING) != std::string::npos)
        return (1);
    return (0);
}

void    Server::m_eraseClientPolls(int clientFd)
{
    std::vector<t_pollfd>::iterator ib = this->m_pfds.begin();
    std::vector<t_pollfd>::iterator ie = this->m_pfds.end();

    for (std::vector<t_pollfd>::iterator i = ib; i != ie; i++)
    {
        if ((*i).fd == clientFd)
        {
            this->m_pfds.erase(i);
            break ;
        }
    }
};



// hash function for the nicknames since the std::hash function is a c++11 feature
// no need for now
// long long Server::m_hash(char *str)
// {
//     int prime = 31;
//     int mod = BIG_NUMBER;
// }

/********************************************************
 *                                                      *
 *                                                      *
 *         THIS SECTION IS MADE FOR THE COMMANDS        *
 *                                                      *
 * *****************************************************/

// not tested/ not working
// will be upgraded when we add channels
void                    Server::m_quit(int clientFd, std::string quitMessage)
{
    Client& client = this->m_clients[clientFd];

    std::string messageToSend = ":" + client._nickname + "!" + client.hostname
                                 + " " + quitMessage;

    this->m_send(clientFd, messageToSend);
    // should I cut the connection
    // stop listening to incoming events
    this->m_eraseClientPolls(clientFd);
    // removing it from client list
    this->m_clients.erase(clientFd);
    close(clientFd);
    // just to compile the thing
};