/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strToken.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ohachim <ohachim@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 15:30:49 by ohachim           #+#    #+#             */
/*   Updated: 2022/03/08 15:45:46 by ohachim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <iostream>

std::string strToken(std::string str, std::string delimiterString)
{    
    static std::string  strDup = str;

    if (str.size())
        strDup = str;

    if (!strDup.size())
        return "";
;
    int found = strDup.find(delimiterString);

    if (static_cast<unsigned long>(found) == std::string::npos)
    {
        std::string toReturn = strDup;
        strDup = "";
        return (toReturn);
    }

    std::string toReturn = strDup.substr(0, found + 2);
    
    strDup = strDup.substr(found + 2, std::string::npos);
    return (toReturn);
}
