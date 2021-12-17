/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azouiten <azouiten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/08 15:38:41 by azouiten          #+#    #+#             */
/*   Updated: 2021/12/15 20:54:30 by azouiten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCMessage.hpp"

int main(void)
{
	Message blob(" USER blob        the fish oblique order");
	blob.parse();

  return 0;
}