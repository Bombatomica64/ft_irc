/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 11:59:47 by lmicheli          #+#    #+#             */
/*   Updated: 2024/06/27 10:35:56 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <server.hpp>

Server::Server(char *port, char *psw)
{
	check_input(port, psw);
	m_port = std::atoi(port);
	m_psw = psw;
	create_socket();
}

void Server::create_socket( void )
{
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == -1)
	{
		std::cerr << "Error: socket creation failed" << std::endl;
		exit(1);
	}
	bind_socket();
}

Server::~Server()
{
	close(m_socket);
}

void Server::bind_socket( void )
{
	m_addr.sin_family = AF_INET;
	m_addr.sin_port = htons(m_port);
	m_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(m_socket, (struct sockaddr *)&m_addr, sizeof(m_addr)) == -1)
	{
		std::cerr << "Error: socket binding failed" << std::endl;
		exit(1);
	}

	if (listen(m_socket, 10) == -1)
	{
		std::cerr << "Error: socket listening failed" << std::endl;
		exit(1);
	}

	if (fcntl(m_socket, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "Error: socket non-blocking failed" << std::endl;
		exit(1);
	}
}

void Server::accept_connection()
{
	server_fd.fd = m_socket;
	server_fd.events = POLLIN;
	fds.push_back(server_fd);

	while (true)
	{
		int ret = poll(fds.data(), fds.size(), -1);
		if (ret == -1)
			throw Server::PollException();

		for (size_t i = 0; i < fds.size(); ++i)
		{
			if (fds[i].revents & POLLIN)
			{
				if (fds[i].fd == m_socket)
				{
					struct sockaddr_in client_addr;
					socklen_t client_addr_size = sizeof(client_addr);
					int client_socket = accept(m_socket, (struct sockaddr *)&client_addr, &client_addr_size);
					if (client_socket == -1)
					{
						// Handle errors
						continue;
					}
					if (fcntl(client_socket, F_SETFL, O_NONBLOCK) == -1)
					{
						close(client_socket);
						throw Server::clientSocketException();
					}
					struct pollfd client_fd;
					client_fd.fd = client_socket;
					client_fd.events = POLLIN;
					fds.push_back(client_fd);
				}
				else
				{
					try
					{
						read_from_client(fds[i].fd);
					}
					catch(const std::exception& e)
					{
						close(fds[i].fd);
						std::cerr << e.what() << std::endl;
						continue ;
					}
				}
			}
		}
	}
}

void Server::read_from_client(int client)
{
	char buffer[1024];
	int ret = recv(client, buffer, 1024, 0);
	if (ret == -1)
	{
		throw Server::ClientException();
		return;
	}
	if (ret == 0)
	{
		close(client);
		return;
	}
	std::string msg(buffer, ret);
	std::cout << "Received: " << msg << std::endl;
}