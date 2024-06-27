/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 11:59:47 by lmicheli          #+#    #+#             */
/*   Updated: 2024/06/27 17:52:51 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <server.hpp>

Server::Server(char *port, char *psw)
{
	get_cmds();
	check_input(port, psw);
	m_port = std::atoi(port);
	m_psw = psw;
	create_socket();
}

void Server::get_cmds()
{
	m_commands.insert("PASS");
	m_commands.insert("NICK");
	m_commands.insert("USER");
	m_commands.insert("JOIN");
	m_commands.insert("PART");
	m_commands.insert("PRIVMSG");
	m_commands.insert("QUIT");
	m_commands.insert("PING");
	m_commands.insert("PONG");
	m_commands.insert("KICK");
	m_commands.insert("TOPIC");
	m_commands.insert("MODE");
	m_commands.insert("INVITE");
}

Client* Server::find_client(int fd)
{
	for (size_t i = 0; i < m_clients.size(); ++i)
	{
		if (m_clients[i]->get_clientSocket() == fd)
			return m_clients[i];
	}
	return NULL;
}

void Server::create_socket(void)
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

void Server::bind_socket(void)
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
	int			ret;
					struct sockaddr_in client_addr;
	socklen_t	client_addr_size;
	int			client_socket;
					struct pollfd client_fd;

	server_fd.fd = m_socket;
	server_fd.events = POLLIN;
	fds.push_back(server_fd);
	while (true)
	{
		ret = poll(fds.data(), fds.size(), -1);
		if (ret == -1)
			throw Server::PollException();
		for (size_t i = 0; i < fds.size(); ++i)
		{
			if (fds[i].revents & POLLIN)
			{
				if (fds[i].fd == m_socket)
				{
					client_addr_size = sizeof(client_addr);
					client_socket = accept(m_socket,
							(struct sockaddr *)&client_addr, &client_addr_size);
					if (client_socket == -1)
					{
						std::cerr << "Error: accept failed" << std::endl;
						// throw Server::ClientException();
						continue ;
					}
					if (fcntl(client_socket, F_SETFL, O_NONBLOCK) == -1)
					{
						close(client_socket);
						throw Server::clientSocketException();
					}
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
					catch (const std::exception &e)
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
	char buffer[BUFFER_SIZE];
	int ret = recv(client, buffer, BUFFER_SIZE, 0);
	if (ret == -1)
	{
		throw Server::ClientException();
		return ;
	}
	parse_msg(client, buffer, ret);
	if (ret == 0)
	{
		close(client);
		return ;
	}
	std::string msg(buffer, ret);
	std::cout << "Received: " << msg << std::endl;
}