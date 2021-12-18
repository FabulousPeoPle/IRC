/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ohachim <ohachim@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/08 14:05:25 by azouiten          #+#    #+#             */
/*   Updated: 2021/12/18 10:58:19 by ohachim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>
#include <vector>

class Client;

#define USER_COMMAND "USER"
#define NICK_COMMAND "NICK"
#define PASS_COMMAND "PASS"

class Message
{
public:
    std::string _message;
    Client      *_client;
    // bool        _completed;
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