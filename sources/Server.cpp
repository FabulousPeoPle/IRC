/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ohachim <ohachim@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/11 16:40:51 by ohachim           #+#    #+#             */
/*   Updated: 2022/01/13 22:13:46 by ohachim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

// #define DEBUG
// #define DEBUG_USERHOST

char* strdup(const char *s);

Server::Server(void) : m_port(DEFAULT_PORT), m_hostname(DEFAULT_HOSTNAME)
{
    #ifdef DEBUG
    std::cout << "Default Server constructor called." << std::endl;
    #endif
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
    #ifdef DEBUG
    std::cout << "Default Server destructor called." << std::endl;
    #endif
}

Server::Server(std::string port, std::string hostname, std::string name) : m_serverName(name), m_port(port), m_hostname(hostname)
{
    #ifdef DEBUG
    std::cout << "Port/Hostname constructor called." << std::endl;
    #endif
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
    #ifdef DEBUG
    std::cout << "Copy constructor called." << std::endl;
    #endif
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
    #ifdef DEBUG
    std::cout << "something happening\n";
    #endif
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

// TODO: check RPL_UMODEIS in other severs


int     findMode(char c)
{
    switch (c)
    {
        case 'a':
            return (Modes::away);
        case 'i':
            return (Modes::invisible);
        case 'w':
            return (Modes::wallops);
        case 'r':
            return (Modes::restricted);
        case 'o':
            return (Modes::oper);
        case 'O':
            return (Modes::local_oper);
        case 's':
            return (Modes::server_notices);           
        default:
            return (-1);
    }
}
void            Server::m_modeCmd(Client& client)
{
    Message& message = client.messages.front();

    if (message.arguments.size() < 2)
        m_reply(client._sock_fd, Replies::ERR_NEEDMOREPARAMS, 0); // NOT SURE ABOUT THIS
    else if (message.arguments[0] != client._nickname)
        m_reply(client._sock_fd, Replies::ERR_USERSDONTMATCH, 0);
    
    for (int i = 1; i < message.arguments.size() - 1; ++i) // MIGHT check for enough params inside looop?
    {
        char prefix = message.arguments[i][0];
        for (int j = 1; j < message.arguments[i].size(); ++j)
        {
            int modeNum = findMode(message.arguments[i][j]);
            if (modeNum == Modes::away) // ignore it for now, don't know the exact behaviour
                continue ;
            if (modeNum == -1)
            {
                m_reply(client._sock_fd, Replies::ERR_UMODEUNKNOWNFLAG, 0);
                continue ; // Ignore it for now, but might be a warning
            }
            if (prefix == '+')
            {
                if (modeNum == Modes::oper || modeNum == Modes::local_oper)
                    continue ;
                client.turnOnMode(modeNum);
            }
            else if (prefix == '-')
            {
                if (modeNum == Modes::restricted)
                    continue ;
                client.turnOffMode(modeNum);
            }
        }
    }
    // should reply with a string of the modes
    m_reply(client._sock_fd, Replies::RPL_UMODEIS, 0);
}
void                Server::m_relay(int clientFd)
{
    Client& client = this->m_clients[clientFd];
    t_messageDQeue& messages = client.messages;
    
    while (!messages.empty())
    {
        Message& message = messages.front();

        message.parse();
        #ifdef DEBUG
        std::cout << message.command << " this is the command\n";
        #endif
        
        // This two lines might be deleting our message
        // if (!messages.empty())
        if (message.command == USERHOST_COMMAND)
            this->m_userhostCmd(m_clients[clientFd]);
        else if (message.command == USER_COMMAND)
            m_reply(clientFd, Replies::ERR_ALREADYREGISTRED, 0); // not sure about this
        else if (message.command == QUIT_COMMAND)
            this->m_quitCmd(clientFd, message._literalMsg);
        else if (message.command == MODE_COMMAND)
            this->m_modeCmd(m_clients[clientFd]);
        if (!messages.empty())
            messages.pop_front();
    }
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
            std::cout << "Client disconnected.\n";
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
                #ifdef DEBUG
                std::cout << "client not authenticated" << std::endl;
                #endif
                // this->m_debugAuthentificate(this->m_pfds[pollIndex].fd);
                this->m_tryAuthentificate(m_clients[this->m_pfds[pollIndex].fd]);
            }
        }
    }
}

bool    Server::m_checkNickSyntax(Message& message)
{
    if (message.arguments.empty())
    {
        #ifdef DEBUG_USERHOST
        std::cout << message.arguments.empty() << " there are no arguments u cunt!\n";
        #endif
        return false;
    }
    std::string & nick = message.arguments.front();
    int len = nick.length();
    
    // pending research on the restrictions demanded
    if (len > 9 || len < 1)
    {
        #ifdef DEBUG_USERHOST
        std::cout << "wrong length u cunt!\n";
        #endif
        return (false);
    }
    return (true);
}

bool    Server::m_checkStatusAuth(Client& client)
{
    if (client._userAuth && client._nickAuth)
    {
        #ifdef DEBUG
        std::cout << "Has authenticated correctly\n";
        #endif
        m_reply(client._sock_fd, Replies::RPL_WELCOME, 0);
        return (client._authenticated = true);
    }
    return (false);
}

bool    Server::m_tryAuthentificate(Client& client)
{
    #ifdef DEBUG
    std::cout << "Trying to authenticate\n";
    #endif
    while (!m_checkStatusAuth(client) && client.messages.size())
    {
        // should userCMD parse?
        Message& msg = client.messages.front();

        msg.parse();
        m_userCmd(client);
        m_nickCmd(client);
        if (!client.messages.empty())
            client.messages.pop_front();
    }
    return (this->m_isAuthenticated(client._sock_fd));
}
// heavy refactor

void    Server::m_reply(int clientFd, int replyCode, int extraArg) // TODO:: change this so that it can take a string argument, needed for MODE reply
{
    #ifdef DEBUG_USERHOST
    std::cout << m_clients[clientFd]._nickname << "|\n";
    std::cout << extraArg << "+++" << clientFd << std::endl;
    #endif
    
    switch (replyCode)
    {
        case Replies::RPL_WELCOME :\
            this->m_send(clientFd, ":" + this->m_serverName + " 001 " + m_clients[clientFd]._nickname + " :Welcome bitch\r\n");
            break;
        case Replies::RPL_USERHOST :\
            this->m_send(clientFd, ":" + this->m_serverName + " 302 " + m_clients[clientFd]._nickname + " :"\
            + m_clients[extraArg]._nickname + ((m_clients[extraArg]._isServerOp) ? "*" : "\0") + "=" \
            + ((m_clients[extraArg]._away) ? "+" : "-") + m_clients[extraArg].hostname + "\r\n");
            break;
        case Replies::ERR_NICKNAMEINUSE :\
            this->m_send(clientFd, ":" + this->m_serverName + " 433 * " + m_clients[clientFd].messages.front().arguments.front() + " :Nickname is already in use bitch\r\n");;
            break;
        case Replies::ERR_ERRONEUSNICKNAME :\
            this->m_send(clientFd, ":" + this->m_serverName + " 432 " + m_clients[clientFd]._nickname + " :Erroneous nickname\r\n");
            break;
        case Replies::ERR_NEEDMOREPARAMS :\
            this->m_send(clientFd, ":" + this->m_serverName + " 461 " + m_clients[clientFd].messages.front().command + " :Not enough parameters\r\n"); // needs the name of the command
            break;
        case Replies::ERR_ALREADYREGISTRED :\
            this->m_send(clientFd, ":" + this->m_serverName + " 462 :Unauthorized command (already registered)\r\n");
            break;
        case Replies::ERR_USERSDONTMATCH:
            this->m_send(clientFd, ":" + this->m_serverName + " 446 :Cannot change mode for other users\r\n");
            break;
        case Replies::ERR_UMODEUNKNOWNFLAG:\
            this->m_send(clientFd, ":" + this->m_serverName + " 501 :Unknown MODE flag\r\n");
            break;
        case Replies:: RPL_UMODEIS:\
            this->m_send(clientFd, ":" + this->m_serverName + " 221 " + m_clients[clientFd]._nickname + " :+i\r\n"); // TODO: change +i to the correct value of modes
            break;
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
    if (this->m_clients[clientFd].messages.empty())
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
        // TODO: What the last parameter?
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

    #ifdef DEBUG
    std::cout << "Queue size == " << messageQueue.size() << std::endl;
    #endif
    while (token.size())
    {
        #ifdef DEBUG
        std::cout << "this is a token: " << token << std::endl;
        #endif
        if (messageQueue.empty() || messageQueue.back().message.find(END_STRING)
                                                        != std::string::npos)
            messageQueue.push_back(Message(token));
        else if (messageQueue.back().message.find(END_STRING) == std::string::npos)
            messageQueue.back().message += token;
        token = strToken("");
    }
    #ifdef DEBUG
    std::cout << "Queue size 2 == " << messageQueue.size() << std::endl;
    printQueue(messageQueue);
    #endif
    if (messageQueue.back().message.find(END_STRING) != std::string::npos)
        return (1);
    return (0);
}

void    Server::m_eraseClientPoll(int clientFd)
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

/********************************************************
 *                                                      *
 *                                                      *
 *         THIS SECTION IS MADE FOR THE COMMANDS        *
 *                                                      *
 * *****************************************************/

// not tested/ not working
// will be upgraded when we add channels

void    Server::m_userhostCmd(Client & client)
{
    // no need to check the size of the queue cz it must has atleast one message at this point
    Message & msg = client.messages.front();
    msg.parse();
    std::vector<std::string>::iterator it = msg.arguments.begin();
    std::vector<std::string>::iterator end = msg.arguments.end();
    int count = 0;

    while (!msg.arguments.empty() && m_checkNickSyntax(msg) && it < end && count < 5)
    {
        // call the userhost reply
        if (m_nicknames.find(*it) != m_nicknames.end())
            m_reply(client._sock_fd, Replies::RPL_USERHOST, m_nicknames[*it]);
        it++;
        count += 1;
    }
}
// not yet functional as we have to talk about the reply function
void    Server::m_isonCmd(Client & client)
{
    // no need to check the size of the queue cz it must has atleast one message at this point
    Message & msg = client.messages.front();
    msg.parse();
    std::vector<std::string>::iterator it = msg.arguments.begin();
    std::vector<std::string>::iterator end = msg.arguments.end();

    while (!msg.arguments.empty() && m_checkNickSyntax(msg) && it < end)
    {
        // call the ison reply
        if (m_nicknames.find(*it) != m_nicknames.end())
            m_reply(client._sock_fd, Replies::RPL_USERHOST, m_nicknames[*it]);
        it++;
    }
}

void    Server::m_nickCmd(Client & client)
{
    Message& msg = client.messages.front();
    
    if (msg.command == NICK_COMMAND && !client._nickAuth)
    {
        #ifdef DEBUG
        std::cout << "We got the nick command\n";
        #endif
        if (!m_checkNickSyntax(msg))
            return ;
        // ask about this
        std::pair<std::map<std::string, int>::iterator , bool> nick =\
        m_nicknames.insert(std::make_pair(msg.arguments.front(), client._sock_fd));
        if (nick.second)
        {
            // TODO: find a better fix, segfault in this line if there are no arguments
            // These two lines cause a segfault
            // if (!client.messages.empty())
            //     client.messages.pop_front();
            if (msg.arguments.size() >= 1)
            {
                client._nickname = msg.arguments.front();
                client._nickAuth = true;
            }
            else
                client._nickAuth = false;
        }
        else
            m_reply(client._sock_fd, Replies::ERR_NICKNAMEINUSE, 0);
    }
}

void    Server::m_userCmd(Client & client)
{
    Message& msg = client.messages.front();
    std::string mode;
    
    if (msg.command == USER_COMMAND)
    {
        #ifdef DEBUG
        std::cout << "we got the user command\n";
        #endif
        if (client._userAuth)
            m_reply(client._sock_fd, Replies::ERR_ALREADYREGISTRED, 0);
        else if (msg.arguments.size() != 3)
            m_reply(client._sock_fd, Replies::ERR_NEEDMOREPARAMS, 0);
        else
        {
            client._username = msg.arguments[0];
            mode = msg.arguments[1];
            client._realname = msg._literalMsg;
            std::cout << "We got the user command\n";
            client._userAuth = true;
            if (!client.messages.empty())
                client.messages.pop_front();
        }
    }
}




// TO WHO SHOULD I SEND THE REPLAY, maybe if the user belongs to no channel, I do nothing!
// upgrade with channels
// parameters should be the client& instead of both the client and the message
void                    Server::m_quitCmd(int clientFd, std::string quitMessage)
{
    Client& client = this->m_clients[clientFd];

    std::string messageToSend = ":" + client._nickname + "!" + client.hostname
                                 + " " + quitMessage + "\r\n";

    this->m_send(clientFd, messageToSend);
    // should I cut the connection
    // stop listening to incoming events
    this->m_eraseClientPoll(clientFd);
    // remove the nickname from the map
    this->m_nicknames.erase(m_clients[clientFd]._nickname);
    // removing it from client list
    this->m_clients.erase(clientFd);
    close(clientFd);
};

