/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCMessage.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azouiten <azouiten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/08 14:05:23 by azouiten          #+#    #+#             */
/*   Updated: 2021/12/10 18:28:53 by azouiten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCMessage.hpp"

IRCMessage::IRCMessage(void){}

IRCMessage::IRCMessage(IRCMessage const & src)
{
    if (&src != this)
        *this = src;
}

IRCMessage::IRCMessage(std::string message)
{
    _message = (char *)message.c_str();
    // should i call the parse function in here? is that a good thing to do?
}

IRCMessage & IRCMessage::operator=(IRCMessage const & rhs)
{
    _message = rhs._message; //allocate new string?
    command = rhs.command;
    arguments = rhs.arguments;
    return (*this);
}

int  IRCMessage::checkCommand(char *token)
{
    int index = 0;
    
    while (token[index])
    {
        if (token[index] < 'A' || token[index] > 'Z')
            return (0);
        index++;
    }
    return (1);
}

void IRCMessage::parse(void)
{
    char *token = NULL;

    // fetch the command while skipping if the prefix if present
    char * msg = strdup(_message);
    token = strtok(msg, " ");
    while (token && !checkCommand(token))
        token = strtok(NULL, " ");
    command = token;
    
    std::cout << "command : " << command << std::endl;

    // take the following arguments
    while (token)
    {
        token = strtok(NULL, " ");
        if (token)
        {
            arguments.push_back(token);
            std::cout << "argument : " << token << std::endl;
        }
    }
}

void IRCMessage::relay(void)
{
    //simple relay function relaying each comman with the apropriat function
    // if (command == USER_COMMAND){}
    // else if ()
}

IRCMessage::~IRCMessage(void){}