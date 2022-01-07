/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ohachim <ohachim@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/08 14:05:23 by azouiten          #+#    #+#             */
/*   Updated: 2022/01/07 18:11:14 by ohachim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"

Message::Message(void)
{
    this->message = "";
}

Message::Message(Message const & src)
{
    if (&src != this)
        *this = src;
}

Message::Message(std::string message, Client & client) : _client(&client)
{
    _messageQueue.push_back(message);
    // should i call the parse function in here? is that a good thing to do?
}

Message & Message::operator=(Message const & rhs)
{
    _messageQueue = rhs._messageQueue; //allocate new string?
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
    // char *token = NULL;

    // // fetch the command while skipping if the prefix if present
    // char * msg = strdup(_messageQueue);
    // token = strtok(msg, " ");
    // while (token && !checkCommand(token))
    //     token = strtok(NULL, " ");
    // command = token;
    
    // std::cout << "command : " << command << std::endl;

    // // take the following arguments
    // while (token)
    // {
    //     token = strtok(NULL, " ");
    //     if (token)
    //     {
    //         arguments.push_back(token);
    //         std::cout << "argument : " << token << std::endl;
    //     }
    // }
}

Message::~Message(void){}
