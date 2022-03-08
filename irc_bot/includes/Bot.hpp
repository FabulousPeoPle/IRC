#ifndef _BOT_HPP_
# define _BOT_HPP_

# include <stdio.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <string.h>
# include <string>
# include <queue>
# include <chrono>
# include <iostream>
# include <stdlib.h>
# include <sstream>
# include <fcntl.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
#include <fstream>
# include "cmds.hpp"
# include "utils.hpp"

# define END_STRING "\r\n"

//errors codes
# define EC_USAGE 100
# define NOTACMD 101
typedef struct s_msg
{
	std::string user;
	std::string nick;
	std::string host;
	std::string cmd;
	std::string args;
	std::string msg;
}t_msg;

class Bot
{
private:
	std::string         userName;
	std::string         nickName;
    std::string         host;
    int                 port;
    int                 sock;
    std::string         passwd;
    struct sockaddr_in  serv_addr;
    std::vector<std::string> joinedChannels;


public:
						Bot(void);
						~Bot(void);
                        int startBot(std::string host, int port, std::string passwd);
                        void idBot(std::string userName, std::string nickName);
                        void idBot(std::string userName);
private:
                        int m_auth(void);
                        int m_send(std::string msg);
                        void m_listner(void);
                        int m_recv(std::queue<std::string> &msg);
                        int m_qHandler(std::queue<std::string> &msg);
                        void m_cmdHandler(std::string msg);
                        int m_msgParse(std::string msg, t_msg &p_msg);
                        int m_privMsg(std::string to, std::string msg);
                        int m_cmdDispatcher(t_msg p_msg);
                        int m_internalCmd(std::string msg, t_msg &p_msg);
                        int m_pmCmdHandler(t_msg &p_msg);
                        int m_invCmdHandler(t_msg &p_msg);
                        int m_writeChanneLog(std::string channelName, std::string msgLog);
                        void m_channelSearch(std::string msg);
};

#endif
