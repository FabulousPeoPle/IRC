/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azouiten <azouiten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/08 14:05:23 by azouiten          #+#    #+#             */
/*   Updated: 2022/02/24 18:05:37 by azouiten         ###   ########.fr       */
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

t_strDQeue  &Message::getMsgQueue(void)
{
    return (_messageQueue);
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

void    Message::pushMsgInQueue(std::string message)
{
    _messageQueue.push_back(message);
}

void    Message::popMsgFromQueue(std::string message)
{
    t_strDQeue::iterator it = _messageQueue.begin();
	t_strDQeue::iterator end = _messageQueue.end();

	while (it != end)
	{
		if (*it == message)
		{
			_messageQueue.erase(it);
			break ;
		}
		it++;
	}
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
void Message::m_trim(char *str)
{
    char *end = std::strchr(str, '\r');
    if (end)
        end[0] = '\0';
}

// needs a quick redo // maybe not
void Message::parse(void)
{
    char *token = NULL;
    char * msg = strdup(message.c_str());
    m_trim(msg);
    char * literalMessage = std::strchr(msg, ':');
    
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
