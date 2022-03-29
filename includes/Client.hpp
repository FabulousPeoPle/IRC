
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include  <vector>
#include <map>
#include <sys/socket.h>
#include <deque>
#include "Message.hpp"
#include <cstdint>
#include <cstring>

#include "Channel.hpp"

class Message;

#define NUM_MODES 6

#define OWNER_MODES 0x7

#define PEASEANT_MODES 0x0

namespace UserModes
{
    enum
    {
        away,
        invisible,
        wallops,
        restricted,
        oper,
        serverNotices,
        total
    };
};


typedef struct      s_fileData
{
    std::string sender;
    std::string reciever;
    std::string name;
    unsigned long length;
    char *      content;
}                   t_fileData;

typedef std::deque<Message> t_messageDQeue;
class Client
{

    private:
        int                                 _sock_fd;
        struct sockaddr_storage             _addr;
        std::string                         lastJoinedChannel;
        socklen_t                           _addr_size;
        std::string                         _nickname;
        std::string                         _username;
        std::string                         hostname;
        std::string                         _ip_address;
        t_messageDQeue                      messages;
        std::string                         _realname;
        bool                                _nickAuth;
        bool                                _userAuth;
        bool                                _passAuth;
        bool                                _isServerOp;
        bool                                _away;
        bool                                _authenticated;
        std::uint8_t                        modes;
        static std::uint8_t                 modeBitMasks[NUM_MODES];
        std::string                         awayMessage;
        
        std::vector<std::string>            _channels;
        
        std::vector<t_fileData >      recievedFiles;
    
    public:

        std::map<std::string, std::uint8_t> _channelModes;
        Client(void);
        Client(Client const & src);
        Client(int, struct sockaddr_storage, socklen_t);
        ~Client(void);


        /////////////////
        //// GETTERS ////
        /////////////////
        int                             getFd(void) const;
        struct sockaddr_storage         getRawAddress(void) const;
        socklen_t                       getAddrLen(void) const;
        t_messageDQeue                  &getMessageQueue(void);
        std::string                     getNickname(void) const;
        std::string                     getUsername(void) const;
        std::string                     getHost(void) const;
        std::string                     getIp(void) const;
        std::string                     getRealname(void) const;
        std::string                     getAwayMsg(void) const;
        bool                            isNickAuth(void) const;
        bool                            isUserAuth(void) const;
        bool                            isPassAuth(void) const;
        bool                            isServerOp(void) const;
        bool                            isAway(void) const;
        bool                            isAuthComplete(void) const;
        bool                            getModeValue(int modeNum) const;
        bool		                    getModeValue(int modeNum, std::string channelName);
        int                             findChanMode(char mode) const;
        std::string                     getLastJoinedChannel(void);
        std::vector<std::string>        &getChannels(void);
        std::vector<t_fileData>         &getFiles(void);
        
        static int                             findMode(char c);
        /////////////////
        //// SETTERS ////
        /////////////////
        void                            setFd(int fd);
        void                            setRawAddress(struct sockaddr_storage addr);
        void                            setAddrLen(socklen_t addrLen);
        void                            pushMsg(Message & message);
        void                            pushFile(t_fileData &fileData);
        void                            setNickname(std::string nickname);
        void                            setUsername(std::string username);
        void                            setIp(std::string ip);
        void                            setHostname(std::string hostname);
        void                            setRealname(std::string realname);
        void                            setAwayMsg(std::string awayMsg);
        void                            setNickAuth(void);
        void                            setUserAuth(void);
        void                            setPassAuth(void);
        void                            setAsServerOp(void);
        void                            setAway(void);
        void                            setAuthComplete(void);
        void                            turnOnMode(int modeNum);
        void		                    turnOnMode(int modeNum, std::string channelName);
        void                            turnOffMode(int modeNum);
        void		                    turnOffMode(int modeNum, std::string channelName);
        void                            pushChannel(std::string channelName, std::uint8_t modes);
        void                            popChannel(std::string channelName);
        void                            deleteFiles(void);
        void                            manageChannelModes(char mode, char prefix, std::vector<std::string> arguments, int paramToUseIndex);

        Client & operator=(Client const & rhs);

        ///////////////
        /// METHODS ///
        ///////////////
        void        partChannel(std::string channelName);
        static std::string                  potentialModes;
        static std::string			        potentialChannelModes;

};

#endif
