/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 16:10:18 by lmicheli          #+#    #+#             */
/*   Updated: 2024/09/13 12:37:43 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cuocuo.hpp>


void signal_handler(int signum)
{
	std::cout << "\nCaught signal " << signum << std::endl;
	throw std::runtime_error("Bot closed");
}

int main(int ac, char **av)
{
	if (ac != 4)
	{
		std::cerr << "Usage: <host> <port> <password>" << std::endl;
		exit(1);
	}

	signal(SIGINT, signal_handler);
	try
	{
		if(inet_addr(av[1]) == INADDR_NONE)
			throw std::invalid_argument("Invalid address/Address not supported.");
		check_input(av[2], av[3]);
		std::string info[3] = {av[1], av[2], av[3]};
		Coucou coucou(info);
		std::cout << "Bye bye from Coucou!" << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return 0;
}
