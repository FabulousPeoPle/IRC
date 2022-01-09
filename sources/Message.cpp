/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ohachim <ohachim@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/08 14:05:23 by azouiten          #+#    #+#             */
/*   Updated: 2022/01/07 19:40:56 by ohachim          ###   ########.fr       */
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

Message::Message(std::string message, Client & client) : _client(&client), _literalMsg("")
{
    // _messageQueue.push_back(message);
    _message = message;
    // should i call the parse function in here? is that a good thing to do?
}

Message & Message::operator=(Message const & rhs)
{
    _message = rhs._message;
    command = rhs.command;
    arguments = rhs.arguments;
    return (*this);
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

void Message::parse(void)
{
    char *token = NULL;

    // fetch the command while skipping if the prefix if present
    char * msg = strdup(_message.c_str());
    char * literalMessage = std::strchr(msg, ':');
    if (literalMessage)
    {
        _literalMsg = std::string(literalMessage);
        literalMessage[0] = '\0';
    }
    token = strtok(msg, " ");
    while (token && !checkCommand(token))
        token = strtok(NULL, " ");
    if (token)
        command = token;
    while (token)
    {
        token = strtok(NULL, " ");
        if (token)
            arguments.push_back(token);
    }
    free(msg);
}

Message::~Message(void){}
