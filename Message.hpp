/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azouiten <azouiten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/08 14:05:25 by azouiten          #+#    #+#             */
/*   Updated: 2021/12/17 13:28:31 by azouiten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>
#include <vector>
#include "Client.hpp"

#define USER_COMMAND "USER"
#define NICK_COMMAND "NICK"
#define PASS_COMMAND "PASS"

class Message
{
public:
    char *_message;
    Client * _client;
    bool     _completed;
    std::string command;
    std::vector<std::string> arguments;
    // client class is still under construction
    
    Message(void);
    Message(Message const & src);
    Message(std::string message, Client & client);
    ~Message(void);

    Message & operator=(Message const & rhs);
    void parse(void);
    int  checkCommand(char *token);
};

#endif