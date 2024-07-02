/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 11:59:47 by lmicheli          #+#    #+#             */
/*   Updated: 2024/07/02 18:31:16 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <server.hpp>

Server::Server(char *port, char *psw)
{
	get_cmds();
	check_input(port, psw);
	m_port = std::strtold(port, NULL);
	m_psw = psw;
	create_socket();
}

void Server::get_cmds()
{
	// m_commands.insert("PASS"); 
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
	m_commands.insert("OPER");
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
	for (std::map<int, Client *>::iterator it = m_clients.begin(); it != m_clients.end(); ++it)
	{
		delete it->second;
	}
	m_clients.clear();
	m_commands.clear();
	client_fds.clear();
	m_channels.clear();
}

void Server::bind_socket(void)
{
	int optval = 1;
	setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	m_addr.sin_family = AF_INET;
	m_addr.sin_port = htons(m_port);
	m_addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(m_socket, (struct sockaddr *)&m_addr, sizeof(m_addr)) == -1)
	{
		std::cerr << "Error: socket binding failed - " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	if (listen(m_socket, 10) == -1)
	{
		std::cerr << "Error: socket listening failed - " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	if (fcntl(m_socket, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "Error: setting socket to non-blocking failed - " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
}

void Server::accept_connection()
{
	int ret = 0;
	struct sockaddr_in client_addr = {};
	socklen_t client_addr_size;
	int client_socket;
	struct pollfd client_fd = {};
	server_fd.fd = m_socket;
	server_fd.events = POLLIN;
	client_fds.push_back(server_fd);
	while (true)
	{
		ret = poll(client_fds.data(), client_fds.size(), -1);
		if (ret == -1)
			throw Server::PollException();
		for (size_t i = 0; i < client_fds.size(); ++i)
		{
			if ((client_fds[i].revents & POLLIN))
			{
				if (client_fds[i].fd == m_socket)
				{
					client_addr_size = sizeof(client_addr);
					client_socket = accept(m_socket,
										   (struct sockaddr *)&client_addr, &client_addr_size);
					if (client_socket == -1)
					{
						std::cerr << "Error: accept failed" << std::endl;
						// throw Server::ClientException();
						continue;
					}
					if (fcntl(client_socket, F_SETFL, O_NONBLOCK) == -1)
					{
						close(client_socket);
						throw Server::clientSocketException();
					}
					client_fd.fd = client_socket;
					client_fd.events = POLLIN;
					client_fds.push_back(client_fd);
					m_clients[client_socket] = new Client(client_socket, client_addr, this);
				}
				else
				{
					try
					{
						if (m_clients[client_fds[i].fd]->get_registered() == false || m_clients[client_fds[i].fd]->get_connected() == false)
							register_client(client_fds[i].fd);
						else
							read_from_client(client_fds[i].fd);
					}
					catch (const std::exception &e)
					{
						std::cerr << e.what() << std::endl;
						close(client_fds[i].fd);
						std::string msg(e.what());
						if (msg == "close")
							throw std::runtime_error("close");
						//return;
					}
				}
			}
		}
	}
}

void Server::read_from_client(int client)
{
	if (m_clients[client]->get_registered() == false || m_clients[client]->get_connected() == false)
	{
		register_client(client);
		return;
	}
	char buffer[BUFFER_SIZE] = {0};
	int ret = recv(client, buffer, BUFFER_SIZE, 0);
	if (ret == -1)
	{
		throw Server::ClientException();
		return;
	}
	// parse_msg(client, buffer, ret);
	if (ret == 0)
	{
		close(client);
		return;
	}

	std::string msg(buffer, ret);
	std::vector<std::string> split_msg = split(msg, " ");
	// if (m_commands.find(split_msg[0]) == m_commands.end())
	// {
	// 	write_to_client(client, "Unknown command");
	// 	return;
	// }
	// parse_cmds(client, msg);
	if (msg == "QUIT") // temporary
	{
		throw std::runtime_error("close");
	}
	std::cout << BLUE << *m_clients[client] << " " << msg << RESET << std::endl;
}

void Server::register_client(int client)
{
	std::string msg;
	char temp[BUFFER_SIZE];
	int ret;

	while ((ret = recv(client, temp, BUFFER_SIZE, 0)) > 0)
	{
		msg.append(temp, ret);
		if (msg.find("\r\n") != std::string::npos) // check docs for \r\n
			break;
	}
	std::cerr << ret << std::endl;
	if (msg.empty())
	{
		std::cerr << "haha, i'm in danger 🚌️🤸️" << std::endl;
		throw Server::ClientException();
		return;
	}
	/*if (ret == -1)
	{
		std::cout << "Error: mannagia a cristo" << std::endl;
		return;
	}*/
	std::cerr << RED "Received: " << msg << RESET << std::endl; // TODO remove
	std::vector<std::string> split_msg = split(msg, " ");
	switch (m_clients[client]->get_reg_steps())
	{
	case 0:
		if (split_msg[0] == "PASS")
		{
			if (split_msg[1] == m_psw && split_msg.size() == 2)
				m_clients[client]->set_reg(1);
			else
			{
				write_to_client(client, "Wrong password");
				
			}
		}
		else
		{
			write_to_client(client, "You must send a password first");
		}
		break;
	case 1:
		if (split_msg[0] == "NICK")
		{
			if (split_msg.size() == 2)
			{
				m_clients[client]->set_nick(split_msg[1]);
				m_clients[client]->set_reg(2);
			}
			else
				write_to_client(client, "You must send a nickname first");
		}
		else
			write_to_client(client, "You must send a nickname first");
		break;
	case 2:
				if (split_msg[0] == "USER")
		{
			if (split_msg.size() >= 5)
			{
				
				std::string realname;
				size_t i = 4;
				while (i < split_msg.size() && split_msg[i].find(":") != 0)
					i++;
				if (i == split_msg.size())
				{
					m_clients[client]->send_message("USER :Not enough parameters");
				}
				while (i < split_msg.size())
				{
					if (split_msg[i].find(":") == 0)
						split_msg[i].erase(0, 1);
					
					realname.append(split_msg[i]);
					if (i != split_msg.size() - 1)
						realname.append(" ");
					i++;
				}
				m_clients[client]->set_user(split_msg[1]);
				m_clients[client]->set_hostname(split_msg[2]);
				m_clients[client]->set_servername(split_msg[3]);
				m_clients[client]->set_realname(realname);
				m_clients[client]->set_reg(3);
				m_clients[client]->set_connected(true);
				m_clients[client]->set_registered(true);
			}
			else
				write_to_client(client, "You must send a username first");
			break;
		}
	default:
		std::cerr << " how do you get here? " << std::endl;
		close(client);
		exit(1);
		break;
	}
}

void	Server::parse_cmds(int client, std::string cmd)
{
	if (!cmd.empty() && m_clients.find(client) != m_clients.end())
		m_clients[client]->parse_cmds(cmd);
}
// std::cout << "Received: " << msg << std::endl;
// if (msg.find("PASS") != std::string::npos)
// {
// 	if (msg.find(m_psw) != std::string::npos)
// 	{
// 		write_to_client(client, "Welcome to the server");
// 		m_clients.push_back(new Client(client, m_addr));
// 		m_clients.back()->connect_to_server();
// 		m_clients.back()->send_message();
// 		m_clients.back()->receive_message();
// 		m_clients.back()->connect_to_channel();
// 		m_clients.back()->quit();
// 	}
// 	else
// 	{
// 		write_to_client(client, "Wrong password");
// 		close(client);
// 	}
// }
// else
// {
// 	write_to_client(client, "You must send a password first");
// 	close(client);
// }

void Server::write_to_client(int client, std::string msg)
{
	send(client, msg.c_str(), msg.size(), 0); // possibly add MSG_NOSIGNAL with errno TODO
}

Client*		Server::get_client_by_nick(std::string nick)
{
	for (std::map<int, Client*>::iterator it = m_clients.begin(); it != m_clients.end(); ++it)
	{
		if (it->second->get_nick() == nick)
			return it->second;
	}
	return NULL;
}

Channel*	Server::get_channel(std::string name)
{
	if (m_channels.find(name) != m_channels.end())
		return m_channels[name];
	return NULL;
}