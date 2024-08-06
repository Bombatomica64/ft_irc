/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 10:31:32 by lmicheli          #+#    #+#             */
/*   Updated: 2024/08/06 10:14:36 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <server.hpp>

void	handle_signal(int sig)
{
	(void)sig;
	throw std::runtime_error("\nClosing server...");
}

int main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cerr << "Usage: " << av[0] << " <port> <password>" << std::endl;
		return 1;
	}
	//CTRL + C
	signal(SIGINT,	handle_signal);
	srand(time(NULL));

	
	try {
		Server server(av[1], av[2]);
		server.accept_connection();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}

	printQrCode();	
}
