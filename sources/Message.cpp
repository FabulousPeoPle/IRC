/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azouiten <azouiten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/08 14:05:23 by azouiten          #+#    #+#             */
/*   Updated: 2022/01/11 16:07:25 by azouiten         ###   ########.fr       */
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
    this->message = message;
    // should i call the parse function in here? is that a good thing to do?
}

Message & Message::operator=(Message const & rhs)
{
    // discuss norm to remove one of these
    message = rhs.message;
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

// needs a quick redo
void Message::parse(void)
{
    char *token = NULL;
    char * msg = strdup(message.c_str());
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
