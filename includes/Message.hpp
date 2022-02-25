/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azouiten <azouiten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/08 14:05:25 by azouiten          #+#    #+#             */
/*   Updated: 2022/02/25 19:56:27 by azouiten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>

#include <deque>
#include <vector>
#include <cstring>

class Client;

std::string strToken(std::string str);

typedef std::deque<std::string> t_strDQeue;

class Message
{
private:
    std::string message;
    std::string command;
    std::string _literalMsg;
    std::vector<std::string> arguments;

public:
    
    Message(void);
    Message(Message const & src);
    Message(std::string message) {this->message = message;};
    ~Message(void);

    ///////////////
    /// GETTERS ///
    ///////////////
    std::string getMsg(void) const;
    std::string getCmd(void) const;
    std::string getLiteralMsg(void) const;
    std::vector<std::string>    &getArgs(void);
    
    ///////////////
    /// SETTERS ///
    ///////////////
    void        setMsg(std::string message);

    ///////////////
    /// METHODS ///
    ///////////////
    void    parse(void);
    void    m_trim(char *str);
    int     checkCommand(char *token);

    /////////////////
    /// OPERATORS ///
    /////////////////
    Message & operator=(Message const & rhs);
};



#endif