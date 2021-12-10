/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azouiten <azouiten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/08 15:38:41 by azouiten          #+#    #+#             */
/*   Updated: 2021/12/10 17:04:52 by azouiten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCMessage.hpp"

int main(void)
{
	IRCMessage blob(" USER blob        the fish oblique order");
	blob.parse();
	blob.relay();

  return 0;
}