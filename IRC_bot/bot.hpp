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

# define END_STRING "\r\n"

//errors codes
# define EC_USAGE 100
# define NOTACMD 101


std::string strToken(std::string str, std::string delim);

#endif
