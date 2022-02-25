/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azouiten <azouiten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/11 16:41:32 by ohachim           #+#    #+#             */
/*   Updated: 2022/02/25 20:01:36 by azouiten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#ifndef _SERVER_HPP_
# define _SERVER_HPP_

#include <sstream>
#include <fstream>
#include <iomanip>
#include <cstdint>
#include <string>
#include <iostream>

#include <cstring>
#include <unistd.h>
#include <fcntl.h>

#include <vector>
#include <map>

#include <algorithm>

#include <poll.h>

#include <Client.hpp>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <Channel.hpp>

#define DEFAULT_HOSTNAME NULL
#define DEFAULT_PORT "6667"

#define MAX_CONNECTIONS 10

#define BUFFER_SIZE 512

// #define ILLEGALE_CHARS_NICK "#&*@.,;:\"\'"

#define END_STRING "\r\n"

// #define BIG_NUMBER 2000000000



namespace Replies
{
    enum
    {
        /******-AUTHENTIFICATION-*******/
        RPL_WELCOME = 1,
        RPL_YOURHOST = 2,
        RPL_CREATED = 3,
        RPL_MYINFO = 4,
        RPL_BOUNCE = 5,
        /*******************************/

        ERR_NICKNAMEINUSE = 433,
        ERR_ERRONEUSNICKNAME = 423,
        RPL_USERHOST = 302,
        ERR_NEEDMOREPARAMS = 461,
        ERR_ALREADYREGISTRED = 462,
        ERR_USERSDONTMATCH = 502,
        ERR_UMODEUNKNOWNFLAG = 501,
        RPL_UMODEIS = 221,
        RPL_MOTDSTART = 375,
        RPL_MOTD = 372,
        RPL_ENDOFMOTD = 376,
        ERR_NOMOTD = 422,
        /*****-AWAY-*********/
        ERR_NOTAWAY = 111, 
        RPL_NOWAWAY = 306,
        RPL_UNAWAY = 305,
        ERR_NOTREGISTERED = 541,
        ERR_UNKNOWNCOMMAND = 421,
        /********************/
        /*****-LUSERS-*******/
        RPL_LUSERCLIENT = 251,
        RPL_LUSEROP = 252,
        RPL_LUSERUNKNOWN = 253,
        RPL_LUSERME = 255,
        RPL_LUSERCHANNELS = 254,
        /********************/
        RPL_PINGREQUEST = 800,
        ERR_BANNEDFROMCHAN = 474,
        ERR_BADCHANNELKEY = 457,
        ERR_NOSUCHCHANNEL = 403,
        ERR_NORECIPIENT = 411,
        ERR_NOTEXTTOSEND = 412,
        ERR_TOOMANYTARGETS = 407,

        /******-WHOIS-*******/
        RPL_WHOISUSER = 311,
        RPL_WHOISSERVER = 312,
        RPL_ENDOFWHOIS = 318,
        ERR_NOSUCHNICK = 401,
        RPL_WHOISOPERATOR = 313,
        RPL_AWAY = 301,
        /********************/

        /******-TOPIC-*******/
        ERR_NOTONCHANNEL = 442,
        RPL_NOTOPIC = 331,
        RPL_TOPIC = 332,
        /********************/

        /******-OPER-********/
        RPL_YOUREOPER = 381,
        ERR_NOOPERHOST = 491,
        ERR_PASSWDMISMATCH = 464,
        /********************/

        /****-MODE_CHANNEL-**/
        RPL_CHANNELMODEIS = 324,
        ERR_CHANOPRIVSNEEDED = 482,
        ERR_USERNOTINCHANNEL = 441,
        RPL_UNIQOPIS = 325,
        RPL_BANLIST = 367,
        RPL_INVITELIST = 346,
        RPL_EXCEPTLIST = 348,
        RPL_ENDOFEXCEPTLIST = 349,
        RPL_ENDOFBANLIST = 368,
        RPL_ENDOFINVITELIST = 347,
        ERR_INTNEEDED = 666,
        ERR_WRONGCHANMODESYNTAX = 888,
        /********************/
        
        ERR_CHANUSERLIMIT = 777,
        ERR_USERONCHANNEL = 443,
        RPL_NAMREPLY = 353,
        RPL_ENDOFNAMES = 366,
        RPL_LIST = 321,
        RPL_LISTEND = 323,
        ERR_NONICKNAMEGIVEN = 431,
        ERR_INVITEONLYCHAN = 473,
        ERR_CANNOTSENDTOCHAN = 404,
    };
};

#define USER_COMMAND        "USER"
#define NICK_COMMAND        "NICK"
#define PASS_COMMAND        "PASS"
#define USERHOST_COMMAND    "USERHOST"
#define QUIT_COMMAND        "QUIT"
#define ISON_COMMAND        "ISON"
#define MODE_COMMAND        "MODE"
#define PONG_COMMAND        "PONG" // yeet
#define PING_COMMAND        "PING" // yeet
#define MOTD_COMMAND        "MOTD"
#define AWAY_COMMAND        "AWAY"
#define LUSERS_COMMAND      "LUSERS"
#define WHOIS_COMMAND       "WHOIS"
#define JOIN_COMMAND        "JOIN"
#define PART_COMMAND        "PART"
#define KICK_COMMAND        "KICK"
#define NOTICE_COMMAND      "NOTICE"
#define PRIVMSG_COMMAND     "PRIVMSG"
#define OPER_COMMAND        "OPER"
#define TOPIC_COMMAND       "TOPIC"
#define NAMES_COMMAND       "NAMES"
#define LIST_COMMAND        "LIST"
#define WHO_COMMAND         "WHO"
#define KICK_COMMAND        "KICK"
#define INVITE_COMMAND      "INVITE"

#define NUM_COMMANDS 20

#define MOTD_LENGTH_LINE 80


typedef struct addrinfo         t_addrinfo;
typedef struct sockaddr         t_sockaddr;
typedef struct sockaddr_storage t_sockaddr_storage;
typedef struct sockaddr_in      t_sockaddr_in;
typedef struct sockaddr_in6     t_sockaddr_in6;
typedef struct pollfd           t_pollfd;

std::string strToken(std::string str);

class Client;
class Message;
class Channel;
class Server;
// Add this one to client class.
typedef struct      s_m_socketInfo 
{
    int family;
    int socktype;
    int protocol;
}                   t_socketInfo;

class Server {
    public:
        typedef void (Server::*cmdFun)(Client&);

                                    // constructors are probably going to be useless
                                        Server(void);
                                        Server(std::string port, std::string hostname, std::string serverName, int maxClients);
                                        Server(const Server& serverRef);
                                        ~Server();
        void                            initializeCmdFuncs(void);
        Server&                         operator=(const Server& serverRef);

        std::string                     getPort(void) const;
        std::string                     getHostname(void) const;
        t_addrinfo                      getHints(void) const;
        t_addrinfo                      getServInfo(void) const;
        int                             getSockfd(void) const;
        t_sockaddr_in6                  getAddr_in6(void) const;
        t_sockaddr_in                   getAddr_in(void) const;


        std::string                     convertToHostname(t_sockaddr_storage& remoteAddr, int sock_fd);

        //  Might wanna set protection for multiple IP adresses

        std::string                     getServName(void) const;

        int                             setServerInfo(void);
        void                            setServerHints(int family, int sockType, int flags);

        //  might upgrade it to taking socktype and protocol?
        int                             setSockfd(int family);

        int                             setSockfd_in(void);
        int                             setSockfd_in6(void);

        void                            setOperPassword(std::string password); // server password is bo7do

        int                             listen(void);

        int                             startServer(void);
        void                            setServPassword(std::string password);


    private:

        bool                            m_isAuthenticated(int clientFd);
        void                            m_managePoll(void);
        int                             m_manageServerEvent(void);
        void                            m_manageClientEvent(int pollIndex);
        //  Maybe this is usless since we are always going to connect to the same thing
        int                             m_setSocket(t_socketInfo socketInfo, t_sockaddr* addr, socklen_t addrlen);
        void                            m_poll(void);
        void*                           m_getInAddr(t_sockaddr* addr) const;
        //void                            m_fillSocketInfo(t_socketInfo& socketInfo, int family, int socktype, int protocol);
        void                            m_parse(std::string& str);
        int                             m_manageRecv(std::string message, int clientFd);
        
        bool                            m_checkStatusAuth(Client& client);
        bool                            m_tryAuthentificate(Client& client);
        
        void                            m_userCmd(Client & client);
        void                            m_nickCmd(Client & client);
        
        void                            m_userhostCmd(Client & client);
        void                            m_isonCmd(Client & client);
        
        void                            m_relay(int clientFd);
        void                            m_debugAuthentificate(int clientFd);
        int                             m_send(int toFd, std::string message);

        void                            m_reply(int clientFd, int replyCode, std::string message);
        void                            m_setCommandFuncs(void);

        bool                            m_isModesSyntaxValid(std::vector<std::string> arguments);

        bool                            m_checkNickSyntax(Message& message);
        void                            m_eraseClientPoll(int clientFd);

        bool                            m_onlyOps(std::vector<std::string> arguments);


        void                            m_quitCmd(Client& client); // Needs a recheck
        // Need to know more about channel class
        void                            m_modeCmd(Client& client);
        void                            m_motdCmd(Client& client);
        void                            m_awayCmd(Client& client);
        void                            m_pingCmd(Client& client);
        void                            m_pongCmd(Client& client); // TODO: SHOULD PROBABLY BE REMOVED
        void                            m_lusersCmd(Client& client);
        void                            m_whoisCmd(Client& client);
        void                            m_topicCmd(Client& client);
        void                            m_operCmd(Client& client);
        void                            m_whoCmd(Client& client);
        
        void                            m_userModeCmd(Client& client, Message& message);

        template <typename T>
        void    printVector(T &vector, std::string name)
        {
            typename T::iterator it = vector.begin();
            typename T::iterator end = vector.end();
            std::cout << "printing vector " << name << std::endl;
            while (it != end)
            {
                std::cout << "|" << *it << "|" << std::endl;
                it++;
            }
        }
        void                            m_channelModeCmd(Client& client, Message& message);

        std::string                     m_makeReplyHeader(int replyNum, std::string nickname);

        std::string                     m_composeMotd(std::ifstream& motdFile, std::string clientNick);
        std::string                     m_composeWhoisQuery(Client& QueryClient, std::string clientNickname, int replyCode);
        std::string                     m_composeRplTopic(Channel& channel);
        std::string                     m_composeChannelModes(std::string channelName);
        std::string                     m_composeNames(std::string channelName);
        std::string                     m_composeList(std::string channelName);
        std::string                     m_composeUserNotInChannel(std::string channelName, std::string clientNickname);// const?


        int                             m_manageChannelModes(char mode, char prefix, std::vector<std::string> arguments); // turn arguments into references?
        std::vector<std::string>        m_manageMaskMode(char mode, char prefix, std::vector<std::string> arguments, int& paramToUseIndex);

        void                            m_findNextMask(std::vector<std::string> arguments, int& paramToUseIndex);


        bool                            m_areModesMasks(std::string modes);
        bool                            m_isValidCommand(std::string potentialCommand); // should be const
        bool                            m_isChannelPrefix(char c) const;
        bool                            m_isUser(Client& client) const;
        bool                            m_isMaskUserMatch(std::string nickname, std::string TLD); // TODO: should be more general
        bool                            m_isMask(std::string str);
        bool                            m_isUserSpecificChannelMode(char c) const; // user modes
        bool                            m_isAttributeSetterMode(char c) const; //password and limit users
        bool                            m_isSimpleChannelMode(char c) const; // turn on turn off mdoes
        bool                            m_isMaskMode(char c) const; // the masking ones 
        bool                            m_isClientOper(Client& client, std::string channelName) const; // make variable const
        bool                            m_isWildCardMask(std::string str) const; // Used for who

        void                            m_listMasks(std::vector<std::string> maskList, char mode, Client& client, Channel& channel);

        void                            m_executeModes(std::vector<std::string> arguments, Channel& channel, Client& client);

        std::vector<std::string>        m_getClientsToMode(std::vector<std::string> arguments);

        int                             m_calculateOperators(void);
        int                             m_calculateUnknownConnections(void);
        int                             m_calculateKnownConnections(void);

        
        void                            m_joinCmd(Client & client);
        bool                            m_grabChannelsNames(Message & msg, std::vector<std::string> & chans, std::vector<std::string> & passes);
        bool                            m_grabChannelsNames(Message & msg, std::vector<std::string> & chans);
        bool                            m_channelExists(std::string);
        void                            m_addClientToChan(int clientFd, std::string channelName, std::string password, bool passProtected);
        void                            m_addChannel(int clientFd, std::string channelName, std::string password);

        void                            m_partCmd(Client & client);
        void                            m_partZero(Client & client);

        void                            m_privMsgCmd_noticeCmd(Client &client);
        void                            m_p_privMsgCmd_noticeCmd(Client &client, Message msg, std::string target);
        
        void                            m_kickCmd(Client & client);
        
        void                            m_inviteCmd(Client & client);
        
        void                            m_namesCmd_listCmd(Client & client);
        void                            m_p_namesCmd_listCmd(Client & client,std::string target, std::string cmd); // still not implemented
        void                            m_mapKeysToVector(std::vector<std::string> &vector, std::map<std::string, Channel> &map);//this should become a template for wider usecases
        void                            m_passCmd(Client &client);

        std::string                     m_constructMask(Client& client);

        std::string                     m_getTLD(std::string mask);
        std::vector<int>                m_grabClientsWithMask(std::string mask);
        bool                            m_nickIsValid(std::string &str);
    
        const std::string               m_serverName;
        const std::string               m_port;
        const std::string               m_hostname;
        const std::string               m_version;
        const int                       m_maxClients;
        t_addrinfo*                     m_servinfo;
        t_addrinfo                      m_hints;
        int                             m_athenticatedUserNum;
        // in case we wanted to do it manually // kinda useless now
        t_sockaddr_in                   m_addr_in;
        t_sockaddr_in6                  m_addr_in6;
        int                             m_sockfd;
        // this might be totally usless
        t_socketInfo                    m_socketInfo;

        int                             m_poll_count;
        std::vector<t_pollfd>           m_pfds;
        std::string                     m_operPassword;
        ////////////////////////////////////////////////////
        /// int : client fd , Client : the client object ///
        ////////////////////////////////////////////////////
        std::map<int, Client>           m_clients;
        int                             m_numOps;
        
        ///////////////////////////////////////////////////////
        /// std::string : client nickname , int : client fd ///
        ///////////////////////////////////////////////////////
        std::map<std::string, int>      m_nicknames;

        static std::string              m_possibleCommands[NUM_COMMANDS];
    
        /////////////////////////////////////////////////////////////////
        /// std::string : channel name , Channel : the channel object ///
        /////////////////////////////////////////////////////////////////
        std::map<std::string, Channel>  m_channels;
        std::map<std::string, cmdFun>   m_cmdFuncs;
        bool                            m_passProtected;
        std::string                     m_password;
};

#endif