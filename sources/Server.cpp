/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ohachim <ohachim@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/11 16:40:51 by ohachim           #+#    #+#             */
/*   Updated: 2022/02/25 17:06:46 by ohachim          ###   ########.fr       */
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
    m_numOps = 0;
    memset(&(this->m_socketInfo), 0, sizeof(this->m_socketInfo));
    memset(&(this->m_hints), 0, sizeof(this->m_hints));
    memset(&(this->m_addr_in), 0, sizeof(this->m_addr_in));
    memset(&(this->m_addr_in6), 0, sizeof(this->m_addr_in6));
}

void    Server::initializeCmdFuncs(void)
{
    this->m_cmdFuncs.insert(std::make_pair(QUIT_COMMAND, &Server::m_quitCmd));
    this->m_cmdFuncs.insert(std::make_pair(KICK_COMMAND, &Server::m_kickCmd));
    this->m_cmdFuncs.insert(std::make_pair(INVITE_COMMAND, &Server::m_inviteCmd));
    this->m_cmdFuncs.insert(std::make_pair(MODE_COMMAND, &Server::m_modeCmd));
    this->m_cmdFuncs.insert(std::make_pair(PING_COMMAND, &Server::m_pingCmd));
    this->m_cmdFuncs.insert(std::make_pair(PONG_COMMAND, &Server::m_pongCmd));
    this->m_cmdFuncs.insert(std::make_pair(MOTD_COMMAND, &Server::m_motdCmd));
    this->m_cmdFuncs.insert(std::make_pair(AWAY_COMMAND, &Server::m_awayCmd));
    this->m_cmdFuncs.insert(std::make_pair(LUSERS_COMMAND, &Server::m_lusersCmd));
    this->m_cmdFuncs.insert(std::make_pair(WHOIS_COMMAND, &Server::m_whoisCmd));
    this->m_cmdFuncs.insert(std::make_pair(TOPIC_COMMAND, &Server::m_topicCmd));
    this->m_cmdFuncs.insert(std::make_pair(OPER_COMMAND, &Server::m_operCmd));
    this->m_cmdFuncs.insert(std::make_pair(JOIN_COMMAND, &Server::m_joinCmd));
    this->m_cmdFuncs.insert(std::make_pair(USERHOST_COMMAND, &Server::m_userhostCmd));
    this->m_cmdFuncs.insert(std::make_pair(NAMES_COMMAND, &Server::m_namesCmd_listCmd));
    this->m_cmdFuncs.insert(std::make_pair(LIST_COMMAND, &Server::m_namesCmd_listCmd));
    this->m_cmdFuncs.insert(std::make_pair(PRIVMSG_COMMAND, &Server::m_privMsgCmd_noticeCmd));
    this->m_cmdFuncs.insert(std::make_pair(NOTICE_COMMAND, &Server::m_privMsgCmd_noticeCmd));
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
    m_numOps = 0;
    
    try
    {
        std::stoi(port);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        std::exit(1);
    }
    
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
        perror("socket");
        return (-1);
    }
    // allowing a port to be reused, unless there is a socket already listening to the port
    if (setsockopt(this->m_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)))
    {
        perror("setsocketopt");
        // is freeing necessary before exit()?
        if (this->m_servinfo)
            freeaddrinfo(this->m_servinfo);
        exit(1);
    }
    if ((bind(this->m_sockfd, addr, addrlen)))
    {
        perror("bind");
        return (-2);
    }
    fcntl(this->m_sockfd, F_SETFL, O_NONBLOCK);
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
        std::exit(1);
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
        fcntl(newFd, F_SETFL, O_NONBLOCK);
        this->m_clients[newFd] = Client(newFd, remoteAddr, addrlen);
        this->m_clients[newFd].setHostname(this->convertToHostname(remoteAddr, newFd));
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

int     findMode(char c) // ADD IS AS SERVER OR CLIENT FUNCTION
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

int            Server::m_manageChannelModes(char mode, char prefix, std::vector<std::string> arguments)
{
     // template for maps?
    if (m_nicknames.find(arguments[2]) == m_nicknames.end())
            return (-1);

    Client& client = m_clients[m_nicknames[arguments[2]]];

    if (prefix == '+')
        client.turnOnMode(client.findMode(mode), arguments[0]);
    else if (prefix == '-')
        client.turnOffMode(client.findMode(mode), arguments[0]);
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



std::vector<std::string>            Server::m_manageMaskMode(char mode, char prefix, std::vector<std::string> arguments, int& paramToUseIndex)
{
    Channel& channel = m_channels[arguments[0]];

    if (prefix == '+')
    {
        if (mode =='e')
            channel.m_addToChanVector(channel.getExceptionBanMasks(), arguments[paramToUseIndex]);
        else if (mode == 'b')
            channel.m_addToChanVector(channel.getBanMasks(), arguments[paramToUseIndex]);
        else if (mode == 'I')
            channel.m_addToChanVector(channel.getInviteMasks(), arguments[paramToUseIndex]);
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
    else
    {
        std::vector<std::string> maskList;
        
        if (mode == 'e')
            maskList = channel.getExceptionBanMasks();
        else if (mode == 'b')
            maskList = channel.getBanMasks();
        else if (mode == 'I')
            maskList = channel.getInviteMasks();

       return (maskList);
    }
    return (std::vector<std::string>());
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
    for (int i = 0; i < maskList.size(); ++i)
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

void            Server::m_executeModes(std::vector<std::string> arguments, Channel& channel, Client& client)
{
    std::string modes = arguments[1];
    char        prefix = '\0';
    int         paramToUseIndex = 2;
    int         start = 0;


    // check mode validity
    if (modes[0] == '+' || modes[0] == '-')
    {
        prefix = modes[0];
        start = 1;
    }
    for (int i = start; i < modes.size(); ++i)
    {
        if (m_isAttributeSetterMode(modes[i]))
        {
            if (channel.manageAttribute(modes[i], prefix, arguments))
                m_reply(client.getFd(), Replies::ERR_INTNEEDED, arguments[2]);
        }
        else if (m_isSimpleChannelMode(modes[i]))
            channel.manageSimpleMode(modes[i], prefix);
        else if (m_isUserSpecificChannelMode(modes[i]) && modes[i] != 'O')
        {
            if (m_manageChannelModes(modes[i], prefix, arguments))
            {
                m_reply(client.getFd(), Replies::ERR_USERNOTINCHANNEL,
                                m_composeUserNotInChannel(channel.getName(), client.getNickname()));
            }
        }
        else if (m_isMaskMode(modes[i]))
        {
            std::vector<std::string> maskList = m_manageMaskMode(modes[i], prefix, arguments, paramToUseIndex);
            if (prefix == '\0')
                m_listMasks(maskList, modes[i], client, channel);
        }
        else if (modes[i] == 'O')
            m_reply(client.getFd(), Replies::RPL_UNIQOPIS, channel.getName()
                            + ' ' + channel.getCreatorName());
        else
            m_reply(client.getFd(), Replies::ERR_UMODEUNKNOWNFLAG, "");
    }
}

bool            Server::m_areModesMasks(std::string modes)
{
    for (int i = 0; i < modes.size(); ++i)
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
    for (int i = start; i < modes.size(); ++i)
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
        if (attributeSetters > 1) // -kl bruh
            return (false);
        if (prefix == '+' && maskSetters && maskSetters != argNum)
            return (false);
        if (prefix == '-' && maskSetters && argNum) // untestd
            return (false);
        if (prefix == '-' && attributeSetters && arguments[1][0] == 'l' && argNum) // prefix relative
            return (false);
        if (attributeSetters && argNum != 1) // prefix relative
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

//TODO: CHANGE MODES IN REPLY
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
        m_reply(client.getFd(), Replies::RPL_CHANNELMODEIS, m_composeChannelModes(channelName));
        return ;
    }  // WHAT IF THE CLIENT IS NOT THE CHANNEL OPERATOR
    if (!m_isClientOper(client, arguments[0]) && !client.getModeValue(UserModes::oper)) 
    {
        m_reply(client.getFd(), Replies::ERR_CHANOPRIVSNEEDED, channelName); // maybe change arguments[1] to channelName
        return ;
    }
    if (arguments.size() >= 2)// MODE CHAN_NAME MODES WHO
    {
        if (!m_isModesSyntaxValid(arguments))
        {
            m_reply(client.getFd(), Replies::ERR_WRONGCHANMODESYNTAX, channelName); // maybe change arguments[1] to channelName
            return ;
        }
        m_executeModes(arguments, m_channels[channelName], client);
    }
}

void            Server::m_userModeCmd(Client& client, Message& message) // TODO: REFACTOR
{
    if (message.getArgs()[0] != client.getNickname())
    {
        m_reply(client.getFd(), Replies::ERR_USERSDONTMATCH, "");
        return ;
    }

    std::string modeChanges;
    char prefix = message.getArgs()[1][0];

    modeChanges += prefix;
    for (int j = 1; j < int(message.getArgs()[1].size()); ++j)
    {
        int modeNum = findMode(message.getArgs()[1][j]);

        if (modeNum == UserModes::away) // Ignore it for now, don't know the exact behaviour
            continue ;
        if (modeNum == -1)
        {
            m_reply(client.getFd(), Replies::ERR_UMODEUNKNOWNFLAG, "");
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
        m_reply(client.getFd(), Replies::RPL_UMODEIS, modeChanges);
}

void            Server::m_modeCmd(Client& client) // TODO: CHANGE THE MAGIC NUMBERS
{
    Message& message = client.getMessageQueue().front();

    if (m_isChannelPrefix(message.getArgs()[0][0]))
        m_channelModeCmd(client, message);
    else if (message.getArgs().size() < 2)
    {
        m_reply(client.getFd(), Replies::ERR_NEEDMOREPARAMS, ""); // NOT SURE ABOUT THIS
        return ;
    }
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
            motd += ':' + this->m_serverName + " 372 " + clientNick + " :-\t  " + line;
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

void                Server::m_awayCmd(Client& client) // TODO: SHOULD WORK TOGETHER WITH PRIVMSG to send messgae in channels the user is in?
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
        client.turnOffMode(UserModes::away);
        m_reply(client.getFd(), Replies::RPL_UNAWAY, ""); // what if the user was never away
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
// TODO: WE SHOULD BE ABLE TO USE NICK AGAIN
std::string                 Server::m_makeReplyHeader(int replyNum, std::string nickname)
{
    std::string strReplyNum = std::to_string(replyNum);
    int         len = strReplyNum.size();
    std::string s = std::string(3 - len, '0');
    std::string header = ':' + this->m_serverName + ' ' + s + strReplyNum + ' ' + nickname;
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
        case Replies::RPL_AWAY:
            return (m_makeReplyHeader(Replies::RPL_AWAY, clientNickname) + ' ' + queryClient.getNickname() + ' ' + queryClient.getAwayMsg());
        case Replies::RPL_WHOISOPERATOR:
            if (queryClient.getModeValue(UserModes::oper))
                return (m_makeReplyHeader(Replies::RPL_WHOISOPERATOR, clientNickname) + ' ' + queryClient.getNickname() + " :is an IRC operator");
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
    m_reply(client.getFd(), Replies::RPL_PINGREQUEST,"");
}

void                Server::m_whoisCmd(Client& client)
{
    Message&    message = client.getMessageQueue().front();
    if (message.getArgs().empty())
    {
        m_reply(client.getFd(), Replies::ERR_NEEDMOREPARAMS,""); // MIGHT remove
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

    m_reply(client.getFd(), Replies::RPL_WHOISUSER, m_composeWhoisQuery(queryClient, client.getNickname(), Replies::RPL_WHOISUSER));
    m_reply(client.getFd(), Replies::RPL_WHOISSERVER, m_composeWhoisQuery(queryClient, client.getNickname(), Replies::RPL_WHOISSERVER));
    if (queryClient.getModeValue(UserModes::oper))
        m_reply(client.getFd(), Replies::RPL_WHOISOPERATOR, m_composeWhoisQuery(queryClient, client.getNickname(), Replies::RPL_WHOISOPERATOR));
    if (!queryClient.getAwayMsg().empty())
        m_reply(client.getFd(), Replies::RPL_AWAY, m_composeWhoisQuery(queryClient, client.getNickname(), Replies::RPL_AWAY));
    m_reply(client.getFd(), Replies::RPL_ENDOFWHOIS, m_composeWhoisQuery(queryClient, client.getNickname(), Replies::RPL_ENDOFWHOIS));

}

std::string         Server::m_composeRplTopic(Channel& channel)
{
    return (channel.getName() + " :" + channel.getTopic());
}

void                Server::m_topicCmd(Client& client)
{
    Message& message = client.getMessageQueue().front();

    std::vector<std::string> arguments = message.getArgs();
    if (arguments.empty())
    {
        m_reply(client.getFd(), Replies::ERR_NEEDMOREPARAMS,"");
        return ;
    }
    std::vector<std::string> &channels = client.getChannels();
    if (std::find(channels.begin(), channels.end(), arguments[0]) == channels.end()) // TODO: wrong reply?
    {
        m_reply(client.getFd(), Replies::ERR_NOTONCHANNEL, arguments[0]);
        return ;
    }

    if (arguments.size() == 1) // there is only one argument, channel name
    {
        if (m_channels[arguments[0]].getTopic().empty()) // checking if topic is empty
        {
            m_reply(client.getFd(), Replies::RPL_NOTOPIC,  arguments[0]);
            return;
        }
        m_reply(client.getFd(), Replies::RPL_TOPIC, m_composeRplTopic(m_channels[arguments[0]])); // there is a topic
        return;
    }
    if (arguments.size() == 2)
    {
        Channel& channel = m_channels[arguments[0]];

        if (channel.getModeValue(ChannelModes::t_topicOperatorOnly))
        {
            if (client.getModeValue(ChannelModes::o_OperatorPrivilege, arguments[0])
                    || client.getModeValue(ChannelModes::O_Creator, arguments[0]))
                channel.getTopic() = arguments[1];
            else
                m_reply(client.getFd(), Replies::ERR_CHANOPRIVSNEEDED, arguments[0]);
        }
        else
            channel.getTopic() = arguments[1];
    }    
}

void                Server::m_operCmd(Client& client)
{
    Message& message = client.getMessageQueue().front();

    std::vector<std::string>    arguments = message.getArgs();
    if (arguments.size() < 2)
    {
        m_reply(client.getFd(), Replies::ERR_NEEDMOREPARAMS, "");
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
    client.turnOnMode(UserModes::oper);
    m_reply(client.getFd(), Replies::RPL_YOUREOPER, "");
    ++m_numOps;
}


int                 countChars(std::string str)
{
    int count = 0;

    for (int i = 0; i < str.size(); ++i)
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
        #ifdef DEBUG
        std::cout << message.getCmd()<< " this is the command\n";
        #endif
        
        // This two lines might be deleting our message
        // if (!messages.empty())
        // TODO: replace with switch case
        // TODO: a map that has command function as values and the string command as key
        std::string command = message.getCmd();


        if (command == USER_COMMAND)
            m_reply(clientFd, Replies::ERR_ALREADYREGISTRED, ""); // not sure about this
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

        int    badFd = this->m_pfds[pollIndex].fd;
        this->m_nicknames.erase(m_clients[badFd].getNickname());
        m_clients.erase(badFd);
        this->m_pfds.erase(this->m_pfds.begin() + pollIndex);
        close(badFd);
    }
    else
    {
        buffer[bytesRead] = '\0';
        std::cout << "this is the buffer: " << buffer << std::endl;
        // send(this->m_pfds[pollIndex].fd, ":555 001 ohachim :welcome\r\n", 86, 0);
        if (this->m_manageRecv(buffer, this->m_pfds[pollIndex].fd))
        {
            if (this->m_isAuthenticated(this->m_pfds[pollIndex].fd))
                this->m_relay(this->m_pfds[pollIndex].fd); // TODO: INSERT TRY AUTHENTIFICATE HERE
            else
            {
                #ifdef DEBUG
                std::cout << "client not authenticated" << std::endl;
                #endif
                this->m_tryAuthentificate(m_clients[this->m_pfds[pollIndex].fd]);
                if (this->m_isAuthenticated(this->m_pfds[pollIndex].fd))
                    this->m_relay(this->m_pfds[pollIndex].fd);
            }
        }
    }
}

void     Server::setServPassword(std::string password)
{
    if (password.empty())
    {
        std::cout << "Error: Password cannot be empty.\n";
        std::exit(-1);
    }
    m_passProtected = true;
    m_password = password;
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
    if (len > 9 || len < 1 || nick == "anonymous")
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
    if (client.isUserAuth() && client.isNickAuth() && (!m_passProtected || client.isPassAuth()))
    {
        #ifdef DEBUG
        std::cout << "Has authenticated correctly\n";
        #endif
        m_reply(client.getFd(), Replies::RPL_WELCOME, client.getNickname());
        m_reply(client.getFd(), Replies::RPL_YOURHOST, client.getNickname() + " :Your host is " + m_serverName + ", running version " + m_version);
        m_reply(client.getFd(), Replies::RPL_CREATED, "");
        m_reply(client.getFd(), Replies::RPL_MYINFO, "");
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
            if (this->m_isValidCommand(msg.getCmd()))
                m_reply(client.getFd(), Replies::ERR_NOTREGISTERED, "");
            if (!client.getMessageQueue().empty())
                client.getMessageQueue().pop_front();
            return (false);
        }
        if (msg.getCmd() == USER_COMMAND)
            m_userCmd(client);
        else if (msg.getCmd() == NICK_COMMAND)
            m_nickCmd(client);
        else if (msg.getCmd() == PASS_COMMAND)
            m_passCmd(client);
        if (!client.getMessageQueue().empty())
            client.getMessageQueue().pop_front();
    }
    return (this->m_isAuthenticated(client.getFd()));
}
// heavy refactor

void    Server::m_reply(int clientFd, int replyCode, std::string message) // TODO:: change this so that it can take a string argument, needed for MODE reply
{
    #ifdef DEBUG_USERHOST
    std::cout << m_clients[clientFd].getNickname() << "|\n";
    std::cout << extraArg << "+++" << clientFd << std::endl;
    #endif
    
    switch (replyCode)  // TODO: care for message syntax
    {
        case Replies::RPL_WELCOME :\
            this->m_send(clientFd, ":" + this->m_serverName + " 001 " + message + " :Welcome sunshine\r\n");
            break;
        case Replies::RPL_YOURHOST:  // TODO: care for message syntax
            this->m_send(clientFd, ":" + this->m_serverName + " 002 " + message + "\r\n");
            break;
        case Replies::RPL_CREATED:
            this->m_send(clientFd, ':' + this->m_serverName + " 003 " + m_clients[clientFd].getNickname() + " :This server was created Sat Feb 12 2020 at 10:40:00 GMT\r\n");
            break;
        case Replies::RPL_MYINFO: // TODO: decide on the possible modes for channels and users
            this->m_send(clientFd, ':' + this->m_serverName + " 004 " + m_clients[clientFd].getNickname() + " :" + this->m_serverName + ' ' + this->m_version + ' ' + "ao" + ' ' + "mtov\r\n");
            break;
        case Replies::RPL_BOUNCE:
            this->m_send(clientFd, ':' + this->m_serverName + " 005 " + m_clients[clientFd].getNickname() + " :Sir t9awed hna 'DS9.GeekShed.net', port '6667'\r\n");
            break;
        case Replies::RPL_USERHOST : // causes BitchX segmentation fault
            this->m_send(clientFd, ":" + this->m_serverName + " 302 " + message + "\r\n");
            break;
        case Replies::ERR_NICKNAMEINUSE :\
            this->m_send(clientFd, ':' + this->m_serverName + " 433 * " + m_clients[clientFd].getMessageQueue().front().getArgs().front() + " :Nickname is already in use hun\r\n");;
            break;
        case Replies::ERR_ERRONEUSNICKNAME :\
            this->m_send(clientFd, ":" + this->m_serverName + " 432 " + message + " :Erroneous nickname\r\n");
            break;
        case Replies::ERR_NEEDMOREPARAMS :\
            this->m_send(clientFd, ':' + this->m_serverName + " 461 " + m_clients[clientFd].getMessageQueue().front().getCmd()+ " :Not enough parameters\r\n"); // needs the name of the command
            break;
        case Replies::ERR_ALREADYREGISTRED :\
            this->m_send(clientFd, ':' + this->m_serverName + " 462 :Unauthorized command (already registered)\r\n");
            break;
        case Replies::ERR_USERSDONTMATCH:
            this->m_send(clientFd, ':' + this->m_serverName + " 446 :Cannot change mode for other users\r\n");
            break;
        case Replies::ERR_UMODEUNKNOWNFLAG:\
            this->m_send(clientFd, ':' + this->m_serverName + " 501 :Unknown MODE flag\r\n");
            break;
        case Replies::RPL_UMODEIS: // TODO: EXTRA CARE
            this->m_send(clientFd, ':' + m_clients[clientFd].getNickname() + " MODE " + m_clients[clientFd].getNickname() + " :" + message + "\r\n"); // TODO: change +i to the correct value of modes
            break;
        case Replies::ERR_NOMOTD :\
            this->m_send(clientFd, ':' + this->m_serverName + " 422 :MOTD File is missing\r\n");
            break;
        case Replies::RPL_MOTDSTART :\
            this->m_send(clientFd, ':' + this->m_serverName + " 375 " + m_clients[clientFd].getNickname() +  " :- " + this->m_serverName + " Message of the day -\r\n");
            break;
        case Replies::RPL_ENDOFMOTD :\
            this->m_send(clientFd, ':' + this->m_serverName + " 376 " + m_clients[clientFd].getNickname() + " :End of MOTD command\r\n");
            break;
        case Replies::RPL_MOTD: // TODO: DECIDE IF WE WANNA PRINT DATE AFTER EVERY LINE
            this->m_send(clientFd, ':' + this->m_serverName + " 372 " + m_clients[clientFd].getNickname() + " :- " + message + "\r\n");
            break;
        case Replies::RPL_NOWAWAY: // TODO: care for message syntax
            this->m_send(clientFd, ':' + this->m_serverName + " 306 :You have been marked as being away\r\n");
            break;
        case Replies::RPL_UNAWAY:  // TODO: care for message syntax
            this->m_send(clientFd, ':' + this->m_serverName + " 305 :You are no longer marked as being away\r\n");
            break;
        case Replies::ERR_NOTREGISTERED:  // TODO: care for message syntax
            this->m_send(clientFd, ':' + this->m_serverName + " 451 :You have not registered\r\n");
            break;
        case Replies::ERR_UNKNOWNCOMMAND:
            this->m_send(clientFd, ':' + this->m_serverName + " 421 :" + message + " :Unknown command\r\n");
            break;
        case Replies::RPL_LUSERCLIENT:
            this->m_send(clientFd, ':' + this->m_serverName + " 251 " + m_clients[clientFd].getNickname() + " :" + std::to_string(this->m_athenticatedUserNum) + " user(s) on the server\r\n"); // TODO: STILL need to add number of services and the number of servers
            break;
        case Replies::RPL_LUSERUNKNOWN:
            this->m_send(clientFd, ':' + this->m_serverName + " 253 " + m_clients[clientFd].getNickname() + " :" + std::to_string(this->m_calculateUnknownConnections()) + " unknown connection(s)\r\n");
            break;
        case Replies::RPL_LUSERME:
            this->m_send(clientFd, ':' + this->m_serverName + " 255 " + m_clients[clientFd].getNickname() + " :" + std::to_string(this->m_calculateKnownConnections()) + " client(s)\r\n");
            break;
        case Replies::RPL_PINGREQUEST:
            this->m_send(clientFd,':' + this->m_serverName + " PONG " + this->m_serverName + " :" + m_clients[clientFd].getNickname() + "\r\n");
            break;
        // the next too need the channel name (to be concidered when refactoring)
        case Replies::ERR_BANNEDFROMCHAN :\
            this->m_send(clientFd, ":" + this->m_serverName + " 474 " + message + " :Cannot join channel (+b)\r\n");
            break;
        case Replies::ERR_BADCHANNELKEY :\
            this->m_send(clientFd, ":" + this->m_serverName + " 475 " + message + " :Cannot join channel (+k)\r\n");
            break;
             // should change after refactoring
        case Replies::ERR_NOSUCHCHANNEL :\
            this->m_send(clientFd, ':' + this->m_serverName + " 403 " + message + " :No such channel\r\n");
            break;
        case Replies::ERR_NORECIPIENT :\
            this->m_send(clientFd, ':' + this->m_serverName + " 411  :No recipient given " + m_clients[clientFd].getMessageQueue().front().getCmd()+ "\r\n");
            break;
        case Replies::ERR_NOTEXTTOSEND :\
            this->m_send(clientFd, ':' + this->m_serverName + " 412 :No text to send\r\n");
            break;
        case Replies::ERR_NOSUCHNICK :\
            this->m_send(clientFd, ':' + this->m_serverName + " 401 " + m_clients[clientFd].getMessageQueue().front().getArgs().front() + " :No such nick/channel\r\n");
            break;
            // needs to change
        case Replies::ERR_TOOMANYTARGETS :\
            this->m_send(clientFd, ':' + this->m_serverName + " 407 " + m_clients[clientFd].getMessageQueue().front().getArgs().front() + " :too many recipients.\r\n");
            break;
        case Replies::RPL_WHOISUSER:
            this->m_send(clientFd, message + "\r\n");
            break;
        case Replies::RPL_WHOISSERVER:
            this->m_send(clientFd, message + "\r\n");
            break;
        case Replies::RPL_WHOISOPERATOR:
            this->m_send(clientFd, message + "\r\n");
            break;
        case Replies::RPL_AWAY:
            this->m_send(clientFd, message + "\r\n");
            break;
        case Replies::RPL_ENDOFWHOIS:
            this->m_send(clientFd, message + "\r\n");
            break;
        case Replies::ERR_NOTONCHANNEL:
            this->m_send(clientFd, m_makeReplyHeader(Replies::ERR_NOTONCHANNEL, this->m_clients[clientFd].getNickname()) + ' ' + message + " :You're not on that channel\r\n");
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
        case Replies::ERR_WRONGCHANMODESYNTAX:
            this->m_send(clientFd, m_makeReplyHeader(Replies::ERR_CHANOPRIVSNEEDED, this->m_clients[clientFd].getNickname()) + ' ' + message + " USAGE: MODE <channel> *( ( '-' / '+' ) *<modes> *<modeparams> )\r\n");
            break;
        case Replies::ERR_USERONCHANNEL:
            this->m_send(clientFd, ':' + this->m_serverName + " 443 <user> <channel> :is already on channel\r\n");
            break;
        case Replies::RPL_NAMREPLY:
            this->m_send(clientFd, ':' + this->m_serverName + " 353 " + message + "\r\n");
            break;
        case Replies::RPL_ENDOFNAMES:
            this->m_send(clientFd, ':' + this->m_serverName + " 366 " + message +" :End of NAMES list\r\n");
            break;
        case Replies::RPL_LIST:
            this->m_send(clientFd, ':' + this->m_serverName + " 321 " + message + "\r\n");
            break;
        case Replies::RPL_LISTEND:
            this->m_send(clientFd, ':' + this->m_serverName + " 323 " + message +" End of LIST\r\n");
            break;
        case Replies::ERR_USERNOTINCHANNEL:
            this->m_send(clientFd, m_makeReplyHeader(Replies::ERR_CHANOPRIVSNEEDED, this->m_clients[clientFd].getNickname()) + ' ' + message + END_STRING);
            break;
        case Replies::RPL_UNIQOPIS:
            this->m_send(clientFd, m_makeReplyHeader(Replies::RPL_UNIQOPIS, this->m_clients[clientFd].getNickname()) + ' ' + message + END_STRING);
            break;
        case Replies::RPL_BANLIST:
            this->m_send(clientFd, m_makeReplyHeader(Replies::RPL_BANLIST, this->m_clients[clientFd].getNickname()) + ' ' + message + END_STRING);
            break;
        case Replies::RPL_EXCEPTLIST:
            this->m_send(clientFd, m_makeReplyHeader(Replies::RPL_EXCEPTLIST, this->m_clients[clientFd].getNickname()) + ' ' + message + END_STRING);
            break;
        case Replies::RPL_INVITELIST:
            this->m_send(clientFd, m_makeReplyHeader(Replies::RPL_INVITELIST, this->m_clients[clientFd].getNickname()) + ' ' + message + END_STRING);
            break;
        case Replies::RPL_ENDOFBANLIST:
            this->m_send(clientFd, m_makeReplyHeader(Replies::RPL_ENDOFBANLIST, this->m_clients[clientFd].getNickname()) + ' ' + message + END_STRING);
            break;
        case Replies::RPL_ENDOFEXCEPTLIST:
            this->m_send(clientFd, m_makeReplyHeader(Replies::RPL_ENDOFEXCEPTLIST, this->m_clients[clientFd].getNickname()) + ' ' + message + END_STRING);
            break;        
        case Replies::RPL_ENDOFINVITELIST:
            this->m_send(clientFd, m_makeReplyHeader(Replies::RPL_ENDOFINVITELIST, this->m_clients[clientFd].getNickname()) + ' ' + message + END_STRING);
            break; 
        case Replies::RPL_LUSERCHANNELS:
            this->m_send(clientFd, m_makeReplyHeader(Replies::RPL_LUSERCHANNELS, this->m_clients[clientFd].getNickname()) + ' ' + std::to_string(m_channels.size()) + " :channels formed" + END_STRING);
            break;
        case Replies::RPL_LUSEROP:
            this->m_send(clientFd, m_makeReplyHeader(Replies::RPL_LUSEROP, this->m_clients[clientFd].getNickname()) + ' ' + std::to_string(m_numOps) + " :operator(s) online" + END_STRING);
            break;
        case Replies::ERR_INTNEEDED:
            m_send(clientFd,  m_makeReplyHeader(Replies::ERR_INTNEEDED, this->m_clients[clientFd].getNickname()) + ' ' + message + " cannot be interpreted as an integer\r\n");
            break;
        case Replies::ERR_NONICKNAMEGIVEN:
            this->m_send(clientFd, ":" + this->m_serverName + " 431 :No nickname given\r\n");
            break;
        case Replies::ERR_INVITEONLYCHAN:
            this->m_send(clientFd, ":" + this->m_serverName + " 473 " + message + " :Cannot join channel (+i)\r\n");
            break;
        case Replies::ERR_CANNOTSENDTOCHAN:
            this->m_send(clientFd, ":" + this->m_serverName + " 404 " + message + " :Cannot send to channel\r\n");
            break;
        case Replies::RPL_NOTOPIC:
            this->m_send(clientFd,m_makeReplyHeader(Replies::RPL_NOTOPIC, this->m_clients[clientFd].getNickname()) + ' ' + message + " :No topic set\r\n");
            break;
    }
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
            m_reply(client.getFd(), Replies::RPL_USERHOST, client.getNickname() + " :"\
            + m_clients[m_nicknames[*it]].getNickname() + ((m_clients[m_nicknames[*it]].getModeValue(UserModes::oper)) ? "*" : "\0") + "=" \
            + ((m_clients[m_nicknames[*it]].getModeValue(UserModes::away)) ? "+" : "-") + m_clients[m_nicknames[*it]].getHostname());
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
            m_reply(client.getFd(), Replies::RPL_USERHOST, "");
        it++;
    }
}

void    Server::m_nickCmd(Client & client)
{
    Message& msg = client.getMessageQueue().front();

    if (msg.getArgs().empty())
    {
        m_reply(client.getFd(), Replies::ERR_NONICKNAMEGIVEN,"");
        return ;
    }
    if (!m_checkNickSyntax(msg))
    {
        m_reply(client.getFd(), Replies::ERR_ERRONEUSNICKNAME, msg.getArgs().front());
        return ;
    }
    std::pair<std::map<std::string, int>::iterator , bool> nick =\
    m_nicknames.insert(std::make_pair(msg.getArgs().front(), client.getFd()));
    if (nick.second && m_checkNickSyntax(msg))
    {
        client.setNickname(msg.getArgs().front());
        client.setNickAuth();
    }
    else
        m_reply(client.getFd(), Replies::ERR_NICKNAMEINUSE, "");
}

void    Server::m_userCmd(Client & client)
{
    Message&    msg = client.getMessageQueue().front();
    std::string mode;
    
    if (client.isUserAuth())
        m_reply(client.getFd(), Replies::ERR_ALREADYREGISTRED, "");
    else if (msg.getArgs().size() != 3)
        m_reply(client.getFd(), Replies::ERR_NEEDMOREPARAMS, "");
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
        m_reply(client.getFd(), Replies::ERR_NEEDMOREPARAMS, "");
    else if (client.isPassAuth())
        m_reply(client.getFd(), Replies::ERR_ALREADYREGISTRED, "");
    else
    {
        if (m_passProtected && args.front() == m_password)
            client.setPassAuth();
    }
}

// TODO: needs tweaks, mainly a hostname, a string of the address, and an if else depending on if a message was left or not
// TODO: QUIT FROM CHANNELS
void                    Server::m_quitCmd(Client& client) 
{
    std::string messageToSend = "ERROR: Closing Link: " + client.getUsername() + "(" 
                                + client.getMessageQueue().front().getLiteralMsg().erase(0, 1) + ") " + client.getHostname() + " (Quit: "
                                    + client.getNickname() + ")" + END_STRING;

    this->m_send(client.getFd(), messageToSend);
    std::vector<std::string>::iterator it = client.getChannels().begin();
    std::vector<std::string>::iterator end = client.getChannels().end();
    while (it != end)
    {
        m_p_privMsgCmd_noticeCmd(client, Message(" PART " + client.getNickname() + ' ' + client.getMessageQueue().front().getLiteralMsg().erase(0, 1)), *it);
        m_channels[*it].removeMember(client.getFd());
        m_channels[*it].removeOp(client.getFd());
        m_channels[*it].removeInvited(client.getFd());
        it++;
    }
    // should I cut the connection
    // stop listening to incoming events
    this->m_eraseClientPoll(client.getFd());
    // remove the nickname from the map
    this->m_nicknames.erase(m_clients[client.getFd()].getNickname());
    // removing it from client list
    this->m_clients.erase(client.getFd());
    close(client.getFd());
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
    printVector(chans, "channels:");
    printVector(passes, "passes/nicknames:");
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
    std::cout << password << " :this is the password given\n";
    std::cout << chan.getPassword() << " :this is the chan pass\n";
    if (chan.isInMaskVector(m_clients[clientFd], chan.getBanMasks())
            && !chan.isInMaskVector(m_clients[clientFd], chan.getExceptionBanMasks()))
        m_reply(clientFd, Replies::ERR_BANNEDFROMCHAN, channelName);
    else if (m_channels[channelName].getModeValue(ChannelModes::i_inviteOnly)
            && !chan.isInMaskVector(m_clients[clientFd], chan.getInviteMasks()))
        m_reply(clientFd, Replies::ERR_INVITEONLYCHAN, channelName);
    else if (((passProtected && !chan.checkPassword(password))
            || (!passProtected && !chan.getPassword().empty())))
        m_reply(clientFd, Replies::ERR_BADCHANNELKEY, channelName);
    else if (chan.isMember(clientFd))
        m_reply(clientFd, Replies::ERR_ALREADYREGISTRED, "");
    else
    {
        // TODO: check the limit on the channel
        if (chan.getMembers().size() == chan.m_getUserLimit()) // initialized with -1 in channel constructor
        {
            m_reply(clientFd, Replies::ERR_CHANUSERLIMIT, channelName);
            return;
        }
        if (m_channels[channelName].getModeValue(ChannelModes::i_inviteOnly))
            m_channels[channelName].removeInvited(clientFd);
        chan.addMember(clientFd);
        m_clients[clientFd].pushChannel(channelName, PEASEANT_MODES);
        m_reply(clientFd, Replies::RPL_TOPIC, m_composeRplTopic(m_channels[channelName]));
        m_p_privMsgCmd_noticeCmd(m_clients[clientFd], Message(": " + m_clients[clientFd].getNickname() + " has joined the channel"), channelName);
    }
}

void                    Server::m_addChannel(int clientFd, std::string channelName, std::string password, bool passProtected)
{
    Channel newChannel(PEASEANT_MODES, clientFd, channelName, channelName.at(0), password);

    std::cout << "is the pass protected: " << passProtected << "pass: " << password << std::endl;
    if (passProtected)
        newChannel.setPassword(password);
    newChannel.addMember(clientFd);
    newChannel.setCreatorNick(m_clients[clientFd].getNickname());
    m_channels.insert(m_channels.end(), std::pair<std::string, Channel>(channelName, newChannel));
    m_clients[clientFd].pushChannel(channelName, OWNER_MODES);
    m_clients[clientFd].turnOnMode(ChannelModes::O_Creator, channelName);
    m_clients[clientFd].turnOnMode(ChannelModes::o_OperatorPrivilege, channelName);
    m_p_privMsgCmd_noticeCmd(m_clients[clientFd], Message(": " + channelName + " channel has been created by " + m_clients[clientFd].getNickname()), channelName);
}

void                    Server::m_joinCmd(Client & client)
{
    std::vector<std::string>    chans;
    std::vector<std::string>    passes;
    Message& msg = client.getMessageQueue().front();
    bool                        passProtected = true;
    
    ////// TODO: syntax check
    if (msg.getArgs().empty())
        m_reply(client.getFd(), Replies::ERR_NEEDMOREPARAMS, "");
    else if (msg.getArgs().size() == 1 && msg.getArgs().front() == "0")
        m_partZero(client);
    else
    {
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
            else
                std::cout << "pass from vector :" << *it_passes << std::endl;
            if (m_channelExists(*it_chan))
                m_addClientToChan(client.getFd(), *it_chan, (passProtected)? *it_passes : "", passProtected);
            else
                m_addChannel(client.getFd(), *it_chan, (passProtected)? *it_passes : "", passProtected);
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
    std::vector<std::string> channelNames;
    /// TODO: syntax check
    std::vector<std::string>::iterator it = msg.getArgs().begin();
    std::vector<std::string>::iterator end = msg.getArgs().end();
    if (!m_grabChannelsNames(msg, channelNames))
    {
        m_reply(client.getFd(), Replies::ERR_NEEDMOREPARAMS, "");
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
            m_p_privMsgCmd_noticeCmd(client, Message("PART " + client.getNickname()), *it_chan);
        if (!m_channels[*it_chan].getMembers().size())
            m_channels.erase(*it_chan);
        it_chan++;
    }
}

void                        Server::m_partZero(Client &client)
{
    std::vector<std::string> & channelNames = client.getChannels();
    std::vector<std::string>::iterator it_chan = channelNames.begin();
    std::vector<std::string>::iterator end_chan = channelNames.end();
    while (it_chan != end_chan)
    {
        m_channels[*it_chan].removeMember(client.getFd());
        m_channels[*it_chan].removeOp(client.getFd());
        client.partChannel(*it_chan);
        // this might be wrong its just a guess
        m_p_privMsgCmd_noticeCmd(client, Message("PART "  + client.getNickname()), *it_chan);
        it_chan++;
    }
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
                m_reply(client.getFd(), Replies::ERR_NOSUCHNICK, "");
            return ;
        }
        else if ((it_chan != m_channels.end() && m_channels[target].getModeValue(ChannelModes::n_noOutsideMessage) && !m_channels[target].isMember(client.getFd() && !client.getModeValue(UserModes::oper)))
         || (it_chan != m_channels.end() && m_channels[target].getModeValue(ChannelModes::m_moderated) && !client.getModeValue(ChannelModes::v_voicePrivilege) && !m_channels[target].isOp(client.getFd()) && !client.getModeValue(UserModes::oper)))
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
            if (m_clients[*it].getModeValue(UserModes::away))
                m_send(client.getFd(), ":" + m_clients[*it].getNickname() + "!" + m_clients[*it].getUsername() + "@" + m_clients[*it].getHostname() + " " + m_clients[*it].getAwayMsg());
            if (isAnonymous)
                m_send(*it, ":anonymous!anonymous@anonymous " + msg.getMsg());
            else
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
                m_reply(client.getFd(), Replies::ERR_NOSUCHNICK, "");
            return ;
        }
        int clientFd = m_nicknames[target];
        if (m_clients[clientFd].getModeValue(UserModes::away))
                m_send(client.getFd(), ":" + m_clients[clientFd].getNickname() + "!" + m_clients[clientFd].getUsername() + "@" + m_clients[clientFd].getHostname() + " " + m_clients[clientFd].getAwayMsg());
        m_send(m_nicknames[target], ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " " + msg.getMsg());
    }
}


// an overload of the privmsg command
void                    Server::m_p_privMsgCmd_noticeCmd(Client &client, Message msg, std::string target)
{
    if (target.at(0) == LOCAL_CHAN || target.at(0) == NETWORKWIDE_CHAN)
    {
        std::map<std::string, Channel>::iterator it_chan = m_channels.find(target);
        Channel & chan = m_channels[target];
        std::vector<int> &members = chan.getMembers();
        std::vector<int>::iterator it = members.begin();
        std::vector<int>::iterator end = members.end();
        while (it != end)
        {
            m_send(*it, ':' + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + ' ' + msg.getMsg());
            it++;
        }
    }
    else
    {
        std::map<std::string, int>::iterator it_user = m_nicknames.find(target);
        int clientFd = m_nicknames[target];
        m_send(m_nicknames[target], ':' + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + ' ' + msg.getMsg());
    }
}

void            Server::m_kickCmd(Client &client)
{
    std::cout << "din dyemak\n";
    Message& msg = client.getMessageQueue().front();
    std::vector<std::string>    chans;
    std::vector<std::string>    nicks;
    std::cout << "din dyemak\n";
    m_grabChannelsNames(msg, chans, nicks);
    if (msg.getArgs().size() < 2 || (chans.size() != 1 && nicks.size() != chans.size()))
    {
        m_reply(client.getFd(), Replies::ERR_NEEDMOREPARAMS, "");
        return ;
    }
    std::vector<std::string>::iterator it_chan = chans.begin();
    std::vector<std::string>::iterator end_chan = chans.end();
    std::vector<std::string>::iterator it_nick = nicks.begin();
    std::vector<std::string>::iterator end_nick = nicks.end();
    if (chans.size() == 1)
    {
        if (m_channels[*it_chan].isOp(client.getFd()))
        {
            while (it_nick != end_nick)
            {
                printVector(m_channels[*it_chan].getMembers(), "members");
                printVector(m_channels[*it_chan].getOps(), "opers");
                m_p_privMsgCmd_noticeCmd(client, Message("KICK : " + client.getNickname()), *it_nick);
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
                m_p_privMsgCmd_noticeCmd(client, Message("KICK :" + client.getNickname()), *it_nick);
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
        m_reply(client.getFd(), Replies::ERR_NEEDMOREPARAMS, "");
    std::string target = *args.begin();
    std::string chanName = *(args.begin() + 1);
    if (!m_channelExists(chanName))
        m_reply(client.getFd(), Replies::ERR_NOSUCHCHANNEL, "");
    else if (m_nicknames.find(target) == m_nicknames.end())
        m_reply(client.getFd(), Replies::ERR_NOSUCHNICK, "");
    else if (!m_channels[chanName].isMember(client.getFd()))
        m_reply(client.getFd(), Replies::ERR_NOTONCHANNEL, "");
    else if (!m_channels[chanName].isOp(m_nicknames[target])  && !client.getModeValue(UserModes::oper) && m_channels[chanName].getModeValue(ChannelModes::i_inviteOnly))
        m_reply(client.getFd(), Replies::ERR_CHANOPRIVSNEEDED, "");
    else if (m_channels[chanName].isMember(m_nicknames[target]))
        m_reply(client.getFd(), Replies::ERR_USERONCHANNEL, "");
    else
    {
        m_p_privMsgCmd_noticeCmd(client, Message("INVITE :" + client.getNickname()), target);
        m_p_privMsgCmd_noticeCmd(client, Message("INVITE :" + client.getNickname()), client.getNickname());
        m_channels[chanName].addInvited(m_nicknames[target]);
    }
}

// should be a template
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
            message += " @ " + channelName + " : ";
        else if (m_channels[channelName].getModeValue(ChannelModes::p_private))
            message += " * " + channelName + " : ";
        else
            message += " = " + channelName + " : ";
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
        message += m_channels[channelName].getTopic();
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
        else if (cmd == LIST_COMMAND && !m_channels[*it_chan].getModeValue(ChannelModes::p_private)\
        && !m_channels[*it_chan].getModeValue(ChannelModes::s_secret))
        {
            if (!m_channels[*it_chan].getModeValue(ChannelModes::a_annonymous))
                m_reply(client.getFd(), Replies::RPL_LIST, m_composeList(*it_chan));
            else
                m_reply(client.getFd(), Replies::RPL_LIST, " anonymous channel");
            m_reply(client.getFd(), Replies::RPL_LISTEND, *it_chan);
        }
        it_chan++;
    }
}

std::string    Server::m_possibleCommands[NUM_COMMANDS] = {"USER", "NICK", "PASS", "USERHOST",
                                                            "QUIT", "ISON", "MODE", "PONG",
                                                            "PING", "MOTD", "AWAY", "LUSERS",
                                                            "WHOIS", "TOPIC", "JOIN", "PART",
                                                            "KICK", "PRIVMSG", "NOTICE", "OPER"};
