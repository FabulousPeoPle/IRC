/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strToken.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ohachim <ohachim@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 15:30:49 by ohachim           #+#    #+#             */
/*   Updated: 2022/02/22 19:36:15 by melalj           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <iostream>

// Needs refactoring

std::string strToken(std::string str, std::string  delim)
{    
    static std::string  strDup = str;

    if (str.size())
        strDup = str;

    if (!strDup.size())
        return "";

    int found = strDup.find(delim);

    if (static_cast<unsigned long>(found) == std::string::npos)
    {
        std::string toReturn = strDup;
        strDup = "";
        return (toReturn);
    }

    std::string toReturn = strDup.substr(0, found + delim.size() - 1);
    
    strDup = strDup.substr(found + delim.size() - 1, std::string::npos);
    return (toReturn);
}
