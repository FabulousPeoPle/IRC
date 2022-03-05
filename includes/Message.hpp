/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ohachim <ohachim@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/08 14:05:25 by azouiten          #+#    #+#             */
/*   Updated: 2022/03/05 19:45:29 by ohachim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>

#include <deque>
#include <vector>
#include <cstring>

class Client;


std::string strToken(std::string str, std::string delimiterString);

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
    std::string    m_grabLiteralMsg(char *msg);
    char     *m_trim(char *str);
    // int     m_trim(char *str, std::string & literalMessage);
    int     m_checkCommand(char *token);


    template <typename T>
        void    printVector(T &vector, std::string name)
        {
            typename T::iterator it = vector.begin();
            typename T::iterator end = vector.end();
            //std::cout << "printing vector " << name << std::endl;
            while (it != end)
            {
                //std::cout << "|" << *it << "|" << std::endl;
                it++;
            }
        }
    /////////////////
    /// OPERATORS ///
    /////////////////
    Message & operator=(Message const & rhs);
};



#endif