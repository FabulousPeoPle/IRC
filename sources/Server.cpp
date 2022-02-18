/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ohachim <ohachim@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/11 16:40:51 by ohachim           #+#    #+#             */
/*   Updated: 2022/02/18 20:12:11 by ohachim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */




#include "Server.hpp"

// #define DEBUG
// #define DEBUG_USERHOST

// TODO: NICK NEEDS TO CHECK FOR NUMBER OF PARAMETERS
// TODO: LITERAL MESSAGE DOESN'T NECESSARILY CONTAIN ':'
// TODO: turn on channel creator when creating channel? what did we do before

char* strdup(const char *s);

Server::Server(void) : m_port(DEFAULT_PORT), m_hostname(DEFAULT_HOSTNAME), m_version("0.1"), m_maxClients(150), m_athenticatedUserNum(0)
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

Server::Server(std::string port, std::string hostname, std::string name, int maxClients) : m_serverName(name), m_port(port), m_hostname(hostname), m_version("0.1"), m_maxClients(maxClients), m_athenticatedUserNum(0)
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

Server::Server(const Server& serverRef) : m_maxClients(serverRef.m_maxClients) // ach had s7our
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

void            Server::setOperPassword(std::string password)
{
    m_operPassword = password;
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

std::string            Server::convertToHostname(t_sockaddr_storage& remoteAddr, int sock_fd)
{
    char buffer[INET6_ADDRSTRLEN];
    int err=getnameinfo((struct sockaddr*)&remoteAddr, sizeof(t_sockaddr_storage), buffer,sizeof(buffer), 0, 0, 0);
    if (err)
    {
        printf("Failed to get hostname!\nExiting...");
        exit(1);
    }
    std::string s(buffer);
    return (s);
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
    if (this->m_clients.size() < this->m_maxClients)
    {
        this->m_clients[newFd] = Client(newFd, remoteAddr, addrlen);
        this->m_clients[newFd].setHostname(this->convertToHostname(remoteAddr, newFd));
        this->m_pfds.push_back((t_pollfd){newFd, POLLIN, 0});
    }
    else
    {
        m_reply(newFd, Replies::RPL_BOUNCE, 0, "");
        close(newFd);
        return (-2);
    }
    return (0);
}

bool            Server::m_isAuthenticated(int clientFd)
{
    return (this->m_clients[clientFd].isAuthComplete());
}

// TODO: check RPL_UMODEIS in other severs


int     findMode(char c)
{
    switch (c)
    {
        case 'a':
            return (UserModes::away);
        case 'i':
            return (UserModes::invisible);
        case 'w':
            return (UserModes::wallops);
        case 'r':
            return (UserModes::restricted);
        case 'o':
            return (UserModes::oper);
        case 's':
            return (UserModes::serverNotices);           
        default:
            return (-1);
    }
}

bool            Server::m_isChannelPrefix(char c) const
{
    return (c == '#' || c == '&');
}

std::vector<std::string>     Server::m_extractTLDs(std::vector<std::string>& arguments, int start)
{
    std::vector<std::string>::iterator itb = arguments.begin() + start; // 2 is the start of the arguments that contain masks
    std::vector<std::string>::iterator ite = arguments.end();

    std::vector<std::string>            TLDs;
    for (std::vector<std::string>::iterator it = itb; it != ite; ++it)
    {
        if (it->substr(0, 5) != "*!*@*") // Means there are no more masks
            break;
        std::string TLD = it->erase(0, 5);
        if (TLD[0] == '.')
            TLDs.push_back(TLD);
    }
    return (TLDs);
}

bool            Server::m_isMaskUserMatch(std::string nickname, std::string TLD) // DON'T EXTRACT TLD HERE
{
    nickname = nickname.erase(0, nickname.size() - TLD.size());

    return (nickname == TLD || TLD.empty());
}

std::string     Server::m_composeChannelModes(std::string channelName)
{
    Channel& channel = m_channels[channelName];

    std::string channelModes = "+";

    for (int i = 0; i < ChannelModes::total; ++i)
    {
        if (channel.getModeValue(i))
            channelModes += Channel::potentialModes[i];
    }
    return (channel.getName() + ' ' + channelModes);
}

/* take mode
    is mode userMode?
    is mode toggler?
    is mode simple setter/remover
    is mode complex setter/remover
*/

bool            Server::m_isUserSpecificChannelMode(char c) const
{
    return (c == 'O' || c == 'o' || c == 'v');
}

bool            Server::m_isClientOper(Client& client, std::string channelName) const
{
    return (client.getModeValue(ChannelModes::o_OperatorPrivilege, channelName)
            || client.getModeValue(ChannelModes::O_Creator, channelName));
}

bool            Server::m_isAttributeSetterMode(char c) const
{
    return (c == 'k' || c == 'l');
}

bool            Server::m_isUserSpecificChannelMode(char c) const
{
    return (c == 'o' || c == 'O' || c == 'v');
}

bool            Server::m_isSimpleChannelMode(char c) const
{
    switch (c)
    {
        case 'a':
        case 'i':
        case 'm':
        case 'n':
        case 'q':
        case 'p':
        case 's':
        case 'r':
        case 't':
            return (true);
        default:
            return (false);
    }
}

bool            Server::m_isMaskMode(char c) const
{
    return (c == 'b' || c == 'e' || c == 'I');
}

void            Server::m_manageChannelModes(char mode, char prefix, std::vector<std::string> arguments, int paramToUseIndex)
{
    if (arguments.size() < paramToUseIndex) // remember to reply not enough arguments
    {
        Client& client = m_clients[m_nicknames[arguments[paramToUseIndex]]];

        if (prefix == '+')
            client.turnOnMode(findMode(mode));
        else if (prefix == '-')
            client.turnOffMode(findMode(mode));
    }
}

void            Server::m_executeModes(std::vector<std::string> arguments, Channel& channel, Client& client)
{
    std::string modes = arguments[2];
    char prefix;
    int     paramToUseIndex = 3;
    if (modes[0] == '+' || modes[0] == '-')
        prefix = modes[0];
    for (int i = 1; i < modes.size() - 1; ++i)
    {
        if (m_isAttributeSetterMode(modes[i]))
            channel.manageAttribute(modes[i], prefix, arguments, paramToUseIndex);
        else if (m_isSimpleChannelMode(modes[i]))
            channel.manageSimpleMode(modes[i], prefix);
        else if (m_isUserSpecificChannelMode(modes[i]))
            m_manageChannelModes(modes[i], prefix, arguments, paramToUseIndex);
        else if (m_isMaskMode(modes[i]))
            // extract mask TLD for later use
        else
            m_reply(client.getFd(), Replies::ERR_UMODEUNKNOWNFLAG, 0, "");

    }
}

void            Server::m_channelModeCmd(Client& client, Message& message)
{
    // TODO: a function that takes a mask and sees if a client mask looks like it
    std::vector<std::string> arguments = message.getArgs();

    if (!m_channelExists(arguments[1]))
    {
        m_reply(client.getFd(), Replies::ERR_NOSUCHCHANNEL, 0, arguments[1]);
        return ;
    }
    if (arguments.size() == 1)
    {
        m_reply(client.getFd(), Replies::RPL_CHANNELMODEIS, 0, m_composeChannelModes(arguments[1]));
        return ;
    }  // WHAT IF THE CLIENT IS NOT THE CHANNEL OPERATOR
    if (!m_isClientOper(client, arguments[1]))
    {
        m_reply(client.getFd(), Replies::ERR_CHANOPRIVSNEEDED, 0, arguments[1]); // maybe change arguments[1] to channelName
        return ;
    }
    if (arguments.size() >= 2)// MODE CHAN_NAME MODES WHO
    {
        m_executeModes(arguments, m_channels[arguments[1]], client);
    }
    
    
}

void            Server::m_userModeCmd(Client& client, Message& message) // TODO: REFACTOR
{
    if (message.getArgs()[0] != client.getNickname())
    {
        m_reply(client.getFd(), Replies::ERR_USERSDONTMATCH, 0, "");
        return ;
    }

    std::string modeChanges;
    char prefix = message.getArgs()[1][0];

    modeChanges += prefix;
    for (int j = 1; j < message.getArgs()[1].size(); ++j)
    {
        int modeNum = findMode(message.getArgs()[1][j]);

        if (modeNum == UserModes::away) // Ignore it for now, don't know the exact behaviour
            continue ;
        if (modeNum == -1)
        {
            m_reply(client.getFd(), Replies::ERR_UMODEUNKNOWNFLAG, 0, "");
            continue ; // Ignore it for now, but might be a warning
        }
        if (prefix == '+')
        {
            if (modeNum == UserModes::oper)
                continue ;
            if (!client.getModeValue(modeNum))
            {
                client.turnOnMode(modeNum);
                modeChanges += message.getArgs()[1][j];
            }
        }
        else if (prefix == '-')
        {
            if (modeNum == UserModes::restricted)
                continue ;
            if (client.getModeValue(modeNum))
            {
                client.turnOffMode(modeNum);
                modeChanges += message.getArgs()[1][j];
            }
        }
    }
    // should reply with a string of the modes
    if (modeChanges.size() > 1) // that's how GeekShed do it
        m_reply(client.getFd(), Replies::RPL_UMODEIS, 0, modeChanges);
}

void            Server::m_modeCmd(Client& client) // TODO: CHANGE THE MAGIC NUMBERS
{
    Message& message = client.getMessageQueue().front();

    if (message.getArgs().size() < 2)
    {
        m_reply(client.getFd(), Replies::ERR_NEEDMOREPARAMS, 0, ""); // NOT SURE ABOUT THIS
        return ;
    }
    if (m_isChannelPrefix(message.getArgs()[0][0]))
        m_channelModeCmd(client, message);
    else
        m_userModeCmd(client, message);
}

std::string         Server::m_composeMotd(std::ifstream& motdFile, std::string clientNick)
{
    std::string     line;
    std::string     motd;
    char            c;
    int             count;
    int             cursor;

    while (std::getline(motdFile, line))
    {
        if (!motd.empty())
            motd += ":" + this->m_serverName + " 372 " + clientNick + " :-\t  " + line;
        motd += '\n';
    }
    count = 0;
    cursor = 0;
    while (cursor < motd.size()) // NEEDS TO BE CHANGED; UGLLYYYYYYYY
    {
        if (count == MOTD_LENGTH_LINE + 1) 
        {
            if (motd[cursor] != '\n')
                motd.insert(cursor, "\n");
            count = 0;
        }
        cursor += 1;
        count += 1;
        if (motd[cursor] == '\n')
            count = 0;
    }
    return (motd);
}


// TODO: Would be nice if we add the servername and command code in every line
void                Server::m_motdCmd(Client& client) // We will assume that there is no target, considering it's server to server communication
{
    std::ifstream motd("motd.txt");

    if (motd.fail())
    {
        m_reply(client.getFd(), Replies::ERR_NOMOTD, 0, "");
        return ;
    }
    m_reply(client.getFd(), Replies::RPL_MOTDSTART, 0, "");
    m_reply(client.getFd(), Replies::RPL_MOTD, 0, this->m_composeMotd(motd, client.getNickname()));
    m_reply(client.getFd(), Replies::RPL_ENDOFMOTD, 0, "");
}

void                Server::m_awayCmd(Client& client) // SHOULD WORK TOGETHER WITH PRIVMSG
{
    Message& message = client.getMessageQueue().front();

    if (message.getLiteralMsg().size())
    {
        client.setAwayMsg(message.getLiteralMsg());
        client.turnOnMode(UserModes::away);
        m_reply(client.getFd(), Replies::RPL_NOWAWAY, 0, "");
    }
    else
    {
        client.setAwayMsg("");
        client.turnOffMode(UserModes::away);
        m_reply(client.getFd(), Replies::RPL_UNAWAY, 0, ""); // what if the user was never away
    }
}

void                Server::m_lusersCmd(Client& client)
{
    m_reply(client.getFd(), Replies::RPL_LUSERCLIENT, 0, "");
    m_reply(client.getFd(), Replies::RPL_LUSEROP, 0, "");
    m_reply(client.getFd(), Replies::RPL_LUSERUNKNOWN, 0, "");
    m_reply(client.getFd(), Replies::RPL_LUSERCHANNELS, 0, "");
    m_reply(client.getFd(), Replies::RPL_LUSERME, 0, "");
    
}
// TODO: WE SHOULD BE ABLE TO USE NICK AGAIN
std::string                 Server::m_makeReplyHeader(int replyNum, std::string nickname)
{
    std::string strReplyNum = std::to_string(replyNum);
    int         len = strReplyNum.size();
    std::string s = std::string(3 - len, '0');
    std::string header = ":" + this->m_serverName + " " + s + strReplyNum + ' ' + nickname;
    return (header);
}

std::string                Server::m_composeWhoisQuery(Client& queryClient, std::string clientNickname, int replyCode)
{
    switch (replyCode)
    {
        case Replies::RPL_WHOISUSER:
            return (m_makeReplyHeader(Replies::RPL_WHOISUSER, clientNickname) + ' ' + queryClient.getNickname()
                                        + " ~" + queryClient.getUsername() + ' ' + queryClient.getHostname() + " * " + queryClient.getRealname());
        case Replies::RPL_WHOISSERVER:
            return (m_makeReplyHeader(Replies::RPL_WHOISSERVER, clientNickname) + ' ' + queryClient.getNickname()
                                                    + ' ' + m_serverName + ": Morocco 1337 Server, Provided by 1337 students Powered by Itisalat Al Maghreb.");
        case Replies::RPL_ENDOFWHOIS:
            return (m_makeReplyHeader(Replies::RPL_ENDOFWHOIS, clientNickname) + ' ' + queryClient.getNickname()
                                         + ' ' + ":End of WHOIS list.");
        default:
            return ("");
    }
}

void                Server::m_pongCmd(Client& client)
{
    (void)client;
    std::cout << "Somebody Pinged\n";
}

void                Server::m_pingCmd(Client& client)
{
    m_reply(client.getFd(), Replies::RPL_PINGREQUEST, 0, "");
}

void                Server::m_whoisCmd(Client& client)
{
    Message&    message = client.getMessageQueue().front();
    if (message.getArgs().empty())
    {
        m_reply(client.getFd(), Replies::ERR_NEEDMOREPARAMS, 0, ""); // MIGHT remove
        return ;
    }
    std::string queryClientName = message.getArgs()[0];
    int clientFd;
    if (this->m_nicknames.find(queryClientName) == this->m_nicknames.end())
    {
        m_reply(client.getFd(), Replies::ERR_NOSUCHNICK, 0, queryClientName);
        return ;
    }
    else
        clientFd = this->m_nicknames[queryClientName];
    Client&     queryClient = this->m_clients[clientFd];

    m_reply(client.getFd(), Replies::RPL_WHOISUSER, 0, m_composeWhoisQuery(queryClient, client.getNickname(), Replies::RPL_WHOISUSER));
    m_reply(client.getFd(), Replies::RPL_WHOISSERVER, 0, m_composeWhoisQuery(queryClient, client.getNickname(), Replies::RPL_WHOISSERVER));
    m_reply(client.getFd(), Replies::RPL_ENDOFWHOIS, 0, m_composeWhoisQuery(queryClient, client.getNickname(), Replies::RPL_ENDOFWHOIS));
}

std::string         Server::m_composeRplTopic(Channel& channel)
{
    return (channel.getName() + ':' + channel.getTopic());
}

void                Server::m_topicCmd(Client& client)
{
    Message& message = client.getMessageQueue().front();

    if (message.getArgs().empty())
    {
        m_reply(client.getFd(), Replies::ERR_NEEDMOREPARAMS, 0, "");
        return ;
    }
    std::vector<std::string> &channels = client.getChannels();
    if (std::find(channels.begin(), channels.end(), message.getArgs()[0]) == channels.end())
    {
        m_reply(client.getFd(), Replies::ERR_NOTONCHANNEL, 0, message.getArgs()[0]);
        return ;
    }

    if (message.getArgs().size() == 1) // there is only one argument, channel name
    {
        if (m_channels[message.getArgs()[0]].getTopic().empty()) // checking if topic is empty
        {
            m_reply(client.getFd(), Replies::RPL_NOTOPIC, 0, message.getArgs()[0]);
            return;
        }
        m_reply(client.getFd(), Replies::RPL_TOPIC, 0,
            m_composeRplTopic(m_channels[message.getArgs()[0]])); // there is a topic
        return;
    }

   // TODO: STILL NEED TO IMPLEMENT TOPIC CHANGING STUFF. 
    
}

void                Server::m_operCmd(Client& client)
{
    Message& message = client.getMessageQueue().front();

    std::vector<std::string>    arguments = message.getArgs();
    if (arguments.size() < 2)
    {
        m_reply(client.getFd(), Replies::ERR_NEEDMOREPARAMS, 0, "");
        return ;
    }
    if (arguments[0] != client.getNickname())
    {
        m_reply(client.getFd(), Replies::ERR_NOOPERHOST, 0, "");
        return ;
    }
    if (arguments[1] != m_operPassword)
    {
        m_reply(client.getFd(), Replies::ERR_PASSWDMISMATCH, 0, "");
        return ;
    }
    client.turnOnMode(UserModes::oper);
    m_reply(client.getFd(), Replies::RPL_YOUREOPER, 0, "");    
}


void                Server::m_relay(int clientFd)
{
    Client& client = this->m_clients[clientFd];
    t_messageDQeue& messages = client.getMessageQueue();
    
    while (!messages.empty())
    {
        Message& message = messages.front();

        message.parse();
        #ifdef DEBUG
        std::cout << message.getCmd()<< " this is the command\n";
        #endif
        
        // This two lines might be deleting our message
        // if (!messages.empty())
        // TODO: replace with switch case
        // TODO: a map that has command function as values and the string command as key
        std::string command = message.getCmd();

        if (command == USERHOST_COMMAND)
            this->m_userhostCmd(m_clients[clientFd]);
        else if (command == USER_COMMAND)
            m_reply(clientFd, Replies::ERR_ALREADYREGISTRED, 0, ""); // not sure about this
        else if (command == QUIT_COMMAND)
            this->m_quitCmd(clientFd, message.getLiteralMsg());
        else if (command == MODE_COMMAND)
            this->m_modeCmd(m_clients[clientFd]);
        else if (command == PING_COMMAND)
            this->m_pingCmd(m_clients[clientFd]);
        else if (command == PONG_COMMAND)
            this->m_pongCmd(m_clients[clientFd]); // doesn't do anything for now
        else if (command == MOTD_COMMAND)
            this->m_motdCmd(m_clients[clientFd]);
        else if (command == AWAY_COMMAND)
            this->m_awayCmd(m_clients[clientFd]);
        else if (command == LUSERS_COMMAND)
            this->m_lusersCmd(m_clients[clientFd]);
        else if (command == WHOIS_COMMAND)
            this->m_whoisCmd(m_clients[clientFd]);
        else if (command == TOPIC_COMMAND)
            this->m_topicCmd(m_clients[clientFd]);
        else if (command == PRIVMSG_COMMAND || command == NOTICE_COMMAND)
            this->m_privMsgCmd_noticeCmd(m_clients[clientFd], (command == PRIVMSG_COMMAND) ? true : false);
        else if (command == OPER_COMMAND)
            this->m_operCmd(m_clients[clientFd]);
        else if (command.size())
            m_reply(clientFd, Replies::ERR_UNKNOWNCOMMAND, 0, command);
        if (!messages.empty())
            messages.pop_front();
    }
}

// void                Server::m_setCommandFuncs(void)
// {
//     this->m_commands["QUIT"] = &(this->m_quit);

// }

void                Server::m_manageClientEvent(int pollIndex)
{
    // I have to error management message size
    char    buffer[BUFFER_SIZE];
    int     bytesRead = recv(this->m_pfds[pollIndex].fd, buffer, BUFFER_SIZE - 1, 0);

    if (bytesRead <= 0)
    {
        if (bytesRead == 0)
            std::cout << "Client disconnected.\n";
        else
            perror("recv: ");
        this->m_nicknames.erase(m_clients[this->m_pfds[pollIndex].fd].getNickname());
        this->m_pfds.erase(this->m_pfds.begin() + pollIndex);
        m_clients.erase(this->m_pfds[pollIndex].fd);
        close(this->m_pfds[pollIndex].fd);
    }
    else
    {
        buffer[bytesRead] = '\0';
        std::cout << "this is the buffer: " << buffer << std::endl;
        // send(this->m_pfds[pollIndex].fd, ":555 001 ohachim :welcome\r\n", 86, 0);
        if (this->m_manageRecv(buffer, this->m_pfds[pollIndex].fd))
        {
            if (this->m_isAuthenticated(this->m_pfds[pollIndex].fd))
                this->m_relay(this->m_pfds[pollIndex].fd);
            else
            {
                #ifdef DEBUG
                std::cout << "client not authenticated" << std::endl;
                #endif
                this->m_tryAuthentificate(m_clients[this->m_pfds[pollIndex].fd]);
            }
        }
    }
}

bool    Server::m_checkNickSyntax(Message& message)
{
    if (message.getArgs().empty())
    {
        #ifdef DEBUG_USERHOST
        std::cout << message.getArgs().empty() << " there are no arguments u cunt!\n";
        #endif
        return false;
    }
    std::string & nick = message.getArgs().front();
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
    if (client.isUserAuth() && client.isNickAuth())
    {
        #ifdef DEBUG
        std::cout << "Has authenticated correctly\n";
        #endif
        m_reply(client.getFd(), Replies::RPL_WELCOME, 0, "");
        m_reply(client.getFd(), Replies::RPL_YOURHOST, 0, "");
        m_reply(client.getFd(), Replies::RPL_CREATED, 0, "");
        m_reply(client.getFd(), Replies::RPL_MYINFO, 0, "");
        this->m_motdCmd(client);
        ++(this->m_athenticatedUserNum);
        client.setAuthComplete();
        return (true);
    }
    return (false);
}

bool    Server::m_isValidCommand(std::string potentialCommand)
{
    for (int i = 0; i < NUM_COMMANDS; ++i)
    {
        if (potentialCommand == Server::m_possibleCommands[i])
            return (true);
    }
    return (false);
}

bool    Server::m_tryAuthentificate(Client& client)
{
    #ifdef DEBUG
    std::cout << "Trying to authenticate\n";
    #endif
    while (!m_checkStatusAuth(client) && client.getMessageQueue().size())
    {
        // should userCMD parse?
        Message& msg = client.getMessageQueue().front();

        msg.parse();
        if (msg.getCmd() != NICK_COMMAND && msg.getCmd() != USER_COMMAND)
        {
            if (this->m_isValidCommand(msg.getCmd())) // TODO: ANAS check if command is an acceptable command
                m_reply(client.getFd(), Replies::ERR_NOTREGISTERED, 0, "");
            if (!client.getMessageQueue().empty())
                client.getMessageQueue().pop_front();
            return (false);
        }
        m_userCmd(client);
        m_nickCmd(client);
        if (!client.getMessageQueue().empty())
            client.getMessageQueue().pop_front();
    }
    return (this->m_isAuthenticated(client.getFd()));
}
// heavy refactor

void    Server::m_reply(int clientFd, int replyCode, int extraArg, std::string message) // TODO:: change this so that it can take a string argument, needed for MODE reply
{
    #ifdef DEBUG_USERHOST
    std::cout << m_clients[clientFd].getNickname() << "|\n";
    std::cout << extraArg << "+++" << clientFd << std::endl;
    #endif
    
    switch (replyCode)  // TODO: care for message syntax
    {
        case Replies::RPL_WELCOME :\
            this->m_send(clientFd, ":" + this->m_serverName + " 001 " + m_clients[clientFd].getNickname() + " :Welcome sunshine\r\n");
            break;
        case Replies::RPL_YOURHOST:  // TODO: care for message syntax
            this->m_send(clientFd, ":" + this->m_serverName + " 002 " + m_clients[clientFd].getNickname() + " :Your host is " + this->m_serverName + ", running version " + this->m_version + "\r\n");
            break;
        case Replies::RPL_CREATED:
            this->m_send(clientFd, ":" + this->m_serverName + " 003 " + m_clients[clientFd].getNickname() + " :This server was created Sat Feb 12 2020 at 10:40:00 GMT\r\n");
            break;
        case Replies::RPL_MYINFO: // TODO: decide on the possible modes for channels and users
            this->m_send(clientFd, ":" + this->m_serverName + " 004 " + m_clients[clientFd].getNickname() + " :" + this->m_serverName + " " + this->m_version + " " + "ao" + " " + "mtov\r\n");
            break;
        case Replies::RPL_BOUNCE:
            this->m_send(clientFd, ":" + this->m_serverName + " 005 " + m_clients[clientFd].getNickname() + " :Try server 'DS9.GeekShed.net', port '6667'\r\n");
            break;
        case Replies::RPL_USERHOST : // causes BitchX segmentation fault
            this->m_send(clientFd, ":" + this->m_serverName + " 302 " + m_clients[clientFd].getNickname() + " :"\
            + m_clients[extraArg].getNickname() + ((m_clients[extraArg].isServerOp()) ? "*" : "\0") + "=" \
            + ((m_clients[extraArg].isAway()) ? "+" : "-") + m_clients[extraArg].getHostname() + "\r\n");
            break;
        case Replies::ERR_NICKNAMEINUSE :\
            this->m_send(clientFd, ":" + this->m_serverName + " 433 * " + m_clients[clientFd].getMessageQueue().front().getArgs().front() + " :Nickname is already in use hun\r\n");;
            break;
        case Replies::ERR_ERRONEUSNICKNAME :\
            this->m_send(clientFd, ":" + this->m_serverName + " 432 " + m_clients[clientFd].getNickname() + " :Erroneous nickname\r\n");
            break;
        case Replies::ERR_NEEDMOREPARAMS :\
            this->m_send(clientFd, ":" + this->m_serverName + " 461 " + m_clients[clientFd].getMessageQueue().front().getCmd()+ " :Not enough parameters\r\n"); // needs the name of the command
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
        case Replies::RPL_UMODEIS: // TODO: EXTRA CARE
            this->m_send(clientFd, ":" + m_clients[clientFd].getNickname() + " MODE " + m_clients[clientFd].getNickname() + " :" + message + "\r\n"); // TODO: change +i to the correct value of modes
            break;
        case Replies::ERR_NOMOTD :\
            this->m_send(clientFd, ":" + this->m_serverName + " 422 :MOTD File is missing\r\n");
            break;
        case Replies::RPL_MOTDSTART :\
            this->m_send(clientFd, ":" + this->m_serverName + " 375 " + m_clients[clientFd].getNickname() +  " :- " + this->m_serverName + " Message of the day -\r\n");
            break;
        case Replies::RPL_ENDOFMOTD :\
            this->m_send(clientFd, ":" + this->m_serverName + " 376 " + m_clients[clientFd].getNickname() + " :End of MOTD command\r\n");
            break;
        case Replies::RPL_MOTD: // TODO: DECIDE IF WE WANNA PRINT DATE AFTER EVERY LINE
            this->m_send(clientFd, ":" + this->m_serverName + " 372 " + m_clients[clientFd].getNickname() + " :- " + message + "\r\n");
            break;
        case Replies::RPL_NOWAWAY: // TODO: care for message syntax
            this->m_send(clientFd, ":" + this->m_serverName + " 306 :You have been marked as being away\r\n");
            break;
        case Replies::RPL_UNAWAY:  // TODO: care for message syntax
            this->m_send(clientFd, ":" + this->m_serverName + " 305 :You are no longer marked as being away\r\n");
            break;
        case Replies::ERR_NOTREGISTERED:  // TODO: care for message syntax
            this->m_send(clientFd, ":" + this->m_serverName + " 451 :You have not registered\r\n");
            break;
        case Replies::ERR_UNKNOWNCOMMAND:
            this->m_send(clientFd, ":" + this->m_serverName + " 421 :" + message + " :Unknown command\r\n");
            break;
        case Replies::RPL_LUSERCLIENT:
            this->m_send(clientFd, ":" + this->m_serverName + " 251 " + m_clients[clientFd].getNickname() + " :" + std::to_string(this->m_athenticatedUserNum) + " user(s) on the server\r\n"); // TODO: STILL need to add number of services and the number of servers
            break;
        case Replies::RPL_LUSEROP:
            this->m_send(clientFd, ":" + this->m_serverName + " 252 " + m_clients[clientFd].getNickname() + " :" + std::to_string(this->m_calculateOperators()) + " operator(s) online\r\n");
            break;
        case Replies::RPL_LUSERUNKNOWN:
            this->m_send(clientFd, ":" + this->m_serverName + " 253 " + m_clients[clientFd].getNickname() + " :" + std::to_string(this->m_calculateUnknownConnections()) + " unknown connection(s)\r\n");
            break;
        case Replies::RPL_LUSERCHANNELS:
            this->m_send(clientFd, ":" + this->m_serverName + " 254 " + m_clients[clientFd].getNickname() + " :" + std::to_string(this->m_channels.size()) + " channels(s)\r\n");
            break;
        case Replies::RPL_LUSERME:
            this->m_send(clientFd, ":" + this->m_serverName + " 255 " + m_clients[clientFd].getNickname() + " :" + std::to_string(this->m_calculateKnownConnections()) + " client(s)\r\n");
            break;
        case Replies::RPL_PINGREQUEST:
            this->m_send(clientFd,":" + this->m_serverName + " PONG " + this->m_serverName + " :" + m_clients[clientFd].getNickname() + "\r\n");
            break;
        // the next too need the channel name (to be concidered when refactoring)
        case Replies::ERR_BANNEDFROMCHAN :\
            this->m_send(clientFd, ":" + this->m_serverName + " 474 <channel name> :Cannot join channel (+b)\r\n");
            break;
        case Replies::ERR_BADCHANNELKEY :\
            this->m_send(clientFd, ":" + this->m_serverName + " 475 <channel name> :Cannot join channel (+k)\r\n");
            break;
             // should change after refactoring
        case Replies::ERR_NOSUCHCHANNEL :\
            this->m_send(clientFd, ":" + this->m_serverName + " 403 " + message + " :No such channel\r\n");
            break;
        case Replies::ERR_NORECIPIENT :\
            this->m_send(clientFd, ":" + this->m_serverName + " 411  :No recipient given " + m_clients[clientFd].getMessageQueue().front().getCmd()+ "\r\n");
            break;
        case Replies::ERR_NOTEXTTOSEND :\
            this->m_send(clientFd, ":" + this->m_serverName + " 412 :No text to send\r\n");
            break;
        case Replies::ERR_NOSUCHNICK :\
            this->m_send(clientFd, ":" + this->m_serverName + " 401 " + m_clients[clientFd].getMessageQueue().front().getArgs().front() + " :No such nick/channel\r\n");
            break;
            // needs to change
        case Replies::ERR_TOOMANYTARGETS :\
            this->m_send(clientFd, ":" + this->m_serverName + " 407 " + m_clients[clientFd].getMessageQueue().front().getArgs().front() + " :too many recipients.\r\n");
            break;
        case Replies::RPL_WHOISUSER:
            this->m_send(clientFd, message + "\r\n");
            break;
        case Replies::RPL_WHOISSERVER:
            this->m_send(clientFd, message + "\r\n");
            break;
        case Replies::RPL_ENDOFWHOIS:
            this->m_send(clientFd, message + "\r\n");
            break;
        case Replies::ERR_NOTONCHANNEL:
            this->m_send(clientFd, m_makeReplyHeader(Replies::ERR_NOTONCHANNEL, this->m_clients[clientFd].getNickname()) + " " + message + " :You're not on that channel\r\n");
            break;
        case Replies::RPL_TOPIC:
            this->m_send(clientFd, m_makeReplyHeader(Replies::RPL_TOPIC, this->m_clients[clientFd].getNickname()) + ' ' + message + "\r\n");
            break;
        case Replies::RPL_YOUREOPER:
            this->m_send(clientFd, m_makeReplyHeader(Replies::RPL_YOUREOPER, this->m_clients[clientFd].getNickname()) + ' ' + ":You are now an IRC operator\r\n");
            break;
        case Replies::ERR_NOOPERHOST:
            this->m_send(clientFd, m_makeReplyHeader(Replies::ERR_NOOPERHOST, this->m_clients[clientFd].getNickname()) + ' ' + ":No O-lines for your host\r\n");
            break;
        case Replies::ERR_PASSWDMISMATCH:
            this->m_send(clientFd, m_makeReplyHeader(Replies::ERR_PASSWDMISMATCH, this->m_clients[clientFd].getNickname()) + ' ' + ":Password incorrect\r\n");
            break;
        case Replies::RPL_CHANNELMODEIS:
            this->m_send(clientFd, m_makeReplyHeader(Replies::RPL_CHANNELMODEIS, this->m_clients[clientFd].getNickname()) + ' ' + message + "\r\n");
            break;
        case Replies::ERR_CHANOPRIVSNEEDED:
            this->m_send(clientFd, m_makeReplyHeader(Replies::ERR_CHANOPRIVSNEEDED, this->m_clients[clientFd].getNickname()) + ' ' + message + " :You're not channel operator\r\n");
            break;
        case Replies::ERR_USERONCHANNEL:
            this->m_send(clientFd, ":" + this->m_serverName + " 443 <user> <channel> :is already on channel\r\n");
            break;
        case Replies::RPL_NAMREPLY:
            this->m_send(clientFd, ":" + this->m_serverName + " 353 " + message + "\r\n");
            break;
        case Replies::RPL_ENDOFNAMES:
            this->m_send(clientFd, ":" + this->m_serverName + " 366 " + message +" :End of NAMES list\r\n");
            break;
        case Replies::RPL_LIST:
            this->m_send(clientFd, ":" + this->m_serverName + " 321 " + message + "\r\n");
            break;
        case Replies::RPL_LISTEND:
            this->m_send(clientFd, ":" + this->m_serverName + " 323 " + message +" End of LIST\r\n");
            break;
    }
}

int             Server::m_calculateOperators(void) // there is probably a better way to do this, but it doesn't matter.
{
    std::map<int, Client>::iterator it;
    int                             operators;

    operators = 0;
    for (it = this->m_clients.begin(); it != this->m_clients.end(); ++it)
    {
        if (it->second.getModeValue(UserModes::oper)) // both operator types??
            ++operators;
    }
    return (operators);

}

int             Server::m_calculateKnownConnections(void)
{
    std::map<int, Client>::iterator it;
    int                             knowns;

    knowns = 0;
    for (it = this->m_clients.begin(); it != this->m_clients.end(); ++it)
    {
        if (Server::m_isUser(it->second))
            ++knowns;
    }
    return (knowns);
}

int             Server::m_calculateUnknownConnections(void)
{
    std::map<int, Client>::iterator it;
    int                             unknowns;

    unknowns = 0;
    for (it = this->m_clients.begin(); it != this->m_clients.end(); ++it)
    {
        if (!Server::m_isUser(it->second))
            ++unknowns;
    }
    return (unknowns);
}

bool            Server::m_isUser(Client& client) const
{
    return (client.isUserAuth() || client.isNickAuth());
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
                {
                    this->m_poll_count--;
                    continue ;
                }

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
    this->m_clients[clientFd].setAuthComplete();
    if (this->m_clients[clientFd].getMessageQueue().empty())
        this->m_clients[clientFd].getMessageQueue().pop_front();
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
        std::cout << i->getMsg();
        std::cout << "]";
    }
    std::cout << '\n';
}

int	Server::m_manageRecv(std::string message, int clientFd)
{
    t_messageDQeue& messageQueue = this->m_clients[clientFd].getMessageQueue();
    std::string token = strToken(message);

    #ifdef DEBUG
    std::cout << "Queue size == " << messageQueue.size() << std::endl;
    #endif
    while (token.size())
    {
        #ifdef DEBUG
        std::cout << "this is a token: " << token << std::endl;
        #endif
        if (messageQueue.empty() || messageQueue.back().getMsg().find(END_STRING)
                                                        != std::string::npos)
            messageQueue.push_back(Message(token));
        else if (messageQueue.back().getMsg().find(END_STRING) == std::string::npos)
            messageQueue.back().setMsg(messageQueue.back().getMsg() + token);
        token = strToken("");
    }
    #ifdef DEBUG
    std::cout << "Queue size 2 == " << messageQueue.size() << std::endl;
    printQueue(messageQueue);
    #endif
    if (messageQueue.back().getMsg().find(END_STRING) != std::string::npos)
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
    Message & msg = client.getMessageQueue().front();
    msg.parse();
    std::vector<std::string>::iterator it = msg.getArgs().begin();
    std::vector<std::string>::iterator end = msg.getArgs().end();
    int count = 0;

    while (!msg.getArgs().empty() && m_checkNickSyntax(msg) && it < end && count < 5)
    {
        // call the userhost reply
        if (m_nicknames.find(*it) != m_nicknames.end())
            m_reply(client.getFd(), Replies::RPL_USERHOST, m_nicknames[*it], "");
        it++;
        count += 1;
    }
}

// not yet functional as we have to talk about the reply function
void    Server::m_isonCmd(Client & client)
{
    // no need to check the size of the queue cz it must has atleast one message at this point
    Message & msg = client.getMessageQueue().front();
    msg.parse();
    std::vector<std::string>::iterator it = msg.getArgs().begin();
    std::vector<std::string>::iterator end = msg.getArgs().end();

    while (!msg.getArgs().empty() && m_checkNickSyntax(msg) && it < end)
    {
        // call the ison reply
        if (m_nicknames.find(*it) != m_nicknames.end())
            m_reply(client.getFd(), Replies::RPL_USERHOST, m_nicknames[*it], "");
        it++;
    }
}

void    Server::m_nickCmd(Client & client)
{
    Message& msg = client.getMessageQueue().front();
    
    if (msg.getCmd()== NICK_COMMAND && !client.isNickAuth())
    {
        #ifdef DEBUG
        std::cout << "We got the nick command\n";
        #endif
        if (!m_checkNickSyntax(msg))
            return ;
        // ask about this
        std::pair<std::map<std::string, int>::iterator , bool> nick =\
        m_nicknames.insert(std::make_pair(msg.getArgs().front(), client.getFd()));
        if (nick.second)
        {
            // TODO: find a better fix, segfault in this line if there are no arguments
            // These two lines cause a segfault
            // if (!client.getMessageQueue().empty())
            //     client.getMessageQueue().pop_front();
            if (msg.getArgs().size() >= 1)
            {
                client.setNickname(msg.getArgs().front());
                client.setNickAuth();
            }
            // else
            //     client.isNickAuth() = false;
        }
        else
            m_reply(client.getFd(), Replies::ERR_NICKNAMEINUSE, 0, "");
    }
}

void    Server::m_userCmd(Client & client)
{
    Message&    msg = client.getMessageQueue().front();
    std::string mode;
    
    if (msg.getCmd()== USER_COMMAND)
    {
        #ifdef DEBUG
        std::cout << "we got the user command\n";
        #endif
        if (client.isUserAuth())
            m_reply(client.getFd(), Replies::ERR_ALREADYREGISTRED, 0, "");
        else if (msg.getArgs().size() != 3)
            m_reply(client.getFd(), Replies::ERR_NEEDMOREPARAMS, 0, "");
        else
        {
            client.setUsername(msg.getArgs()[0]);
            mode = msg.getArgs()[1];
            client.setRealname(msg.getLiteralMsg());
            client.setUserAuth();
        }
    }
}

// TODO: needs tweaks, mainly a hostname, a string of the address, and an if else depending on if a message was left or not
// TODO: QUIT FROM CHANNELS
void                    Server::m_quitCmd(int clientFd, std::string quitMessage) 
{
    Client& client = this->m_clients[clientFd];

    std::string messageToSend = "ERROR : Closing Link: " + client.getUsername() + "(" 
                                + client.getMessageQueue().front().getLiteralMsg() + ") [address] (Quit: "
                                    + client.getNickname() + ")" + END_STRING;

    this->m_send(clientFd, messageToSend);
    // should I cut the connection
    // stop listening to incoming events
    this->m_eraseClientPoll(clientFd);
    // remove the nickname from the map
    this->m_nicknames.erase(m_clients[clientFd].getNickname());
    // removing it from client list
    this->m_clients.erase(clientFd);
    close(clientFd);
};

/* QUIT ERROR MSG in GeekShed
 *  QUIT :sayonara wa
 *  ERROR :Closing Link: ohachim[ll62-88-161-251-62.ll62.iam.net.ma] (Quit: ohachim)
*/

// AF_UNSPEC comboed with AF_INET6

    // just to compile the thing

bool                    Server::m_grabChannelsNames(Message & msg, std::vector<std::string> & chans, std::vector<std::string> & passes)
{
    std::vector<std::string>::iterator it = msg.getArgs().begin();
    std::vector<std::string>::iterator end = msg.getArgs().end();
    std::string arg;

    //normally i shouldnt parse here but i have a feeling i have to
    if (msg.getArgs().empty())
        return (false);
    while (it != end && (it->at(0) == LOCAL_CHAN || it->at(0) == NETWORKWIDE_CHAN))
    {
        chans.push_back(*it);
        it++;
    }
    while (it != end)
    {
        passes.push_back(*it);
        it++;
    }
    if (chans.empty())
        return (false);
    return (true);
}
// should change to accommodate channel modes concerned
bool                    Server::m_channelExists(std::string channelName)
{
    if (m_channels.find(channelName) != m_channels.end())
        return (true);
    return (false);
}

void                    Server::m_addClientToChan(int clientFd, std::string channelName, std::string password, bool passProtected)
{
    Channel & chan = m_channels[channelName];

    if (chan.isBanned(clientFd))
        m_reply(clientFd, Replies::ERR_BANNEDFROMCHAN, 0, "");
    if ((passProtected && !chan.checkPassword(password)) || (!passProtected && !chan.getPassword().empty()))
        m_reply(clientFd, Replies::ERR_BADCHANNELKEY, 0, "");
    else
    {
        chan.addMember(clientFd);
        m_clients[clientFd].pushChannel(channelName, PEASEANT_MODES);
        m_reply(clientFd, Replies::RPL_TOPIC, 0, "");
    }
}

void                    Server::m_addChannel(int clientFd, std::string channelName, std::string password, bool passProtected)
{
    Channel newChannel(PEASEANT_MODES, clientFd, channelName, channelName.at(0), (passProtected ? password : ""));
    newChannel.addOp(clientFd);
    newChannel.addMember(clientFd);
    m_channels.insert(m_channels.end(), std::pair<std::string, Channel>(channelName, newChannel));
    m_clients[clientFd].pushChannel(channelName, OWNER_MODES);
}

void                    Server::m_joinCmd(Client & client)
{
    std::vector<std::string>    chans;
    std::vector<std::string>    passes;
    Message& msg = client.getMessageQueue().front();
    bool                        passProtected;
    
    ////// TODO: syntax check
    //grab the channels with their passes
    m_grabChannelsNames(msg, chans, passes);
    std::vector<std::string>::iterator it_chan = chans.begin();
    std::vector<std::string>::iterator end_chan = chans.end();
    std::vector<std::string>::iterator it_passes = passes.begin();
    std::vector<std::string>::iterator end_passes = passes.end();

    while (it_chan < end_chan)
    {
        if (it_passes == end_passes)
            passProtected = false;
        if (m_channelExists(*it_chan))
            m_addClientToChan(client.getFd(), *it_chan, *it_passes, passProtected);
        else
            m_addChannel(client.getFd(), *it_chan, *it_passes, passProtected);
    }
}

bool                    Server::m_grabChannelsNames(Message & msg, std::vector<std::string> & chans)
{
    std::vector<std::string>::iterator it = msg.getArgs().begin();
    std::vector<std::string>::iterator end = msg.getArgs().end();
    std::string arg;

    if (msg.getArgs().empty())
        return (false);
    while (it != end && (it->at(0) == LOCAL_CHAN || it->at(0) == NETWORKWIDE_CHAN))
    {
        chans.push_back(*it);
        it++;
    }
    if (chans.empty())
        return (false);
    return (true);
}

void                        Server::m_partCmd(Client &client)
{
    Message& msg = client.getMessageQueue().front();
    std::vector<std::string> channelNames;
    /// TODO: syntax check
    std::vector<std::string>::iterator it = msg.getArgs().begin();
    std::vector<std::string>::iterator end = msg.getArgs().end();
    if (!m_grabChannelsNames(msg, channelNames))
    {
        m_reply(client.getFd(), Replies::ERR_NEEDMOREPARAMS, 0, "");
        return ;
    }
    std::vector<std::string>::iterator it_chan = channelNames.begin();
    std::vector<std::string>::iterator end_chan = channelNames.end();
    std::string partMsg = msg.getLiteralMsg();
    while (it_chan != end_chan)
    {
        if (!m_channelExists(*it_chan))
        {
            m_reply(client.getFd(), Replies::ERR_NOSUCHCHANNEL, 0, *it_chan);
            it_chan++;
            continue ;
        }
        else if (!m_channels[*it_chan].isMember(client.getFd()))
        {
            m_reply(client.getFd(), Replies::ERR_NOTONCHANNEL, 0, "");
            it_chan++;
            continue ;
        }
        m_channels[*it_chan].removeMember(client.getFd());
        m_channels[*it_chan].removeOp(client.getFd());
        client.partChannel(*it_chan);
        // this might be wrong its just a guess
        m_privMsgCmd_noticeCmd(client, false, Message("PART " + *it_chan + " " + client.getNickname()));
        
        it_chan++;
    }
}

void                    Server::m_privMsgCmd_noticeCmd(Client &client, bool notifs)
{
    Message& msg = client.getMessageQueue().front();
    
    if (msg.getArgs().empty() || msg.getLiteralMsg().empty() || msg.getArgs().size() != 1)
    {
        if (notifs && msg.getArgs().empty())
            m_reply(client.getFd(), Replies::ERR_NORECIPIENT, 0, "");
        else if (notifs && msg.getLiteralMsg().empty())
            m_reply(client.getFd(), Replies::ERR_NOTEXTTOSEND, 0, "");
        else if (notifs && msg.getArgs().size() != 1)
            m_reply(client.getFd(), Replies::ERR_TOOMANYTARGETS, 0, "");
        return ;
    }
    std::string target = msg.getArgs().front();
    if (target.at(0) == LOCAL_CHAN || target.at(0) == NETWORKWIDE_CHAN)
    {
        std::map<std::string, Channel>::iterator it_chan = m_channels.find(target);
        if (it_chan == m_channels.end())
        {
            if (notifs)
                m_reply(client.getFd(), Replies::ERR_NOSUCHNICK, 0, "");
            return ;
        }
        Channel & chan = m_channels[target];
        std::vector<int> &members = chan.getMembers();
        std::vector<int>::iterator it = members.begin();
        std::vector<int>::iterator end = members.end();
        while (it != end)
        {
            m_send(*it, ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " " + msg.getMsg());
            it++;
        }
    }
    else
    {
        std::map<std::string, int>::iterator it_user = m_nicknames.find(target);
        if (it_user == m_nicknames.end())
        {
            if (notifs)
                m_reply(client.getFd(), Replies::ERR_NOSUCHNICK, 0, "");
            return ;
        }
        int clientFd = m_nicknames[target];
        m_send(m_nicknames[target], ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " " + msg.getMsg());
    }
}


// an overload of the privmsg command
void                    Server::m_privMsgCmd_noticeCmd(Client &client, bool notifs, Message msg) // this would be used by the server it self
{
    std::string target = msg.getArgs().front();
    if (target.at(0) == LOCAL_CHAN || target.at(0) == NETWORKWIDE_CHAN)
    {
        std::map<std::string, Channel>::iterator it_chan = m_channels.find(target);
        Channel & chan = m_channels[target];
        std::vector<int> &members = chan.getMembers();
        std::vector<int>::iterator it = members.begin();
        std::vector<int>::iterator end = members.end();
        while (it != end)
        {
            m_send(*it, ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " " + msg.getMsg());
            it++;
        }
    }
    else
    {
        std::map<std::string, int>::iterator it_user = m_nicknames.find(target);
        int clientFd = m_nicknames[target];
        m_send(m_nicknames[target], ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " " + msg.getMsg());
    }
}

void            Server::m_kickCmd(Client &client)
{
    Message& msg = client.getMessageQueue().front();
    std::vector<std::string>    chans;
    std::vector<std::string>    nicks;
    m_grabChannelsNames(msg, chans, nicks);
    if (msg.getArgs().size() < 2 || (chans.size() != 1 && nicks.size() != chans.size()))
    {
        m_reply(client.getFd(), Replies::ERR_NEEDMOREPARAMS, 0, "");
        return ;
    }
    std::vector<std::string>::iterator it_chan = chans.begin();
    std::vector<std::string>::iterator end_chan = chans.begin();
    std::vector<std::string>::iterator it_nick = nicks.begin();
    std::vector<std::string>::iterator end_nick = nicks.begin();
    if (chans.size() == 1)
    {
        if (m_channels[*it_chan].isOp(client.getFd()))
        {
            while (it_nick != end_nick)
            {
                m_channels[*it_chan].removeMember(m_nicknames[*it_nick]);
                m_channels[*it_chan].removeOp(m_nicknames[*it_nick]);
                m_clients[m_nicknames[*it_nick]].popChannel(*it_chan);
                m_privMsgCmd_noticeCmd(client, false, Message("KICK " + *it_nick + " :" + client.getNickname()));
                it_nick++;
            }
        }
        else if (!m_channels[*it_chan].isMember(client.getFd()))
            m_reply(client.getFd(), Replies::ERR_NOTONCHANNEL, 0, "");
        else
            m_reply(client.getFd(), Replies::ERR_CHANOPRIVSNEEDED, 0, "");
    }
    else
    {
        while (it_chan != end_chan)
        {
            if (m_channels[*it_chan].isOp(client.getFd()))
            {
                m_channels[*it_chan].removeMember(m_nicknames[*it_nick]);
                m_channels[*it_chan].removeOp(m_nicknames[*it_nick]);
                m_clients[m_nicknames[*it_nick]].popChannel(*it_chan);
                m_privMsgCmd_noticeCmd(client, false, Message("KICK " + *it_nick + " :" + client.getNickname()));
            }
            else if (!m_channels[*it_chan].isMember(client.getFd()))
                m_reply(client.getFd(), Replies::ERR_NOTONCHANNEL, 0, "");
            else
                m_reply(client.getFd(), Replies::ERR_CHANOPRIVSNEEDED, 0, "");
            it_nick++;
            it_chan++;
        }
    }
}
// }
// TODO: REMOVE EXTRA SPACES FROM NICKS IF THERE ARE ANY
// TODO: maybe make so that the last parameter isn't necessart


void            Server::m_inviteCmd(Client &client)
{
    Message& msg = client.getMessageQueue().front();
    std::vector<std::string> &args = msg.getArgs();
    if (args.size() != 2)
        m_reply(client.getFd(), Replies::ERR_NEEDMOREPARAMS, 0, "");
    std::string target = *args.begin();
    std::string chanName = *(args.begin() + 1);
    if (!m_channelExists(chanName))
        m_reply(client.getFd(), Replies::ERR_NOSUCHCHANNEL, 0, "");
    else if (m_nicknames.find(target) == m_nicknames.end())
        m_reply(client.getFd(), Replies::ERR_NOSUCHNICK, 0, "");
    else if (!m_channels[chanName].isMember(client.getFd()))
        m_reply(client.getFd(), Replies::ERR_NOTONCHANNEL, 0, "");
    else if (!m_channels[chanName].isOp(m_nicknames[target]))
        m_reply(client.getFd(), Replies::ERR_CHANOPRIVSNEEDED, 0, "");
    else if (m_channels[chanName].isMember(m_nicknames[target]))
        m_reply(client.getFd(), Replies::ERR_USERONCHANNEL, 0, "");
    else
    {
        m_privMsgCmd_noticeCmd(client, false, Message("INVITE " + target + " :" + client.getNickname()));
        m_privMsgCmd_noticeCmd(client, false, Message("INVITE " + client.getNickname() + " :" + client.getNickname()));
        m_channels[chanName].addInvited(m_nicknames[target]);
    }
}

void    Server::m_mapKeysToVector(std::vector<std::string> &vector, std::map<std::string, Channel> &map)
{
    std::map<std::string, Channel>::iterator it_m = map.begin();
	std::map<std::string, Channel>::iterator end_m = map.end();
	while (it_m != end_m)
	{
		vector.push_back(it_m->first);
		it_m++;
	}
}

std::string Server::m_composeNames(std::string channelName)
{
    std::string message = "";
    if (m_channelExists(channelName))
    {
        // if (m_channels[channelName].getModeValue())
    }
    return(message);
}

std::string Server::m_composeList(std::string channelName)
{
    std::string message = "";
    if (m_channelExists(channelName))
    {
        // if (m_channels[channelName].getModeValue())
    }
    return(message);
}

void    Server::m_namesCmd_listCmd(Client & client, std::string cmd)
{
    Message& msg = client.getMessageQueue().front();
    std::vector<std::string>    chans;
    m_grabChannelsNames(msg, chans);
    if (chans.empty())
        m_mapKeysToVector(chans, m_channels);
    
    std::vector<std::string>::iterator    it_chan = chans.begin();
    std::vector<std::string>::iterator    end_chan = chans.end();
    while (it_chan != end_chan)
    {
        if (cmd == NAMES_COMMAND)
        {
            m_reply(client.getFd(), Replies::RPL_NAMREPLY, 0, m_composeNames(*it_chan));
            m_reply(client.getFd(), Replies::RPL_ENDOFNAMES, 0, *it_chan);
        }
        else if (cmd == LIST_COMMAND)
        {
            m_reply(client.getFd(), Replies::RPL_LIST, 0, m_composeList(*it_chan));
            m_reply(client.getFd(), Replies::RPL_LISTEND, 0, *it_chan);
        }
        it_chan++;
    }
}

std::string    Server::m_possibleCommands[NUM_COMMANDS] = {"USER", "NICK", "PASS", "USERHOST", "QUIT", "ISON", "MODE", "PONG", "PING", "MOTD", "AWAY", "LUSERS", "WHOIS", "TOPIC", "JOIN", "PART", "KICK", "PRIVMSG", "NOTICE", "OPER"}; // TODO: UPDATE