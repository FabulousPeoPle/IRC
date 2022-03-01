/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ohachim <ohachim@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/08 14:05:23 by azouiten          #+#    #+#             */
/*   Updated: 2022/03/01 11:04:04 by ohachim          ###   ########.fr       */
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

int  Message::checkCommand(char *token)
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
int Message::m_trim(char *str)
{
    char *end = std::strrchr(str, '\r');
    if (end)
        end[0] = '\0';
    int idx = 0;
    while (str[idx] && str[idx] == ' ')
        idx++;
    return (idx);
}

// needs a quick redo // maybe not
void Message::parse(void)
{
    char *token = NULL;
    char * msg = strdup(message.c_str());
    int idx = m_trim(msg);
    char * buff;
    char * literalMessage = ((buff = std::strrchr(msg, ':')) == (msg + idx)) ? NULL : buff;
    
    if (literalMessage)
    {
        _literalMsg = std::string(literalMessage);
        literalMessage[0] = '\0';
    }
    token = strtok(msg, " ,");
    while (token && !checkCommand(token))
        token = strtok(NULL, ", ");
    if (token)
        command = token;
    while (token)
    {
        token = strtok(NULL, ", ");
        if (token)
            arguments.push_back(token);
    }
    free(msg);
}

Message::~Message(void){}
