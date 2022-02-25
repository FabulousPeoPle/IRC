/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ohachim <ohachim@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/08 14:05:25 by azouiten          #+#    #+#             */
/*   Updated: 2022/02/22 19:22:48 by ohachim          ###   ########.fr       */
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
    t_strDQeue  _messageQueue;
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
    t_strDQeue  &getMsgQueue(void);
    std::string getMsg(void) const;
    std::string getCmd(void) const;
    std::string getLiteralMsg(void) const;
    std::vector<std::string>    &getArgs(void);
    
    ///////////////
    /// SETTERS ///
    ///////////////
    void        pushMsgInQueue(std::string message);
    void        popMsgFromQueue(std::string message);
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