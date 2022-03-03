/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azouiten <azouiten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/08 14:05:23 by azouiten          #+#    #+#             */
/*   Updated: 2022/03/03 13:19:34 by azouiten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Message.hpp"

Message::Message(void): _literalMsg("")
{}

Message::Message(Message const & src): _literalMsg("")
{
    if (&src != this)
        *this = src;
}

Message & Message::operator=(Message const & rhs)
{
    // discuss norm to remove one of these
    message = rhs.message;
    command = rhs.command;
    arguments = rhs.arguments;
    return (*this);
}


std::string Message::getMsg(void) const
{
    return (message);
}

std::string Message::getCmd(void) const
{
    return (command);
}

std::string Message::getLiteralMsg(void) const
{
    return (_literalMsg);
}

std::vector<std::string>    &Message::getArgs(void)
{
    return (arguments);
}

void    Message::setMsg(std::string message)
{
    this->message = message;
}

int  Message::m_checkCommand(char *token)
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
// also needs a redo

std::string Message::m_grabLiteralMsg(char *msg)
{
    char * literalMsgPtr = strchr(msg, ':');
    if (!literalMsgPtr)
        return (std::string(""));
    std::string returnValue = std::string(literalMsgPtr);
    literalMsgPtr[0] = '\0';
    return (returnValue);
}

char *Message::m_trim(char *str)
{
    int idx = 0;
    
    while (str[idx] && (str[idx] == ' ' || str[idx] == '\t' || str[idx] == ':'))
        idx++;
    return (str + idx);
}

void    Message::parse(void)
{
    char * msgbuffer = strdup(message.c_str());
    char * msg = m_trim(msgbuffer);
    _literalMsg = m_grabLiteralMsg(msg);
    char *token = NULL;
    
    token = strtok(msg, " ,\t\r\n");
    while (token && !m_checkCommand(token))
        token = strtok(NULL, ", \t\r\n");
    if (token)
        command = token;
    while (token)
    {
        token = strtok(NULL, ", \t\r\n");
        if (token)
            arguments.push_back(token);
    }
    std::cout << "|" << command << "|" << std::endl;
    printVector(arguments, " arguments ");
    std::cout << _literalMsg << std::endl;
    free(msgbuffer);
}

Message::~Message(void){}
