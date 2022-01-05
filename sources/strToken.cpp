/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strToken.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ohachim <ohachim@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 15:30:49 by ohachim           #+#    #+#             */
/*   Updated: 2022/01/05 18:03:40 by ohachim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <iostream>

// Needs refactoring

std::string strToken(std::string str)
{    
    static std::string  strDup = str;

    if (str.size())
        strDup = str;

    if (!strDup.size())
        return "";

    int found = strDup.find("\n");

    if (static_cast<unsigned long>(found) == std::string::npos)
    {
        std::string toReturn = strDup;
        strDup = "";
        return (toReturn);
    }

    std::string toReturn = strDup.substr(0, found + 1);
    
    strDup = strDup.substr(found + 1, std::string::npos);
    return (toReturn);
}
