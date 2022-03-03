/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   intToString.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ohachim <ohachim@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 11:42:47 by ohachim           #+#    #+#             */
/*   Updated: 2022/03/03 11:47:28 by ohachim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>

std::string intToString(int num)
{
    std::stringstream ss;

    ss << num;
    return (ss.str());
}