/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mruggier <mruggier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 10:31:32 by lmicheli          #+#    #+#             */
/*   Updated: 2024/07/30 16:34:04 by mruggier         ###   ########.fr       */
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

	
	Server server(av[1], av[2]);
	try
	{
		server.accept_connection();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
}