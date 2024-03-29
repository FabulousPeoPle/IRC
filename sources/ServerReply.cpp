
#include "Server.hpp"

void    Server::m_reply(int clientFd, int replyCode, std::string message)
{  
    switch (replyCode)
    {
        case Replies::RPL_WELCOME :\
            this->m_send(clientFd, ":" + this->m_serverName + " 001 " + message + " :Mar7ba bikom 3andna f server dyalna\r\n");
            break;
        case Replies::RPL_YOURHOST:
            this->m_send(clientFd, ":" + this->m_serverName + " 002 " + message + END_STRING);
            break;
        case Replies::RPL_CREATED:
            this->m_send(clientFd, ':' + this->m_serverName + " 003 " + m_clients[clientFd].getNickname() + " :This server was created Sat Feb 12 2020 at 10:40:00 GMT\r\n");
            break;
        case Replies::RPL_MYINFO:
            this->m_send(clientFd, ':' + this->m_serverName + " 004 " + m_clients[clientFd].getNickname() + " :" + this->m_serverName + ' ' + this->m_version + ' ' + Channel::potentialModes + ' ' + Client::potentialModes + END_STRING);
            break;
        case Replies::RPL_BOUNCE:
            this->m_send(clientFd, ':' + this->m_serverName + " 005 " + m_clients[clientFd].getNickname() + " :Sir t9awed hna 'DS9.GeekShed.net', port '6667'\r\n");
            break;
        case Replies::RPL_USERHOST :
            this->m_send(clientFd, ":" + this->m_serverName + " 302 " + message + END_STRING);
            break;
        case Replies::ERR_NICKNAMEINUSE :\
            this->m_send(clientFd, ':' + this->m_serverName + " 433 * " + m_clients[clientFd].getMessageQueue().front().getArgs().front() + " :Nickname is already in use hun\r\n");;
            break;
        case Replies::ERR_ERRONEUSNICKNAME :\
            this->m_send(clientFd, ":" + this->m_serverName + " 432 " + message + " :Erroneous nickname\r\n");
            break;
        case Replies::ERR_NEEDMOREPARAMS :\
            this->m_send(clientFd, ':' + this->m_serverName + " 461 " + message + " :Not enough parameters\r\n");
            break;
        case Replies::ERR_ALREADYREGISTRED :\
            this->m_send(clientFd, ':' + this->m_serverName + " 462 " + message + " :Unauthorized command (already registered)\r\n");
            break;
        case Replies::ERR_USERSDONTMATCH:
            this->m_send(clientFd, ':' + this->m_serverName + " 446 :Cannot change mode for other users\r\n");
            break;
        case Replies::ERR_UMODEUNKNOWNFLAG:\
            this->m_send(clientFd, ':' + this->m_serverName + " 501 :Unknown MODE flag\r\n");
            break;
        case Replies::RPL_UMODEIS:
            this->m_send(clientFd, ':' + m_clients[clientFd].getNickname() + " MODE " + m_clients[clientFd].getNickname() + " :" + message + END_STRING);
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
        case Replies::RPL_MOTD:
            this->m_send(clientFd, ':' + this->m_serverName + " 372 " + m_clients[clientFd].getNickname() + " :- " + message + END_STRING);
            break;
        case Replies::RPL_NOWAWAY:
            this->m_send(clientFd, ':' + this->m_serverName + " 306 :You have been marked as being away\r\n");
            break;
        case Replies::RPL_UNAWAY:
            this->m_send(clientFd, ':' + this->m_serverName + " 305 :You are no longer marked as being away\r\n");
            break;
        case Replies::ERR_NOTREGISTERED:
            this->m_send(clientFd, ':' + this->m_serverName + " 451 :You have not registered\r\n");
            break;
        case Replies::ERR_UNKNOWNCOMMAND:
            this->m_send(clientFd, ':' + this->m_serverName + " 421 :" + message + " :Unknown command\r\n");
            break;
        case Replies::RPL_LUSERCLIENT:
            this->m_send(clientFd, ':' + this->m_serverName + " 251 " + m_clients[clientFd].getNickname() + " :There are " + intToString(this->m_authenticatedUserNum) + " users and 0 services on 1 servers\r\n");
            break;
        case Replies::RPL_LUSERUNKNOWN:
            this->m_send(clientFd, ':' + this->m_serverName + " 253 " + m_clients[clientFd].getNickname() + " " + intToString(this->m_calculateUnknownConnections()) + " :unknown connection(s)\r\n");
            break;
        case Replies::RPL_LUSERME:
            this->m_send(clientFd, ':' + this->m_serverName + " 255 " + m_clients[clientFd].getNickname() + " :I have " + intToString(this->m_calculateKnownConnections()) + " clients and 1 servers\r\n");
            break;
        case Replies::RPL_PINGREQUEST:
            this->m_send(clientFd,':' + this->m_serverName + " PONG " + this->m_serverName + " :" + m_clients[clientFd].getNickname() + END_STRING);
            break;
        case Replies::ERR_BANNEDFROMCHAN :\
            this->m_send(clientFd, ":" + this->m_serverName + " 474 " + message + " :Cannot join channel (+b)\r\n");
            break;
        case Replies::ERR_BADCHANNELKEY :\
            this->m_send(clientFd, ":" + this->m_serverName + " 475 " + message + " :Cannot join channel (+k)\r\n");
            break;
        case Replies::ERR_NOSUCHCHANNEL :\
            this->m_send(clientFd, ':' + this->m_serverName + " 403 " + message + " :No such channel\r\n");
            break;
        case Replies::ERR_NORECIPIENT :\
            this->m_send(clientFd, ':' + this->m_serverName + " 411  :No recipient given " + m_clients[clientFd].getMessageQueue().front().getCmd()+ END_STRING);
            break;
        case Replies::ERR_NOTEXTTOSEND :\
            this->m_send(clientFd, ':' + this->m_serverName + " 412 :No text to send\r\n");
            break;
        case Replies::ERR_NOSUCHNICK :\
            this->m_send(clientFd, ':' + this->m_serverName + " 401 " + message + " :No such nick/channel\r\n");
            break;
        case Replies::ERR_TOOMANYTARGETS :\
            this->m_send(clientFd, ':' + this->m_serverName + " 407 " + m_clients[clientFd].getMessageQueue().front().getArgs().front() + " :too many recipients.\r\n");
            break;
        case Replies::RPL_WHOISUSER:
            this->m_send(clientFd, message + END_STRING);
            break;
        case Replies::RPL_WHOISSERVER:
            this->m_send(clientFd, message + END_STRING);
            break;
        case Replies::RPL_WHOISOPERATOR:
            this->m_send(clientFd, message + END_STRING);
            break;
        case Replies::RPL_AWAY:
            this->m_send(clientFd, message + END_STRING);
            break;
        case Replies::RPL_WHOISCHANNELS:
            this->m_send(clientFd, message + END_STRING);
            break;
        case Replies::RPL_ENDOFWHOIS:
            this->m_send(clientFd, message + END_STRING);
            break;
        case Replies::ERR_NOTONCHANNEL:
            this->m_send(clientFd, m_makeReplyHeader(Replies::ERR_NOTONCHANNEL, this->m_clients[clientFd].getNickname()) + ' ' + message + " :You're not on that channel\r\n");
            break;
        case Replies::RPL_TOPIC:
            this->m_send(clientFd, m_makeReplyHeader(Replies::RPL_TOPIC, this->m_clients[clientFd].getNickname()) + ' ' + message + END_STRING);
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
            this->m_send(clientFd, ':' + m_serverName + " MODE " + message + END_STRING);
            break;
        case Replies::ERR_CHANOPRIVSNEEDED:
            this->m_send(clientFd, m_makeReplyHeader(Replies::ERR_CHANOPRIVSNEEDED, this->m_clients[clientFd].getNickname()) + ' ' + message + " :You're not channel/server operator\r\n");
            break;
        case Replies::ERR_WRONGCHANMODESYNTAX:
            this->m_send(clientFd, m_makeReplyHeader(Replies::ERR_CHANOPRIVSNEEDED, this->m_clients[clientFd].getNickname()) + ' ' + message + " :WRONG USAGE\r\n");
            break;
        case Replies::ERR_USERONCHANNEL:
            this->m_send(clientFd, m_makeReplyHeader(Replies::ERR_USERONCHANNEL, this->m_clients[clientFd].getNickname()) + ' ' + message + " :is already on channel\r\n");
            break;
        case Replies::RPL_NAMREPLY:
            this->m_send(clientFd, m_makeReplyHeader(Replies::RPL_NAMREPLY, this->m_clients[clientFd].getNickname()) + ' '  + message + END_STRING);
            break;
        case Replies::RPL_ENDOFNAMES:
            this->m_send(clientFd, m_makeReplyHeader(Replies::RPL_ENDOFNAMES, this->m_clients[clientFd].getNickname()) + ' ' + message +" :End of NAMES list\r\n");
            break;
        case Replies::RPL_LIST:
            this->m_send(clientFd, m_makeReplyHeader(Replies::RPL_LIST, this->m_clients[clientFd].getNickname()) + ' ' + message + END_STRING);
            break;
        case Replies::RPL_LISTEND:
            this->m_send(clientFd, m_makeReplyHeader(Replies::RPL_LISTEND, this->m_clients[clientFd].getNickname()) + ' ' + message +" End of /LIST\r\n");
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
            this->m_send(clientFd, m_makeReplyHeader(Replies::RPL_LUSERCHANNELS, this->m_clients[clientFd].getNickname()) + ' ' + intToString(m_channels.size()) + " :channels formed" + END_STRING);
            break;
        case Replies::RPL_LUSEROP:
            this->m_send(clientFd, m_makeReplyHeader(Replies::RPL_LUSEROP, this->m_clients[clientFd].getNickname()) + ' ' + intToString(m_numOps) + " :operator(s) online" + END_STRING);
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
            this->m_send(clientFd, m_makeReplyHeader(Replies::RPL_NOTOPIC, this->m_clients[clientFd].getNickname()) + ' ' + message + " :No topic set\r\n");
            break;
        case Replies::ERR_NOTAWAY:
            this->m_send(clientFd, m_makeReplyHeader(Replies::RPL_NOTOPIC, this->m_clients[clientFd].getNickname()) + " :Not away\r\n");
            break;
        case Replies::RPL_FILERECIEVED:
            this->m_send(clientFd, m_makeReplyHeader(Replies::RPL_FILERECIEVED, m_clients[clientFd].getNickname()) + " :You have recieved this file " + message + END_STRING);
            break;
        case Replies::RPL_FILESENT:
            this->m_send(clientFd, m_makeReplyHeader(Replies::RPL_FILESENT, m_clients[clientFd].getNickname()) + " :You have sent this file " + message +END_STRING);
            break;
        case Replies::RPL_READYTORECIEVE:
            this->m_send(clientFd, m_makeReplyHeader(Replies::RPL_READYTORECIEVE, m_clients[clientFd].getNickname()) + " :Server ready to recieve " + message +END_STRING);
            break;
        case Replies::ERR_FTPTIMEOUT:
            this->m_send(clientFd, m_makeReplyHeader(Replies::ERR_FTPTIMEOUT, m_clients[clientFd].getNickname()) + " :You took too long to send file\r\n");
            break;
        case Replies::ERR_RECIEVEDNOFILES:
            this->m_send(clientFd, m_makeReplyHeader(Replies::ERR_RECIEVEDNOFILES, m_clients[clientFd].getNickname()) + " :You have not recieved any files\r\n");
            break;
        case Replies::RPL_READYTOSEND:
            this->m_send(clientFd, m_makeReplyHeader(Replies::RPL_READYTOSEND, m_clients[clientFd].getNickname()) + " :Server is sending file with length " + message + " |\r\n");
            break;
        case Replies::RPL_ISON:
            this->m_send(clientFd, m_makeReplyHeader(Replies::RPL_ISON, m_clients[clientFd].getNickname()) + " :" + message + END_STRING);
            break;
        case Replies::ERR_CHANUSERLIMIT:
            this->m_send(clientFd, m_makeReplyHeader(Replies::ERR_CHANUSERLIMIT, m_clients[clientFd].getNickname()) + " " + message +  " :Channel limit reached" + END_STRING);
            break;
        default:
            return ;
    }
}
