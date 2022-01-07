/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azouiten <azouiten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/08 14:05:25 by azouiten          #+#    #+#             */
/*   Updated: 2022/01/07 18:33:28 by azouiten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>

#include <deque>
#include <vector>

class Client;

std::string strToken(std::string str);

typedef std::deque<std::string> t_strDQeue;

#define USER_COMMAND "USER"
#define NICK_COMMAND "NICK"
#define PASS_COMMAND "PASS"

class Message
{
private:
    std::string _message;

public:
    t_strDQeue  _messageQueue;
    Client      *_client;
    // bool        _completed;
    std::string command;
    std::string _literalMsg;
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