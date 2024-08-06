/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 10:56:04 by lmicheli          #+#    #+#             */
/*   Updated: 2024/08/06 10:14:38 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <main.hpp>

void	check_input(char *port, char *psw)
{
	if (port == NULL || psw == NULL)
	{
		std::cerr << "Usage: <host> <port>" << std::endl;
		exit(1);
	}
	std::string portstr(port);
	if (portstr.find_first_not_of("0123456789") != std::string::npos)
	{
		std::cerr << "Invalid port number" << std::endl;
		exit(1);
	}
	double portnm = std::strtod(port, NULL);
	if (portnm < 0 || portnm > 65535)
	{
		std::cerr << "Invalid port number" << std::endl;
		exit(1);
	}
}
