/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ohachim <ohachim@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/08 14:05:23 by azouiten          #+#    #+#             */
/*   Updated: 2021/12/18 10:58:56 by ohachim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"

Message::Message(void)
{}

Message::Message(Message const & src)
{
    if (&src != this)
        *this = src;
}

Message::Message(std::string message, Client & client) : _client(&client)
{
    _message = (char *)message.c_str();
    // should i call the parse function in here? is that a good thing to do?
}

Message & Message::operator=(Message const & rhs)
{
    _message = rhs._message; //allocate new string?
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
    // char * msg = strdup(_message);
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