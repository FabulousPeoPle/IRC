/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCMessage.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azouiten <azouiten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/08 14:05:25 by azouiten          #+#    #+#             */
/*   Updated: 2021/12/10 19:01:39 by azouiten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <vector>

#define USER_COMMAND "USER"
#define NICK_COMMAND "NICK"
#define PASS_COMMAND "PASS"

class IRCMessage
{
private:
    char *_message;
public:
    std::string command;
    std::vector<std::string> arguments;
    // client class is still under construction
    
    IRCMessage(void);
    IRCMessage(IRCMessage const & src);
    IRCMessage(std::string message);
    ~IRCMessage(void);

    IRCMessage & operator=(IRCMessage const & rhs);
    void parse(void);
    int  checkCommand(char *token);
};