
#include "Server.hpp"


char* strdup(const char *s);

Server::Server(void) : m_port(DEFAULT_PORT), m_hostname(DEFAULT_HOSTNAME), m_version("0.1"), m_maxClients(150), m_authenticatedUserNum(0)
{
    this->m_servinfo = NULL;
    this->m_sockfd = -1;
    this->m_poll_count = 0;
    m_numOps = 0;
    memset(&(this->m_socketInfo), 0, sizeof(this->m_socketInfo));
    memset(&(this->m_hints), 0, sizeof(this->m_hints));
    memset(&(this->m_addr_in), 0, sizeof(this->m_addr_in));
    memset(&(this->m_addr_in6), 0, sizeof(this->m_addr_in6));
}

void    Server::initializeCmdFuncs(void)
{
    this->m_cmdFuncs.insert(std::make_pair(QUIT_COMMAND, &Server::m_quitCmd));
    this->m_cmdFuncs.insert(std::make_pair(PART_COMMAND, &Server::m_partCmd));
    this->m_cmdFuncs.insert(std::make_pair(KICK_COMMAND, &Server::m_kickCmd));
    this->m_cmdFuncs.insert(std::make_pair(INVITE_COMMAND, &Server::m_inviteCmd));
    this->m_cmdFuncs.insert(std::make_pair(MODE_COMMAND, &Server::m_modeCmd));
    this->m_cmdFuncs.insert(std::make_pair(PING_COMMAND, &Server::m_pingCmd));
    this->m_cmdFuncs.insert(std::make_pair(PONG_COMMAND, &Server::m_pongCmd));
    this->m_cmdFuncs.insert(std::make_pair(MOTD_COMMAND, &Server::m_motdCmd));
    this->m_cmdFuncs.insert(std::make_pair(AWAY_COMMAND, &Server::m_awayCmd));
    this->m_cmdFuncs.insert(std::make_pair(LUSERS_COMMAND, &Server::m_lusersCmd));
    this->m_cmdFuncs.insert(std::make_pair(WHOIS_COMMAND, &Server::m_whoisCmd));
    this->m_cmdFuncs.insert(std::make_pair(ISON_COMMAND, &Server::m_isonCmd));
    this->m_cmdFuncs.insert(std::make_pair(TOPIC_COMMAND, &Server::m_topicCmd));
    this->m_cmdFuncs.insert(std::make_pair(OPER_COMMAND, &Server::m_operCmd));
    this->m_cmdFuncs.insert(std::make_pair(JOIN_COMMAND, &Server::m_joinCmd));
    this->m_cmdFuncs.insert(std::make_pair(USERHOST_COMMAND, &Server::m_userhostCmd));
    this->m_cmdFuncs.insert(std::make_pair(NAMES_COMMAND, &Server::m_namesCmd_listCmd));
    this->m_cmdFuncs.insert(std::make_pair(LIST_COMMAND, &Server::m_namesCmd_listCmd));
    this->m_cmdFuncs.insert(std::make_pair(PRIVMSG_COMMAND, &Server::m_privMsgCmd_noticeCmd));
    this->m_cmdFuncs.insert(std::make_pair(NOTICE_COMMAND, &Server::m_privMsgCmd_noticeCmd));
    this->m_cmdFuncs.insert(std::make_pair(NICK_COMMAND, &Server::m_nickCmd));
    this->m_cmdFuncs.insert(std::make_pair(SEND_COMMAND, &Server::m_sendCmd));
    this->m_cmdFuncs.insert(std::make_pair(FETCH_COMMAND, &Server::m_fetchCmd));
}

Server::~Server(void)
{
}

Server::Server(std::string port, std::string hostname, std::string name, int maxClients) : m_serverName(name), m_port(port), m_hostname(hostname), m_version("1.0"), m_maxClients(maxClients), m_authenticatedUserNum(0)
{
    this->m_servinfo = NULL;
    this->m_sockfd = -1;
    this->m_poll_count = 0;
    m_numOps = 0;
    
    try
    {
        std::stoi(port);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
         if (this->m_servinfo)
            freeaddrinfo(this->m_servinfo);
        std::exit(1);
    }
    
    memset(&(this->m_socketInfo), 0, sizeof(this->m_socketInfo));
    memset(&(this->m_hints), 0, sizeof(this->m_hints));
    memset(&(this->m_addr_in), 0, sizeof(this->m_addr_in));
    memset(&(this->m_addr_in6), 0, sizeof(this->m_addr_in6));
}

Server::Server(const Server& serverRef) : m_maxClients(serverRef.m_maxClients)
{
    (void)serverRef;
    *this = serverRef;
}

Server&         Server::operator=(const Server& serverRef)
{
    std::cout << "Server not copied, reference returned\n";
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
    if ((ret = getaddrinfo(hostname, this->m_port.c_str(), &this->m_hints, &this->m_servinfo)))
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

int             Server::m_setSocket(t_socketInfo socketInfo, t_sockaddr* addr, socklen_t addrlen)
{
    int yes = 1;

    if ((this->m_sockfd = socket(socketInfo.family, socketInfo.socktype, socketInfo.protocol)) == -1)
    {
        std::perror("socket");
        return (-1);
    }
    if (setsockopt(this->m_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)))
    {
        std::perror("setsocketopt");
        if (this->m_servinfo)
            freeaddrinfo(this->m_servinfo);
        exit(1);
    }
    if ((bind(this->m_sockfd, addr, addrlen)))
    {
        std::perror("bind");
        return (-2);
    }
    fcntl(this->m_sockfd, F_SETFL, O_NONBLOCK);
    return (0);
}

int             Server::setSockfd(int family)
{
    t_addrinfo*     p;
    int             ret;

    for (p = this->m_servinfo; p != NULL; p = p->ai_next)
    {
        if (p->ai_family == family || family == AF_UNSPEC)
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
        std::cerr << "Couldn't find a socket." << std::endl;
        std::exit(1);
    }
    return (0);
}

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
        std::perror("listen: ");
        if (this->m_servinfo)
            freeaddrinfo(this->m_servinfo);
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
        std::perror("poll: ");
        if (this->m_servinfo)
            freeaddrinfo(this->m_servinfo);
        exit(-1);
    }
}

void*           Server::m_getInAddr(t_sockaddr* addr) const
{
    if (addr->sa_family == AF_INET)
        return (&(((t_sockaddr_in*)addr)->sin_addr));
    return (&(((t_sockaddr_in6*)addr)->sin6_addr));
}

// std::string            Server::convertToHostname(t_sockaddr_storage& remoteAddr, int sock_fd) // for the project continuation
// {
//     char buffer[INET6_ADDRSTRLEN];
//     int err = getnameinfo((struct sockaddr*)&remoteAddr, sizeof(t_sockaddr_storage), buffer,sizeof(buffer), 0, 0, 0);
//     if (err)
//     {
//         printf("Failed to get hostname!\nExiting...");
//         if (this->m_servinfo)
//             freeaddrinfo(this->m_servinfo);
//         exit(1);
//     }
//     std::string s(buffer);
//     return (s);
// }

int            Server::m_manageServerEvent(void)
{
    t_sockaddr_storage  remoteAddr;
    socklen_t           addrlen = sizeof(t_sockaddr_storage);
    int                 newFd = accept(this->m_sockfd, (t_sockaddr*)&remoteAddr,
                                        &addrlen);

    if (newFd == -1)
    {
        std::perror("accept: ");
        return (-1);
    }

    int yes = 1;

    if (setsockopt(newFd, SOL_SOCKET, SO_NOSIGPIPE, &yes, sizeof(yes)))
    {
        std::perror("setsocketopt");
        if (this->m_servinfo)
            freeaddrinfo(this->m_servinfo);
        exit(1);
    }

    if (static_cast<int>(this->m_clients.size()) < this->m_maxClients)
    {
        fcntl(newFd, F_SETFL, O_NONBLOCK);
        this->m_clients[newFd] = Client(newFd, remoteAddr, addrlen);
        this->m_clients[newFd].setHostname(inet_ntoa(((t_sockaddr_in*)&remoteAddr)->sin_addr));
        this->m_pfds.push_back((t_pollfd){newFd, POLLIN, 0});
    }
    else
    {
        m_reply(newFd, Replies::RPL_BOUNCE, "");
        close(newFd);
        return (-2);
    }
    return (0);
}

bool            Server::m_isAuthenticated(int clientFd)
{
    return (this->m_clients[clientFd].isAuthComplete());
}

bool            Server::m_isChannelPrefix(char c) const
{
    return (c == '#' || c == '&');
}

bool            Server::m_isMaskUserMatch(std::string hostname, std::string TLD)
{
    hostname = hostname.erase(0, hostname.size() - TLD.size());

    return (hostname == TLD || TLD.empty());
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
    if (channelModes.size() == 1)
        channelModes = "";
    return (channel.getName() + ' ' + channelModes);
}

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

int            Server::m_manageChannelModes(char mode, char prefix, std::vector<std::string> arguments, std::string& modeChanges)
{
    if (m_nicknames.find(arguments[2]) == m_nicknames.end())
            return (-1);

    Client& client = m_clients[m_nicknames[arguments[2]]];
    int modeInt = client.findChanMode(mode);
    bool modeValue = client.getModeValue(mode, arguments[0]);
    
    if (prefix == '+')
    {
        if (!modeValue)
        {
            client.turnOnMode(modeInt, arguments[0]);
            modeChanges += mode;
            if (modeChanges.find(arguments[2]) == std::string::npos)
                modeChanges += ' ' + arguments[2];
        }
    }
    else if (prefix == '-')
    {
        if (modeValue)
        {
            client.turnOffMode(modeInt, arguments[0]);
            modeChanges += mode;
            if (modeChanges.find(arguments[2]) == std::string::npos)
                modeChanges += ' ' + arguments[2];
        }
    }
    return (0);
}

bool                                Server::m_isMask(std::string mask)
{
    return (mask == "*!*@*" || (mask.substr(0, 6) == "*!*@*." && mask.size() > 6));
}


std::string                     Server::m_getTLD(std::string mask)
{
    if (mask == "*")
        return ("*");
    return (mask.erase(0, 5));
}


std::vector<std::string>                         Server::m_manageMaskMode(char mode, std::vector<std::string> arguments)
{
        Channel& channel = m_channels[arguments[0]];
        std::vector<std::string> maskList;
        
        if (mode == 'e')
            maskList = channel.getExceptionBanMasks();
        else if (mode == 'b')
            maskList = channel.getBanMasks();
            
        else if (mode == 'I')
            maskList = channel.getInviteMasks();
        else
            return (std::vector<std::string>());

       return (maskList);
}

void            Server::m_manageMaskMode(char mode, char prefix, std::vector<std::string> arguments, int& paramToUseIndex)
{
    Channel&    channel = m_channels[arguments[0]];

    if (prefix == '+')
    {
        if (mode =='e')
            channel.m_addToMaskVector(channel.getExceptionBanMasks(), arguments[paramToUseIndex]);
        else if (mode == 'b')
        {
            channel.m_addToMaskVector(channel.getBanMasks(), arguments[paramToUseIndex]);
        }
        else if (mode == 'I')
        {
            channel.m_addToMaskVector(channel.getInviteMasks(), arguments[paramToUseIndex]);
        }
        ++paramToUseIndex;
    }
    else if (prefix == '-')
    {
        if (mode == 'e')
            channel.getExceptionBanMasks().clear();
        else if (mode == 'b')
            channel.getBanMasks().clear();
        else if (mode == 'I')
            channel.getInviteMasks().clear();
    }
}

void            Server::m_listMasks(std::vector<std::string> maskList, char mode, Client& client, Channel& channel)
{
    int replyCodeStart;
    int replyCodeEnd;
    std::string listType;  
    switch (mode)
    {
        case 'b':
            replyCodeStart = Replies::RPL_BANLIST;
            replyCodeEnd = Replies::RPL_ENDOFBANLIST;
            listType = "ban list";
            break;
        case 'e':
            replyCodeStart = Replies::RPL_EXCEPTLIST;
            replyCodeEnd = Replies::RPL_ENDOFEXCEPTLIST;
            listType = "exception list";
            break;
        case 'I':
            replyCodeStart = Replies::RPL_INVITELIST;
            replyCodeEnd = Replies::RPL_ENDOFINVITELIST; 
            listType = "invite list";
            break;
        default:
            return;
    }
    for (int i = 0; i < static_cast<int>(maskList.size()); ++i)
    {
        std::string mask;
        
        if (maskList[i] == "*")
            mask = "*!*@*";
        else
            mask = "*!*@*" + maskList[i];
        m_reply(client.getFd(), replyCodeStart, channel.getName() + ' ' + mask);
    }
    m_reply(client.getFd(), replyCodeEnd, channel.getName() + " :End of " + listType);
}

std::string     Server::m_composeUserNotInChannel(std::string channelName, std::string clientName)
{
    return (clientName + ' ' + channelName + " :They aren't on that channel");
}

std::string            Server::m_executeModes(std::vector<std::string> arguments, Channel& channel, Client& client)
{
    std::string modes = arguments[1];
    char        prefix = '\0';
    int         paramToUseIndex = 2;
    int         start = 0;
    std::string modeChanges = "";

    if (modes[0] == '+' || modes[0] == '-')
    {
        prefix = modes[0];
        modeChanges += prefix;
        start = 1;
    }
    for (int i = start; i < static_cast<int>(modes.size()); ++i)
    {
        if (m_isAttributeSetterMode(modes[i]))
        {
            if (channel.manageAttribute(modes[i], prefix, arguments, modeChanges))
                m_reply(client.getFd(), Replies::ERR_INTNEEDED, arguments[2]);
        }
        else if (m_isSimpleChannelMode(modes[i]))
            channel.manageSimpleMode(modes[i], prefix, modeChanges);
        else if (m_isUserSpecificChannelMode(modes[i]) && modes[i] != 'O')
        {
            if (m_manageChannelModes(modes[i], prefix, arguments, modeChanges))
            {
                m_reply(client.getFd(), Replies::ERR_USERNOTINCHANNEL,
                                m_composeUserNotInChannel(channel.getName(), client.getNickname()));
            }
        }
        else if (m_isMaskMode(modes[i]))
        {
            if (prefix == '\0')
            {
                std::vector<std::string> maskList = m_manageMaskMode(modes[i], arguments);
                m_listMasks(maskList, modes[i], client, channel);
            }
            else
            {
                m_manageMaskMode(modes[i], prefix, arguments, paramToUseIndex);
                modeChanges += modes[i];
                if (prefix == '+')
                    modeChanges += " " + arguments[paramToUseIndex - 1];
            }
        }
        else if (modes[i] == 'O')
            m_reply(client.getFd(), Replies::RPL_UNIQOPIS, channel.getName()
                            + ' ' + channel.getCreatorName());
        else
            m_reply(client.getFd(), Replies::ERR_UMODEUNKNOWNFLAG, "");
    }
    return (modeChanges);
}

bool            Server::m_areModesMasks(std::string modes)
{
    for (int i = 0; i < static_cast<int>(modes.size()); ++i)
    {
        if (!m_isMaskMode(modes[i]))
            return (false);
    }
    return (true);
}

bool            Server::m_isModesSyntaxValid(std::vector<std::string> arguments)
{
    char    prefix = arguments[1][0];
    int     attributeSetters = 0;
    int     userModeSetters = 0;
    int     chanModeSetters = 0;
    int     maskSetters = 0;
    int     argNum = arguments.size() - 2;

    int start = 0;
    std::string modes;
    if (prefix == '+' || prefix == '-')
        modes = arguments[1].erase(0, 1);
    else
        modes = arguments[1];
    for (int i = start; i < static_cast<int>(modes.size()); ++i)
    {
        if (m_isAttributeSetterMode(modes[i]))
            attributeSetters += 1;
        else if (m_isSimpleChannelMode(modes[i]))
            chanModeSetters += 1;
        else if (m_isUserSpecificChannelMode(modes[i]))
            userModeSetters += 1;
        else if (m_isMaskMode(modes[i]))
            maskSetters += 1;
    }
    if (prefix == '+' || prefix == '-')
    {
        if (attributeSetters + chanModeSetters + userModeSetters + maskSetters > 3)
            return (false);
        if ((attributeSetters && userModeSetters)
            || (attributeSetters && maskSetters)
            || (maskSetters && userModeSetters))
            return (false);
        if (attributeSetters > 1)
            return (false);
        if (prefix == '+' && maskSetters && maskSetters != argNum)
            return (false);
        if (prefix == '-' && maskSetters && argNum)
            return (false);
        if (prefix == '-' && attributeSetters && arguments[1][0] == 'l' && argNum)
            return (false);
        if (arguments[1][0] == 'O')
            return (false);
        if (attributeSetters && argNum != 1 && arguments[1][0] != 'l')
            return (false);
        if (userModeSetters && argNum != 1)
            return (false);
    }
    else
    {
        if (attributeSetters)
            return (false);
        if (chanModeSetters)
            return (false);
        else if (userModeSetters > 1)
            return (false);
        if (userModeSetters == 1 && arguments[1].find('O') == std::string::npos)
            return (false);
    }
    return (true);
}


void            Server::m_pongCmd(Client& client)
{
    (void)client;
    std::cout << "Pong cmd received\n";
}
void            Server::m_channelModeCmd(Client& client, Message& message)
{
    std::vector<std::string> arguments = message.getArgs();
    std::string channelName = arguments[0];

    if (!m_channelExists(channelName))
    {
        m_reply(client.getFd(), Replies::ERR_NOSUCHCHANNEL, channelName);
        return ;
    }
    if (arguments.size() == 1)
    {
        m_reply(client.getFd(), Replies::RPL_CHANNELMODEIS, channelName + ' ' + m_composeChannelModes(channelName));
        return ;
    }
    if (!m_isClientOper(client, arguments[0]) && !client.getModeValue(UserModes::oper)) 
    {
        m_reply(client.getFd(), Replies::ERR_CHANOPRIVSNEEDED, channelName);
        return ;
    }
    if (arguments.size() >= 2)
    {
        if (!m_isModesSyntaxValid(arguments))
        {
            m_reply(client.getFd(), Replies::ERR_WRONGCHANMODESYNTAX, channelName);
            return ;
        }
        std::string modeChanges = m_executeModes(arguments, m_channels[channelName], client);
        if (modeChanges.size() > 1)
        {
            m_p_privMsgCmd_noticeCmd(client, Message("MODE " + channelName + ' ' + modeChanges), channelName);
            m_p_privMsgCmd_noticeCmd(client, Message("MODE " + channelName + ' ' + modeChanges), client.getNickname());
        }
    }
}

std::string     Server::m_composeUserModes(Client& client)
{
    std::string userModes = "+";

    for (int i = 0; i < UserModes::total; ++i)
    {
        if (client.getModeValue(i))
            userModes += Client::potentialModes[i];
    }
    return (userModes);
}

void            Server::m_userModeCmd(Client& client, Message& message)
{
    if (message.getArgs()[0] != client.getNickname())
    {
        m_reply(client.getFd(), Replies::ERR_USERSDONTMATCH, "");
        return ;
    }

    if (message.getArgs().size() == 1)
    {
        std::string userModes = m_composeUserModes(client);
        if (userModes.size() > 1)
            m_reply(client.getFd(), Replies::RPL_UMODEIS, userModes);
        return;
    }

    std::string modeChanges;

    char prefix = message.getArgs()[1][0];

    modeChanges += prefix;
    for (int j = 1; j < int(message.getArgs()[1].size()); ++j)
    {
        int modeNum = Client::findMode(message.getArgs()[1][j]);

        if (modeNum == UserModes::away)
            continue ;
        if (modeNum == -1)
        {
            m_reply(client.getFd(), Replies::ERR_UMODEUNKNOWNFLAG, "");
            continue ;
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
                if (modeNum == UserModes::oper)
                    --m_numOps;
                client.turnOffMode(modeNum);
                modeChanges += message.getArgs()[1][j];
            }
        }
    }
    if (modeChanges.size() > 1)
        m_reply(client.getFd(), Replies::RPL_UMODEIS, modeChanges);
}

void            Server::m_modeCmd(Client& client)
{
    Message& message = client.getMessageQueue().front();
    if (message.getArgs().empty())
    {
        m_reply(client.getFd(), Replies::ERR_NEEDMOREPARAMS, MODE_COMMAND);
        return ;
    }
    std::string potentialChannelName = message.getArgs()[0];
    if (m_isChannelPrefix(potentialChannelName[0]))
        m_channelModeCmd(client, message);
    else
        m_userModeCmd(client, message);
}

std::string         Server::m_composeMotd(std::ifstream& motdFile, std::string clientNick)
{
    std::string     line;
    std::string     motd;
    int             count;
    int             cursor;

    while (std::getline(motdFile, line))
    {
        if (!motd.empty())
            motd += ':' + this->m_serverName + " 372 " + clientNick + " :-\t  " + line;
        motd += '\n';
    }
    count = 0;
    cursor = 0;
    while (cursor < static_cast<int>(motd.size()))
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


void                Server::m_motdCmd(Client& client)
{
    std::ifstream motd("motd.txt");

    if (motd.fail())
    {
        m_reply(client.getFd(), Replies::ERR_NOMOTD, "");
        return ;
    }
    m_reply(client.getFd(), Replies::RPL_MOTDSTART, "");
    m_reply(client.getFd(), Replies::RPL_MOTD, this->m_composeMotd(motd, client.getNickname()));
    m_reply(client.getFd(), Replies::RPL_ENDOFMOTD, "");
}

void                Server::m_awayCmd(Client& client)
{
    Message& message = client.getMessageQueue().front();

    if (message.getLiteralMsg().size())
    {
        client.setAwayMsg(message.getLiteralMsg());
        client.turnOnMode(UserModes::away);
        m_reply(client.getFd(), Replies::RPL_NOWAWAY, "");
    }
    else
    {
        client.setAwayMsg("");
        if (!client.getModeValue(UserModes::away))
        {
            m_reply(client.getFd(), Replies::ERR_NOTAWAY, "");
            return ;
        }
        client.turnOffMode(UserModes::away);
        m_reply(client.getFd(), Replies::RPL_UNAWAY, "");
    }
}

void                Server::m_lusersCmd(Client& client)
{
    m_reply(client.getFd(), Replies::RPL_LUSERCLIENT, "");
    m_reply(client.getFd(), Replies::RPL_LUSEROP, "");
    m_reply(client.getFd(), Replies::RPL_LUSERUNKNOWN, "");
    m_reply(client.getFd(), Replies::RPL_LUSERCHANNELS, "");
    m_reply(client.getFd(), Replies::RPL_LUSERME, "");
}

std::string                 Server::m_makeReplyHeader(int replyNum, std::string nickname)
{
    std::string strReplyNum = intToString(replyNum);
    int         len = strReplyNum.size();
    std::string s = std::string(3 - len, '0');
    std::string header = ':' + this->m_serverName + ' ' + s + strReplyNum + ' ' + nickname;
    return (header);
}

std::string                Server::m_composeWhoisQuery(Client& client, Client& queryClient, int replyCode)
{
    std::string awayMsg = queryClient.getAwayMsg();

    if (awayMsg.empty())
        awayMsg = "is away";
    switch (replyCode)
    {
        case Replies::RPL_WHOISUSER:
            return (m_makeReplyHeader(Replies::RPL_WHOISUSER, client.getNickname()) + ' ' + queryClient.getNickname()
                                        + " ~" + queryClient.getUsername() + ' ' + queryClient.getHostname() + " * " + queryClient.getRealname());
        case Replies::RPL_WHOISSERVER:
            return (m_makeReplyHeader(Replies::RPL_WHOISSERVER, client.getNickname()) + ' ' + queryClient.getNickname()
                                                    + ' ' + m_serverName + " :Morocco 1337 Server, Provided by 1337 students Powered by Itisalat Al Maghreb.");
        case Replies::RPL_AWAY:
            return (m_makeReplyHeader(Replies::RPL_AWAY, client.getNickname()) + ' ' + queryClient.getNickname() + ' ' + awayMsg);
        case Replies::RPL_WHOISOPERATOR:
            if (queryClient.getModeValue(UserModes::oper))
                return (m_makeReplyHeader(Replies::RPL_WHOISOPERATOR, client.getNickname()) + ' ' + queryClient.getNickname() + " :is an IRC operator");
        case Replies::RPL_ENDOFWHOIS:
            return (m_makeReplyHeader(Replies::RPL_ENDOFWHOIS, client.getNickname()) + ' ' + queryClient.getNickname()
                                         + ' ' + ":End of WHOIS list.");
        case Replies::RPL_WHOISCHANNELS:
            return ("");
        default:
            return ("");
    }
}

void                Server::m_pingCmd(Client& client)
{
    m_reply(client.getFd(), Replies::RPL_PINGREQUEST,"");
}

std::string         Server::m_composeWhoIsChannels(Client& client, Client& queryClient, std::string channelName, std::string appliedModes)
{
    return (m_makeReplyHeader(Replies::RPL_WHOISCHANNELS, client.getNickname())
        + " :" + appliedModes
        + "~" + queryClient.getUsername()
        + " " + channelName);
}

void                Server::m_whoisCmd(Client& client)
{
    Message&    message = client.getMessageQueue().front();
    if (message.getArgs().empty())
    {
        m_reply(client.getFd(), Replies::ERR_NEEDMOREPARAMS, WHOIS_COMMAND);
        return ;
    }
    std::string queryClientName = message.getArgs()[0];
    int clientFd;
    if (this->m_nicknames.find(queryClientName) == this->m_nicknames.end())
    {
        m_reply(client.getFd(), Replies::ERR_NOSUCHNICK,queryClientName);
        return ;
    }
    else
        clientFd = this->m_nicknames[queryClientName];
    
    Client&     queryClient = this->m_clients[clientFd];

    m_reply(client.getFd(), Replies::RPL_WHOISUSER, m_composeWhoisQuery(client, queryClient, Replies::RPL_WHOISUSER));
    m_reply(client.getFd(), Replies::RPL_WHOISSERVER, m_composeWhoisQuery(client, queryClient, Replies::RPL_WHOISSERVER));
    if (queryClient.getModeValue(UserModes::oper))
        m_reply(client.getFd(), Replies::RPL_WHOISOPERATOR, m_composeWhoisQuery(client, queryClient, Replies::RPL_WHOISOPERATOR));
    if (queryClient.getModeValue(UserModes::away))
        m_reply(client.getFd(), Replies::RPL_AWAY, m_composeWhoisQuery(client, queryClient, Replies::RPL_AWAY));

    std::vector<std::string> queryClientChannels = queryClient.getChannels();
    std::string appliedModes;

    for (int i = 0; i < static_cast<int>(queryClientChannels.size()); ++i)
    {
        appliedModes = "";
        if (queryClient.getModeValue(ChannelModes::O_Creator, queryClientChannels[i])
            || queryClient.getModeValue(ChannelModes::o_OperatorPrivilege, queryClientChannels[i]))
            appliedModes += "@";
        if (queryClient.getModeValue(ChannelModes::v_voicePrivilege, queryClientChannels[0]))
            appliedModes += "+";
        if (appliedModes.size())
            m_reply(client.getFd(), Replies::RPL_WHOISCHANNELS, m_composeWhoIsChannels(client, queryClient, queryClientChannels[i], appliedModes));
    }
    m_reply(client.getFd(), Replies::RPL_ENDOFWHOIS, m_composeWhoisQuery(client, queryClient, Replies::RPL_ENDOFWHOIS));

}

std::string         Server::m_composeRplTopic(Channel& channel)
{
    return (channel.getName() + " :" + ((channel.getTopic().size() <= 1) ? "NO TOPIC" : channel.getTopic().erase(0, 1)));
}

void                Server::m_topicCmd(Client& client)
{
    Message& message = client.getMessageQueue().front();

    std::vector<std::string> arguments = message.getArgs();
    std::string topicToSet = message.getLiteralMsg();
    if (arguments.empty())
    {
        m_reply(client.getFd(), Replies::ERR_NEEDMOREPARAMS, TOPIC_COMMAND);
        return ;
    }
    std::vector<std::string> &channels = client.getChannels();
    if (std::find(channels.begin(), channels.end(), arguments[0]) == channels.end())
    {
        m_reply(client.getFd(), Replies::ERR_NOTONCHANNEL, arguments[0]);
        return ;
    }
    std::string channelName = arguments[0]; 

    // if (arguments.size() == 1)
    if (topicToSet.empty())
    {
        if (m_channels[channelName].getTopic().empty())
        {
            m_reply(client.getFd(), Replies::RPL_NOTOPIC,  channelName);
            return;
        }
        m_reply(client.getFd(), Replies::RPL_TOPIC, m_composeRplTopic(m_channels[channelName]));
        return;
    }
    // if (arguments.size() == 2)
    if (topicToSet.size())
    {
        Channel& channel = m_channels[channelName];

        if (channel.getModeValue(ChannelModes::t_topicOperatorOnly))
        {
            if (client.getModeValue(ChannelModes::o_OperatorPrivilege, channelName)
                    || client.getModeValue(ChannelModes::O_Creator, channelName) || client.getModeValue(UserModes::oper))
            {
                channel.getTopic() = topicToSet.erase(0, 1);
                m_p_privMsgCmd_noticeCmd(client, Message(" TOPIC " + channelName + " :" + topicToSet), channelName);
                m_p_privMsgCmd_noticeCmd(client, Message(" TOPIC " + channelName + " :" + topicToSet), client.getNickname());
            }

            else
                m_reply(client.getFd(), Replies::ERR_CHANOPRIVSNEEDED, channelName);
        }
        else
        {
            channel.getTopic() = topicToSet.erase(0, 1);
            m_p_privMsgCmd_noticeCmd(client, Message(" TOPIC " + channelName + " :" + topicToSet), channelName);
            m_p_privMsgCmd_noticeCmd(client, Message(" TOPIC " + channelName + " :" + topicToSet), client.getNickname());
        }
    }
    
}

void                Server::m_operCmd(Client& client)
{
    Message& message = client.getMessageQueue().front();

    std::vector<std::string>    arguments = message.getArgs();
    if (arguments.size() < 2)
    {
        m_reply(client.getFd(), Replies::ERR_NEEDMOREPARAMS, OPER_COMMAND);
        return ;
    }
    if (arguments[0] != client.getNickname())
    {
        m_reply(client.getFd(), Replies::ERR_NOOPERHOST, "");
        return ;
    }
    if (arguments[1] != m_operPassword)
    {
        m_reply(client.getFd(), Replies::ERR_PASSWDMISMATCH, "");
        return ;
    }
    if (!client.getModeValue(UserModes::oper))
    {
        client.turnOnMode(UserModes::oper);
        std::string modeChanges = "+o";
        if (client.getModeValue(UserModes::restricted))
        {
            client.turnOffMode(UserModes::restricted);
            modeChanges += " -r";
        }
        m_reply(client.getFd(), Replies::RPL_UMODEIS, modeChanges);
        m_reply(client.getFd(), Replies::RPL_YOUREOPER, "");
        ++m_numOps;
    }
    else
        m_reply(client.getFd(), Replies::ERR_NOOPERHOST, "");
}


int                 countChars(std::string str)
{
    int count = 0;

    for (int i = 0; i < static_cast<int>(str.size()); ++i)
    {
        if (str[i] == '*')
            ++count;
    }
    return (count);
}

bool                Server::m_isWildCardMask(std::string str) const
{
    return ((str[0] == '*' || str[str.size() - 1] == '*') && countChars(str) == 1);
}

bool                Server::m_onlyOps(std::vector<std::string> arguments)
{
    return (arguments.size() == 2 && arguments[1] == "o");
}

typedef void (Server::*cmdFun)(Client&);

void                Server::m_relay(int clientFd)
{
    Client& client = this->m_clients[clientFd];
    t_messageDQeue& messages = client.getMessageQueue();
    
    while (!messages.empty())
    {
        Message& message = messages.front();

        message.parse();
        
        std::string command = message.getCmd();

        if (command == USER_COMMAND || command == PASS_COMMAND)
            m_reply(clientFd, Replies::ERR_ALREADYREGISTRED, command);
        else if (this->m_isValidCommand(command))
        {
            cmdFun fp = m_cmdFuncs[command];
            (this->*fp)(m_clients[clientFd]);
        }
        else if (command.size())
            m_reply(clientFd, Replies::ERR_UNKNOWNCOMMAND, command);   
        if (!messages.empty())
            messages.pop_front();
    }
}

void                Server::m_manageClientEvent(int pollIndex)
{
    char    buffer[BUFFER_SIZE];
    int     bytesRead = recv(this->m_pfds[pollIndex].fd, buffer, BUFFER_SIZE - 1, 0);

    if (bytesRead <= 0)
    {
        if (bytesRead == 0)
            std::cout << "Client disconnected.\n";
        else
            std::perror("recv: ");

        int    badFd = this->m_pfds[pollIndex].fd;
        m_quitCmd(m_clients[badFd]);
    }
    else
    {
        buffer[bytesRead] = '\0';
        if (this->m_manageRecv(buffer, this->m_pfds[pollIndex].fd))
        {
            if (this->m_isAuthenticated(this->m_pfds[pollIndex].fd))
                this->m_relay(this->m_pfds[pollIndex].fd);
            else
            {
                this->m_tryAuthentificate(m_clients[this->m_pfds[pollIndex].fd]);
                if (this->m_isAuthenticated(this->m_pfds[pollIndex].fd))
                    this->m_relay(this->m_pfds[pollIndex].fd);
            }
        }
    }
}

bool Server::m_nickIsValid(std::string &str)
{
    
    return (!(std::find_if(str.begin(), str.end(), isAlphaNum) == str.end()));
}

void     Server::setServPassword(std::string password)
{
    if (password.empty())
    {
        std::cout << "Error: Password cannot be empty.\n";
        std::exit(-1);
    }
    m_password = password;
}

bool    Server::m_checkNickSyntax(Message& message)
{
    if (message.getArgs().empty())
    {
        return false;
    }
    std::string & nick = message.getArgs().front();
    int len = nick.length();
    
    if (len > 9 || len < 1 || nick == "anonymous" || !m_nickIsValid(nick))
    {
        return (false);
    }
    return (true);
}

bool    Server::m_checkStatusAuth(Client& client)
{
    if (client.isUserAuth() && client.isNickAuth() && client.isPassAuth())
    {
        m_reply(client.getFd(), Replies::RPL_WELCOME, client.getNickname());
        m_reply(client.getFd(), Replies::RPL_YOURHOST, client.getNickname() + " :Your host is " + m_serverName + ", running version " + m_version);
        m_reply(client.getFd(), Replies::RPL_CREATED, "");
        m_reply(client.getFd(), Replies::RPL_MYINFO, "");
        this->m_motdCmd(client);
        ++(this->m_authenticatedUserNum);
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
    while (!m_checkStatusAuth(client) && client.getMessageQueue().size())
    {
        Message& msg = client.getMessageQueue().front();

        msg.parse();

        std::string command = msg.getCmd();
        
        if (command != NICK_COMMAND && command != USER_COMMAND && command != PASS_COMMAND)
        {
            if (this->m_isValidCommand(command))
                m_reply(client.getFd(), Replies::ERR_NOTREGISTERED, "");
            if (!client.getMessageQueue().empty())
                client.getMessageQueue().pop_front();
            return (false);
        }
        if (command == USER_COMMAND)
            m_userCmd(client);
        else if (command == NICK_COMMAND)
            m_nickCmd(client);
        else if (command == PASS_COMMAND)
            m_passCmd(client);
        if (!client.getMessageQueue().empty())
            client.getMessageQueue().pop_front();
    }
    return (this->m_isAuthenticated(client.getFd()));
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

    while (i < this->m_pfds.size() && this->m_poll_count)
    {
        if (this->m_pfds[i].revents & POLLIN)
        {
            if (this->m_pfds[i].fd == this->m_sockfd)
            {
                if (this->m_manageServerEvent())
                {
                    this->m_poll_count--;
                    continue ;
                }

            }
            else
                this->m_manageClientEvent(i);
            this->m_poll_count--;
        }
        i++;
    }
}

void    Server::m_debugAuthentificate(int clientFd)
{
    this->m_send(clientFd, ':' + this->m_serverName + " 001 ohachim :Welcome\r\n");
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
    int size = message.size();
    int total = 0;

    while (size)
    {
        bytesSent = send(toFd, message.data() + total, size, 0);
        if (bytesSent == -1)
            return (-1);
        total += bytesSent;
        size -= bytesSent;
    }
    return (0);
}

int	Server::m_manageRecv(std::string message, int clientFd)
{
    t_messageDQeue& messageQueue = this->m_clients[clientFd].getMessageQueue();
    std::string token = strToken(message, END_STRING);

    while (token.size())
    {
        if (messageQueue.empty() || messageQueue.back().getMsg().find(END_STRING)
                                                        != std::string::npos)
            messageQueue.push_back(Message(token));
        else if (messageQueue.back().getMsg().find(END_STRING) == std::string::npos)
            messageQueue.back().setMsg(messageQueue.back().getMsg() + token);
        token = strToken("", END_STRING);
    }
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

void    Server::m_userhostCmd(Client & client)
{
    Message & msg = client.getMessageQueue().front();
    
    std::vector<std::string>::iterator it = msg.getArgs().begin();
    std::vector<std::string>::iterator end = msg.getArgs().end();
    int count = 0;

    std::vector<std::string> arguments = msg.getArgs();
    if (arguments.empty())
    {
        m_reply(client.getFd(), Replies::ERR_NEEDMOREPARAMS, USERHOST_COMMAND);
        return ;
    }
    while (it < end && count < 5)
    {
        if (m_nicknames.find(*it) != m_nicknames.end())
            m_reply(client.getFd(), Replies::RPL_USERHOST, client.getNickname() + " :"\
            + m_clients[m_nicknames[*it]].getNickname() + ((m_clients[m_nicknames[*it]].getModeValue(UserModes::oper)) ? "*" : "") + "=" \
            + ((m_clients[m_nicknames[*it]].getModeValue(UserModes::away)) ? "+" : "-") + "~" + m_clients[m_nicknames[*it]].getUsername()
            + "@" + m_clients[m_nicknames[*it]].getHostname());
        it++;
        count += 1;
    }
}

bool    Server::m_isOnServer(std::string nickname)
{
     return (m_nicknames.find(nickname) != m_nicknames.end());
}

void    Server::m_isonCmd(Client & client)
{
    Message & msg = client.getMessageQueue().front();

    std::vector<std::string> arguments = msg.getArgs();

    std::string isOnServer = "";

    for (int i = 0; i < static_cast<int>(arguments.size()); ++i)
    {
        if (m_isOnServer(arguments[i]))
            isOnServer += arguments[i] + ' ';
    }
    if (!(isOnServer.empty()))
        isOnServer.erase(isOnServer.size() - 1, 1);
    
    m_reply(client.getFd(), Replies::RPL_ISON, isOnServer);
}

void    Server::m_nickCmd(Client & client)
{
    Message& msg = client.getMessageQueue().front();

    if (msg.getArgs().empty())
        m_reply(client.getFd(), Replies::ERR_NONICKNAMEGIVEN,"");
    else if (!m_checkNickSyntax(msg))
        m_reply(client.getFd(), Replies::ERR_ERRONEUSNICKNAME, msg.getArgs().front());
    else 
    {
        std::pair<std::map<std::string, int>::iterator , bool> nick =\
        m_nicknames.insert(std::make_pair(msg.getArgs().front(), client.getFd()));
        if (nick.second)
        {
            if (client.isAuthComplete())
                m_nicknames.erase(client.getNickname());
            client.setNickname(msg.getArgs().front());
            client.setNickAuth();
        }
        else
            m_reply(client.getFd(), Replies::ERR_NICKNAMEINUSE, "");
    }
}

void    Server::m_userCmd(Client & client)
{
    Message&    msg = client.getMessageQueue().front();
    std::string mode;
    
    if (client.isUserAuth())
        m_reply(client.getFd(), Replies::ERR_ALREADYREGISTRED, msg.getCmd());
    else if (msg.getArgs().size() < 3)
        m_reply(client.getFd(), Replies::ERR_NEEDMOREPARAMS, USER_COMMAND);
    else
    {
        client.setUsername(msg.getArgs()[0]);
        mode = msg.getArgs()[1];
        client.setRealname(msg.getLiteralMsg());
        client.setUserAuth();
    }
}

void    Server::m_passCmd(Client &client)
{
    Message &msg = client.getMessageQueue().front();
    std::vector<std::string> &args = msg.getArgs();
    
    if  (args.empty())
        m_reply(client.getFd(), Replies::ERR_NEEDMOREPARAMS, PASS_COMMAND);
    else if (client.isPassAuth())
        m_reply(client.getFd(), Replies::ERR_ALREADYREGISTRED, msg.getCmd());
    else
    {
        if (args.front() == m_password)
        {
            client.setPassAuth();
        }
    }
}

std::string             Server::m_constructMask(Client& client)
{
    return (':' + client.getNickname() + "!~" + client.getUsername() + '@' + client.getHostname());
}

void                    Server::m_quitCmd(Client& client) 
{
    if (client.isAuthComplete())
    {
        std::string literalMsg = "";
        if (!client.getMessageQueue().empty() && client.getMessageQueue().front().getLiteralMsg().size())
            literalMsg = client.getMessageQueue().front().getLiteralMsg().erase(0, 1);
        std::string messageToSend = "ERROR: Closing Link: " + client.getUsername() + "(" 
                                    + literalMsg + ") " + client.getHostname() + " (Quit: "
                                        + client.getNickname() + ")" + END_STRING;
        if (this->m_send(client.getFd(), messageToSend) < 0)
            std::cout << "Error: send\n";
        std::vector<std::string>::iterator it = client.getChannels().begin();
        std::vector<std::string>::iterator end = client.getChannels().end();
        while (it != end)
        {
            m_p_privMsgCmd_noticeCmd(client, Message(" QUIT :Quit: "  + client.getNickname()), *it);
            m_channels[*it].removeMember(client.getFd());
            m_channels[*it].removeOp(client.getFd());
            m_channels[*it].removeInvited(client.getFd());
            if (!m_channels[*it].getMembers().size())
                m_channels.erase(*it);
            it++;
        }
        m_p_privMsgCmd_noticeCmd(client, Message(" QUIT :Quit: "  + client.getNickname()), client.getNickname());
        --m_authenticatedUserNum;
    }
    int    badFd = client.getFd();

    m_clients[badFd].deleteFiles();
    if (m_clients[badFd].getModeValue(UserModes::oper))
        --m_numOps;
    this->m_nicknames.erase(m_clients[badFd].getNickname());
    m_clients.erase(badFd);
    this->m_eraseClientPoll(client.getFd());
    close(badFd);
};

bool                    Server::m_grabChannelsNames(Message & msg, std::vector<std::string> & chans, std::vector<std::string> & passes)
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
    while (it != end)
    {
        passes.push_back(*it);
        it++;
    }
    if (chans.empty())
        return (false);
    // printVector(chans, "channels:");
    // printVector(passes, "passes/nicknames:");
    return (true);
}

bool                    Server::m_channelExists(std::string channelName)
{
    if (m_channels.find(channelName) != m_channels.end())
        return (true); 
    return (false);
}

void                    Server::m_addClientToChan(int clientFd, std::string channelName, std::string password, bool passProtected)
{
    Channel & chan = m_channels[channelName];;
    if (chan.isInMaskVector(m_clients[clientFd], chan.getBanMasks())
            && !chan.isInMaskVector(m_clients[clientFd], chan.getExceptionBanMasks()))
        m_reply(clientFd, Replies::ERR_BANNEDFROMCHAN, channelName);
    else if (m_channels[channelName].getModeValue(ChannelModes::i_inviteOnly)
            && !chan.isInMaskVector(m_clients[clientFd], chan.getInviteMasks()) && !m_channels[channelName].isInvited(clientFd))
        m_reply(clientFd, Replies::ERR_INVITEONLYCHAN, channelName);
    else if (((passProtected && !chan.checkPassword(password))
            || (!passProtected && !chan.getPassword().empty())))
        m_reply(clientFd, Replies::ERR_BADCHANNELKEY, channelName);
    else if (chan.isMember(clientFd))
        m_reply(clientFd, Replies::ERR_ALREADYREGISTRED, JOIN_COMMAND);
    else
    {
        if (static_cast<int>(chan.getMembers().size()) >= chan.m_getUserLimit() && chan.m_getUserLimit() >= 0)
        {
            m_reply(clientFd, Replies::ERR_CHANUSERLIMIT, channelName);
            return;
        }
        if (!password.empty())
            m_channels[channelName].turnOnMode(ChannelModes::k_passProtected);
        if (m_channels[channelName].getModeValue(ChannelModes::i_inviteOnly))
            m_channels[channelName].removeInvited(clientFd);
        chan.addMember(clientFd);
        m_clients[clientFd].pushChannel(channelName, PEASEANT_MODES);
        m_reply(clientFd, Replies::RPL_CHANNELMODEIS, channelName + ' ' +  m_composeChannelModes(channelName));
        m_reply(clientFd, Replies::RPL_TOPIC, m_composeRplTopic(m_channels[channelName]));
        m_p_privMsgCmd_noticeCmd(m_clients[clientFd], Message("JOIN :" + channelName), channelName);
        m_p_privMsgCmd_noticeCmd(m_clients[clientFd], Message("JOIN :" + channelName), m_clients[clientFd].getNickname());
        m_p_namesCmd_listCmd(m_clients[clientFd], channelName, NAMES_COMMAND);
    }
}

void                    Server::m_addChannel(int clientFd, std::string channelName, std::string password)
{
    Channel newChannel(PEASEANT_MODES, clientFd, channelName, channelName.at(0), password);
    newChannel.addMember(clientFd);
    if (!password.empty())
        newChannel.turnOnMode(ChannelModes::k_passProtected);
    newChannel.setCreatorNick(m_clients[clientFd].getNickname());
    m_channels.insert(m_channels.end(), std::pair<std::string, Channel>(channelName, newChannel));
    m_clients[clientFd].pushChannel(channelName, OWNER_MODES);
    m_clients[clientFd].turnOnMode(ChannelModes::O_Creator, channelName);
    m_clients[clientFd].turnOnMode(ChannelModes::o_OperatorPrivilege, channelName);
    m_reply(clientFd, Replies::RPL_CHANNELMODEIS, channelName + ' ' + m_composeChannelModes(channelName));
    m_p_privMsgCmd_noticeCmd(m_clients[clientFd], Message("JOIN :" + channelName), m_clients[clientFd].getNickname());
    m_p_privMsgCmd_noticeCmd(m_clients[clientFd], Message(":" + channelName + " supported commands :INVITE, PART, KICK, MODE, NAMES, LIST "),  m_clients[clientFd].getNickname());
    m_p_namesCmd_listCmd(m_clients[clientFd], channelName, NAMES_COMMAND);
}

void                    Server::m_joinCmd(Client & client)
{
    std::vector<std::string>    chans;
    std::vector<std::string>    passes;
    Message& msg = client.getMessageQueue().front();
    bool                        passProtected = true;
    
    if (msg.getArgs().empty())
        m_reply(client.getFd(), Replies::ERR_NEEDMOREPARAMS, JOIN_COMMAND);
    else if (msg.getArgs().size() == 1 && msg.getArgs().front() == "0")
        m_partZero(client);
    else
    {
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
                m_addClientToChan(client.getFd(), *it_chan, (passProtected)? *it_passes : "", passProtected);
            else
                m_addChannel(client.getFd(), *it_chan, (passProtected)? *it_passes : "");
            if (it_passes != end_passes)
                it_passes++;
            it_chan++;
        }
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
    if (msg.getArgs().size() >= 1 && msg.getArgs().front() == std::string("0"))
    {
        m_partZero(client);
        return ;
    }
    std::vector<std::string> channelNames;
    std::vector<std::string> &args = msg.getArgs();
    if (args.empty())
    {
        m_reply(client.getFd(), Replies::ERR_NEEDMOREPARAMS, "");
        return;
    }
    if (!m_grabChannelsNames(msg, channelNames))
    {
        m_reply(client.getFd(), Replies::ERR_NEEDMOREPARAMS, PART_COMMAND);
        return ;
    }
    std::vector<std::string>::iterator it_chan = channelNames.begin();
    std::vector<std::string>::iterator end_chan = channelNames.end();
    std::string partMsg = msg.getLiteralMsg();
    while (it_chan != end_chan)
    {
        if (!m_channelExists(*it_chan))
        {
            m_reply(client.getFd(), Replies::ERR_NOSUCHCHANNEL, *it_chan);
            it_chan++;
            continue ;
        }
        else if (!m_channels[*it_chan].isMember(client.getFd()))
        {
            m_reply(client.getFd(), Replies::ERR_NOTONCHANNEL, "");
            it_chan++;
            continue ;
        }
        m_channels[*it_chan].removeMember(client.getFd());
        m_channels[*it_chan].removeOp(client.getFd());
        client.partChannel(*it_chan);
        if (!m_channels[*it_chan].getModeValue(ChannelModes::q_quiet))
            m_p_privMsgCmd_noticeCmd(client, Message("PART " + *it_chan + " " + partMsg), *it_chan);
        m_p_privMsgCmd_noticeCmd(client, Message("PART " + *it_chan + " " + partMsg), client.getNickname());
        if (!m_channels[*it_chan].getMembers().size())
            m_channels.erase(*it_chan);
        it_chan++;
    }
}

void                        Server::m_partZero(Client &client)
{
    Message & msg = client.getMessageQueue().front();
    std::string partMsg = msg.getLiteralMsg();
    std::vector<std::string> & channelNames = client.getChannels();
    std::vector<std::string>::iterator it_chan = channelNames.begin();
    std::vector<std::string>::iterator end_chan = channelNames.end();
    while (it_chan != end_chan)
    {
        m_channels[*it_chan].removeMember(client.getFd());
        m_channels[*it_chan].removeOp(client.getFd());
        m_p_privMsgCmd_noticeCmd(client, Message("PART " + *it_chan + " " + partMsg), *it_chan);
        m_p_privMsgCmd_noticeCmd(client, Message("PART " + *it_chan + " " + partMsg), client.getNickname());
        if (!m_channels[*it_chan].getMembers().size())
            m_channels.erase(*it_chan);
        it_chan++;
    }
    channelNames.clear();
}

std::vector<int>        Server::m_grabClientsWithMask(std::string mask)
{
    std::vector<int>    members;
    if (!m_isMask(mask))
        return (members);
    std::string TLD = m_getTLD(mask);
    std::map<int, Client>::iterator it = m_clients.begin();
    std::map<int, Client>::iterator end = m_clients.end();
    while (it != end)
    {
        if (m_isMaskUserMatch(it->second.getHostname(), TLD))
            members.push_back(it->second.getFd());
        it++;
    }
    return (members);
}

void                    Server::m_privMsgCmd_noticeCmd(Client &client)
{
    Message& msg = client.getMessageQueue().front();
    bool    isAnonymous = false;
    bool    notifs = (msg.getCmd() == PRIVMSG_COMMAND) ? true : false;
    if (msg.getArgs().empty() || msg.getLiteralMsg().empty() || msg.getArgs().size() != 1)
    {
        if (notifs && msg.getArgs().empty())
            m_reply(client.getFd(), Replies::ERR_NORECIPIENT, "");
        else if (notifs && msg.getLiteralMsg().empty())
            m_reply(client.getFd(), Replies::ERR_NOTEXTTOSEND, "");
        else if (notifs && msg.getArgs().size() != 1)
            m_reply(client.getFd(), Replies::ERR_TOOMANYTARGETS, "");
        return ;
    }
    std::string target = msg.getArgs().front();
    if (target.at(0) == LOCAL_CHAN || target.at(0) == NETWORKWIDE_CHAN)
    {
        std::map<std::string, Channel>::iterator it_chan = m_channels.find(target);
        if (it_chan == m_channels.end() && !m_isMask(target.erase(0, 1)))
        {
            if (notifs)
                m_reply(client.getFd(), Replies::ERR_NOSUCHNICK, target);
            return ;
        }
        else if ((it_chan != m_channels.end() && m_channels[target].getModeValue(ChannelModes::n_noOutsideMessage) && !m_channels[target].isMember(client.getFd() && !client.getModeValue(UserModes::oper)))
         || (it_chan != m_channels.end() && m_channels[target].getModeValue(ChannelModes::m_moderated) && !client.getModeValue(ChannelModes::v_voicePrivilege, target) && !m_channels[target].isOp(client.getFd()) && !client.getModeValue(UserModes::oper)))
        {
            m_reply(client.getFd(), Replies::ERR_CANNOTSENDTOCHAN, target);
            return ;
        }
        else if (it_chan != m_channels.end() && it_chan->second.getModeValue(ChannelModes::a_annonymous))
            isAnonymous = true;
        std::vector<int> mem;
        std::string mask = target;
        if (m_isMask(mask.erase(0, 1)))
            mem = m_grabClientsWithMask(mask.erase(0, 1));
        std::vector<int> &members = (m_isMask(mask.erase(0, 1))) ? mem : m_channels[target].getMembers();
        std::vector<int>::iterator it = members.begin();
        std::vector<int>::iterator end = members.end();
        while (it != end)
        {
                if (m_clients[*it].getNickname() == client.getNickname())
                {
                    it++;
                    continue ;
                }
                if (m_clients[*it].getModeValue(UserModes::away))
                    m_send(client.getFd(), ":" + m_clients[*it].getNickname() + "!~" + m_clients[*it].getUsername() + "@" + m_clients[*it].getHostname() + " " + m_clients[*it].getAwayMsg());
                if (isAnonymous)
                    m_send(*it, ":anonymous!~anonymous@anonymous " + msg.getMsg());
                else
                    m_send(*it, ":" + client.getNickname() + "!~" + client.getUsername() + "@" + client.getHostname() + " " + msg.getMsg());
                it++;
        }
    }
    else
    {
        std::map<std::string, int>::iterator it_user = m_nicknames.find(target);
        if (it_user == m_nicknames.end())
        {
            if (notifs)
                m_reply(client.getFd(), Replies::ERR_NOSUCHNICK, target);
            return ;
        }
        int clientFd = m_nicknames[target];
        if (m_clients[clientFd].getModeValue(UserModes::away))
                m_send(client.getFd(), ":" + m_clients[clientFd].getNickname() + "!~" + m_clients[clientFd].getUsername() + "@" + m_clients[clientFd].getHostname() + " " + m_clients[clientFd].getAwayMsg());
        m_send(m_nicknames[target], ":" + client.getNickname() + "!~" + client.getUsername() + "@" + client.getHostname() + " " + msg.getMsg());
    }
}


void                    Server::m_p_privMsgCmd_noticeCmd(Client &client, Message msg, std::string target)
{
    msg.parse();
    if (target.at(0) == LOCAL_CHAN || target.at(0) == NETWORKWIDE_CHAN)
    {
        
        Channel & chan = m_channels[target];
        std::vector<int> &members = chan.getMembers();
        std::vector<int>::iterator it = members.begin();
        std::vector<int>::iterator end = members.end();

        while (it != end)
        {
            if (*it != client.getFd())
                m_send(*it, ':' + client.getNickname() + "!~" + client.getUsername() + "@" + client.getHostname() + " " + msg.getMsg() + END_STRING);
            it++;
        }
    }
    else
        m_send(m_nicknames[target], ':' + client.getNickname() + "!~" + client.getUsername() + "@" + client.getHostname() + " " + msg.getMsg() + END_STRING);
}

void            Server::m_kickCmd(Client &client)
{
    Message& msg = client.getMessageQueue().front();
    std::vector<std::string>    chans;
    std::vector<std::string>    nicks;
    m_grabChannelsNames(msg, chans, nicks);
    if (msg.getArgs().size() < 2 || (chans.size() != 1 && nicks.size() != chans.size()))
    {
        m_reply(client.getFd(), Replies::ERR_NEEDMOREPARAMS, KICK_COMMAND);
        return ;
    }
    std::vector<std::string>::iterator it_chan = chans.begin();
    std::vector<std::string>::iterator end_chan = chans.end();
    std::vector<std::string>::iterator it_nick = nicks.begin();
    std::vector<std::string>::iterator end_nick = nicks.end();
    std::string kickMsg = (msg.getLiteralMsg().empty()) ? ":" + client.getNickname() : msg.getLiteralMsg();
    if (chans.size() == 1)
    {
        if (m_channels[*it_chan].isOp(client.getFd()))
        {
            while (it_nick != end_nick)
            {
                // printVector(m_channels[*it_chan].getMembers(), "members");
                // printVector(m_channels[*it_chan].getOps(), "opers");
                m_p_privMsgCmd_noticeCmd(client, Message("KICK " + *it_nick + " " + kickMsg), *it_nick);
                m_channels[*it_chan].removeMember(m_nicknames[*it_nick]);
                m_channels[*it_chan].removeOp(m_nicknames[*it_nick]);
                m_clients[m_nicknames[*it_nick]].popChannel(*it_chan);
                it_nick++;
            }
        }
        else if (!m_channels[*it_chan].isMember(client.getFd()))
            m_reply(client.getFd(), Replies::ERR_NOTONCHANNEL, "");
        else if (!m_channels[*it_chan].isOp(client.getFd()) && !client.getModeValue(UserModes::oper))
            m_reply(client.getFd(), Replies::ERR_CHANOPRIVSNEEDED, "");
    }
    else
    {
        while (it_chan != end_chan)
        {
            if (m_channels[*it_chan].isOp(client.getFd()) || client.getModeValue(UserModes::oper))
            {
                m_channels[*it_chan].removeMember(m_nicknames[*it_nick]);
                m_channels[*it_chan].removeOp(m_nicknames[*it_nick]);
                m_clients[m_nicknames[*it_nick]].popChannel(*it_chan);
                m_p_privMsgCmd_noticeCmd(client, Message("KICK " + *it_nick + " " + kickMsg), *it_nick);
            }
            else if (!m_channels[*it_chan].isMember(client.getFd()))
                m_reply(client.getFd(), Replies::ERR_NOTONCHANNEL, "");
            else
                m_reply(client.getFd(), Replies::ERR_CHANOPRIVSNEEDED, "");
            it_nick++;
            it_chan++;
        }
    }
}

void            Server::m_inviteCmd(Client &client)
{
    Message& msg = client.getMessageQueue().front();
    std::vector<std::string> &args = msg.getArgs();
    if (args.size() != 2)
    {
        m_reply(client.getFd(), Replies::ERR_NEEDMOREPARAMS, INVITE_COMMAND);
        return;
    }
    std::string target = *args.begin();
    std::string chanName = *(args.begin() + 1);
    if (!m_channelExists(chanName))
        m_reply(client.getFd(), Replies::ERR_NOSUCHCHANNEL, chanName);
    else if (m_nicknames.find(target) == m_nicknames.end())
        m_reply(client.getFd(), Replies::ERR_NOSUCHNICK, target);
    else if (!m_channels[chanName].isMember(client.getFd()))
        m_reply(client.getFd(), Replies::ERR_NOTONCHANNEL, "");
    else if (!client.getModeValue(ChannelModes::O_Creator, chanName)  && !client.getModeValue(ChannelModes::o_OperatorPrivilege, chanName)  && !client.getModeValue(UserModes::oper) && m_channels[chanName].getModeValue(ChannelModes::i_inviteOnly))
        m_reply(client.getFd(), Replies::ERR_CHANOPRIVSNEEDED, "");
    else if (m_channels[chanName].isMember(m_nicknames[target]))
        m_reply(client.getFd(), Replies::ERR_USERONCHANNEL, target + ' ' + chanName);
    else
    {
        m_p_privMsgCmd_noticeCmd(client, Message("INVITE " + client.getNickname() + " :" + chanName), target);
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
        if (m_channels[channelName].getModeValue(ChannelModes::s_secret))
            message += "@ " + channelName + " :";
        else if (m_channels[channelName].getModeValue(ChannelModes::p_private))
            message += "* " + channelName + " :";
        else
            message += "= " + channelName + " :";
        std::vector<int> &members = m_channels[channelName].getMembers();
        std::vector<int>::iterator it = members.begin();
        std::vector<int>::iterator end = members.end();
        while (it != end)
        {
            if (m_channels[channelName].isOp(*it))
                message += "@";
            else if (m_clients[*it].getModeValue(ChannelModes::v_voicePrivilege, channelName))
                message += "+";
            message += m_clients[*it].getNickname() + " ";
            it++;
        }
    }
    return(message);
}

std::string Server::m_composeList(std::string channelName)
{
    std::string message = "";
    if (m_channelExists(channelName))
    {
        message += channelName;
        if (m_channels[channelName].getModeValue(ChannelModes::s_secret))
            message += " secret : ";
        else if (m_channels[channelName].getModeValue(ChannelModes::p_private))
            message += " private : ";
        else
            message += " public : ";
        message += (m_channels[channelName].getTopic().empty()) ? "NO TOPIC" : m_channels[channelName].getTopic();
    }
    return(message);
}

void    Server::m_namesCmd_listCmd(Client & client)
{
    Message& msg = client.getMessageQueue().front();
    std::string cmd = msg.getCmd();
    std::vector<std::string>    chans;
    m_grabChannelsNames(msg, chans);
    if (chans.empty())
        m_mapKeysToVector(chans, m_channels);

    std::vector<std::string>::iterator    it_chan = chans.begin();
    std::vector<std::string>::iterator    end_chan = chans.end();
    while (it_chan != end_chan)
    {
        if (cmd == NAMES_COMMAND && !m_channels[*it_chan].getModeValue(ChannelModes::p_private)\
        && !m_channels[*it_chan].getModeValue(ChannelModes::s_secret))
        {
            if (!m_channels[*it_chan].getModeValue(ChannelModes::a_annonymous))
                m_reply(client.getFd(), Replies::RPL_NAMREPLY, m_composeNames(*it_chan));
            else
                m_reply(client.getFd(), Replies::RPL_NAMREPLY, " anonymous channel");
            m_reply(client.getFd(), Replies::RPL_ENDOFNAMES, *it_chan);
        }
        else if (cmd == NAMES_COMMAND && (m_channels[*it_chan].getModeValue(ChannelModes::p_private)\
        || m_channels[*it_chan].getModeValue(ChannelModes::s_secret)))
            m_reply(client.getFd(), Replies::RPL_ENDOFNAMES, *it_chan);
        else if (cmd == LIST_COMMAND && !m_channels[*it_chan].getModeValue(ChannelModes::p_private)\
        && !m_channels[*it_chan].getModeValue(ChannelModes::s_secret))
        {
            if (!m_channels[*it_chan].getModeValue(ChannelModes::a_annonymous))
                m_reply(client.getFd(), Replies::RPL_LIST, m_composeList(*it_chan));
            else
                m_reply(client.getFd(), Replies::RPL_LIST, " anonymous channel");
            m_reply(client.getFd(), Replies::RPL_LISTEND, *it_chan);
        }
        else if (cmd == LIST_COMMAND && (m_channels[*it_chan].getModeValue(ChannelModes::p_private)\
        || m_channels[*it_chan].getModeValue(ChannelModes::s_secret)))
            m_reply(client.getFd(), Replies::RPL_LISTEND, *it_chan);
        it_chan++;
    }
    if (chans.empty())
    {
        if (msg.getCmd() == LIST_COMMAND)
            m_reply(client.getFd(), Replies::RPL_LISTEND, "");
        else if (msg.getCmd() == NAMES_COMMAND)
            m_reply(client.getFd(), Replies::RPL_ENDOFNAMES, "");
    }
}

void    Server::m_p_namesCmd_listCmd(Client & client, std::string target, std::string cmd)
{
    if (cmd == NAMES_COMMAND && !m_channels[target].getModeValue(ChannelModes::p_private)\
    && !m_channels[target].getModeValue(ChannelModes::s_secret))
    {
        if (!m_channels[target].getModeValue(ChannelModes::a_annonymous))
            m_reply(client.getFd(), Replies::RPL_NAMREPLY, m_composeNames(target));
        else
            m_reply(client.getFd(), Replies::RPL_NAMREPLY, " anonymous channel ");
        m_reply(client.getFd(), Replies::RPL_ENDOFNAMES, target);
    }
    else if (cmd == LIST_COMMAND && !m_channels[target].getModeValue(ChannelModes::p_private)\
    && !m_channels[target].getModeValue(ChannelModes::s_secret))
    {
        if (!m_channels[target].getModeValue(ChannelModes::a_annonymous))
            m_reply(client.getFd(), Replies::RPL_LIST, m_composeList(target));
        else
            m_reply(client.getFd(), Replies::RPL_LIST, " anonymous channel ");
        m_reply(client.getFd(), Replies::RPL_LISTEND, target);
    }
}

bool            Server::m_validArgsFtp(Message &msg, t_fileData &fileData)
{
    std::vector<std::string>::iterator it = msg.getArgs().begin();
    std::string target = *it;
    std::string fileName = *(++it);
    std::string fileLength = *(++it);
    unsigned long length;

    try
    {
        length = std::atoi(fileLength.c_str());
    }
    catch (std::exception)
    {
        return (false);
    };
    fileData.reciever = target;
    fileData.name = fileName;
    fileData.length = length;
    return (true);
}

void            Server::m_sendCmd(Client &client)
{
    Message& msg = client.getMessageQueue().front();
    t_fileData fileData;
    fileData.content = NULL;
    fileData.sender = client.getNickname();
    if (msg.getArgs().empty() || msg.getArgs().size() < 3)
    {
        m_reply(client.getFd(), Replies::ERR_NEEDMOREPARAMS, SEND_COMMAND);
    }
    else if (m_nicknames.find(msg.getArgs().front()) == m_nicknames.end())
        m_reply(client.getFd(), Replies::ERR_NOSUCHNICK, msg.getArgs().front());
    else if (m_validArgsFtp(msg, fileData))
    {
        unsigned long readBytes = 0;
        int rec = 0;
        char *buffer = (char*)malloc(sizeof(char) * fileData.length + 1);
        m_reply(client.getFd(), Replies::RPL_READYTORECIEVE, "");
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        while (readBytes < fileData.length)
        {
            rec = recv(client.getFd(), buffer + readBytes, fileData.length - readBytes, 0);
            if (rec > 0)
                readBytes += rec;
            else if (std::chrono::steady_clock::now() - start >= std::chrono::seconds(10))
                return ;
            else if (rec <= 0)
                continue ;
            if ((!readBytes && std::chrono::steady_clock::now() - start >= std::chrono::seconds(1)) || int(std::chrono::steady_clock::now() - start >= std::chrono::seconds(10)))
            {
                m_reply(client.getFd(), Replies::ERR_FTPTIMEOUT, "");
                return ;
            }
        }
        FILE *dest = fopen("ftp_text", "wb");
        int fd = fileno(dest);
        unsigned long i = 0;

        write (fd, buffer, fileData.length);
        fileData.content = (char*)malloc(sizeof(char) * (fileData.length));
        while (i < fileData.length)
        {
            fileData.content[i] = buffer[i];
            i++;
        }
        free(buffer);
        buffer = NULL;
        m_clients[m_nicknames[fileData.reciever]].pushFile(fileData);
        m_reply(m_nicknames[fileData.reciever], Replies::RPL_FILERECIEVED, fileData.name);
        m_reply(client.getFd(), Replies::RPL_FILESENT, fileData.name + " to " + fileData.reciever);
    }
}

void            Server::m_fetchCmd(Client &client)
{
    if (client.getFiles().empty())
        m_reply(client.getFd(), Replies::ERR_RECIEVEDNOFILES, "");
    else
    {
        t_fileData fileData = client.getFiles().front();
        unsigned long sentBytes = 0;
        m_reply(client.getFd(), Replies::RPL_READYTOSEND, std::to_string(fileData.length));
        
        FILE *dest = fopen("ftp_text_fetch", "wb");
        int fd = fileno(dest);

        write (fd, fileData.content, fileData.length);
        usleep(100);
        while (sentBytes < fileData.length)
        {
            int hold = send(client.getFd(), fileData.content + sentBytes, fileData.length - sentBytes, 0);
            if (hold == -1)
                continue;
            sentBytes += hold;
        }
        free(fileData.content);
        m_clients[m_nicknames[fileData.reciever]].getFiles().erase(m_clients[m_nicknames[fileData.reciever]].getFiles().begin());
    }
}

std::string     Server::m_possibleCommands[NUM_COMMANDS] = {"USER", "NICK", "PASS", "USERHOST",
                                                            "QUIT", "ISON", "MODE", "PONG",
                                                            "PING", "MOTD", "AWAY", "LUSERS",
                                                            "WHOIS", "TOPIC", "JOIN", "PART",
                                                            "KICK", "PRIVMSG", "NOTICE", "OPER",
                                                            "NAMES", "SEND", "FETCH", "INVITE",
                                                            "LIST"};
