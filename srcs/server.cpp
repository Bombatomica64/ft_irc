/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mruggier <mruggier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 11:59:47 by lmicheli          #+#    #+#             */
/*   Updated: 2024/07/29 18:27:11 by mruggier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <server.hpp>

Server::Server(char *port, char *psw)
{
	get_cmds();
	check_input(port, psw);
	m_port = std::strtold(port, NULL);
	m_salt = generate_salt(16);
	m_hash = hash_password(psw, m_salt);
	create_socket();
	
	time_t now = time(0);
	struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%a, %d %b %Y at %H:%M:%S", &tstruct);
	m_date = buf;
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
	m_cmds["PRIVMSG"] = &Server::privmsg;
	m_cmds["JOIN"] = &Server::join;
	m_cmds["PART"] = &Server::part;
	m_cmds["MODE"] = &Server::mode;
	m_cmds["INVITE"] = &Server::invite;
	m_cmds["TOPIC"] = &Server::topic;
	m_cmds["KICK"] = &Server::kick;
	m_cmds["QUIT"] = &Server::quit;
	m_cmds["NAMES"] = &Server::names;
	m_cmds["PASS"] = &Server::pass;
	m_cmds["USER"] = &Server::user;
	m_cmds["CAP"] = &Server::cap;
	m_cmds["WHO"] = &Server::who;
	m_cmds["PING"] = &Server::ping;
	m_cmds["USERHOST"] = &Server::userhost;
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
			it->second->send_message("Server shutting down");
			close(it->first);
			delete it->second;
		}
	m_clients.clear();
	m_commands.clear();
	client_fds.clear();
	for (std::map<std::string, Channel *>::iterator it = m_channels.begin(); it != m_channels.end(); ++it)
		delete it->second;
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
	int optval = 1;
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
					setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
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
					m_clients[client_socket] = new Client(client_socket, client_addr);
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
						m_clients.erase(client_fds[i].fd);
						close(client_fds[i].fd);
						client_fds.erase(client_fds.begin() + i);
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

	// int ret = recv(client, buffer, BUFFER_SIZE, 0);
	// if (ret == -1)
	// {
	// 	throw Server::ClientException();
	// 	return;
	// }
	// if (ret == 0)
	// {
	// 	close(client);
	// 	return;
	// }
void Server::read_from_client(int client)
{
	if (m_clients[client]->get_registered() == false || m_clients[client]->get_connected() == false)
	{
		register_client(client);
		return;
	}
	char temp[BUFFER_SIZE] = {0};
	int total_ret;

	std::string msg = "";
	total_ret = recv(client, temp, BUFFER_SIZE, 0);
	std::cerr << total_ret << std::endl;
	if (total_ret == -1)
	{
		std::cout << "Error: mannagia a cristo " << strerror(errno) << std::endl;
		throw Server::ClientException();
	}
	temp[total_ret] = '\0';
	msg.append(temp);
	
	std::cerr << total_ret << std::endl;
	if (total_ret == -1)
	{
		std::cout << "Error: mannagia a cristo " << strerror(errno) << std::endl;
	}
	if (total_ret == 0 || msg == "\n")
		return;
	if (msg.empty())
	{
		std::cerr << "🥍haha, i'm in danger 🚌️🤸️" << std::endl;
		throw Server::ClientException();
		return;
	}
	std::cout << RED "Received: {" << msg.substr(0, msg.size() - 1) << "}"RESET << std::endl; 
	std::vector<std::string> split_msg = split(msg, " ");
	parse_cmds(client, trimString(msg));
	// if (m_commands.find(split_msg[0]) == m_commands.end())
	// {
	// 	write_to_client(client, "Unknown command");
	// 	return;
	// }
	// parse_cmds(client, msg);
	// if (msg == "QUIT") // temporary
	// {
	// 	throw std::runtime_error("close");
	// }
	std::cout << BLUE << " " << msg << RESET << std::endl;
}

void Server::register_client(int client)
{
	char temp[BUFFER_SIZE] = {0};
	int total_ret;

	std::string msg = "";
	total_ret = recv(client, temp, BUFFER_SIZE, 0);
	std::cerr << total_ret << std::endl;
	if (total_ret == -1)
	{
		std::cout << "Error: mannagia a cristo " << strerror(errno) << std::endl;
		throw Server::ClientException();
	}
	msg.append(temp);
	if (msg.empty())
	{
		std::cerr << "haha, i'm in danger 🚌️🤸️" << std::endl;
		//throw Server::ClientException();
		// return;
	}
	std::cout << RED "Received: [" << msg.substr(0, msg.size() - 1) << "]" << RESET << std::endl; // TODO remove
	if (msg == "\n")
	{
		write_to_client(client, "You must send a password first");
		return;
	}
	std::vector<std::string> split_msg = split(msg, " ");
	if (split_msg.size() > 0 && split_msg[0] == "CAP")
	{
		cap(client, msg);
		return;
	}
	switch (m_clients[client]->get_reg_steps())
	{
	case 0:
		if (split_msg[0] == "PASS")
		{
			if (split_msg.size() > 1 && split_msg[1].find(":") == 0)
				split_msg[1].erase(0, 1);
			if (verify_password(split_msg[1], m_hash, m_salt) && split_msg.size() >= 2)
				m_clients[client]->set_reg(1);
			else if (split_msg.size() == 1)
			{
				write_to_client(client, ":irc 461 PASS :Not enough parameters"); //ERR_NEEDMOREPARAMS
			}
			else
			{
				write_to_client(client, "Wrong password");
				// std::cerr << BRIGHT_MAGENTA"|" << split_msg[1] <<"|" RESET << std::endl;
			}
		}
		else
		{
			write_to_client(client, ":irc 461 PASS :Not enough parameters"); //ERR_NEEDMOREPARAMS
		}
		break;
	case 1:
		if (split_msg[0] == "NICK")
		{
			if (split_msg.size() == 2)
			{
				if (get_client_by_nick(split_msg[1]) != NULL || split_msg[1].find("[bot]") != std::string::npos || split_msg[1] == "coucou")
				{
					write_to_client(client,":irc 433 " + split_msg[1] + " :Nickname already in use"); //ERR_NICKNAMEINUSE
					break;
				}
				if (split_msg[1].find(":@#&") != std::string::npos)
				{
					write_to_client(client, ":irc 432 " + split_msg[1] + " :Erroneous nickname"); //ERR_ERRONEUSNICKNAME
					break;
				}
				m_clients[client]->set_nick(split_msg[1]);
				m_clients[client]->set_reg(2);
			}
			else
				write_to_client(client, ":irc 461 NICK :No nickname given"); //ERR_NONICKNAMEGIVEN
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

				m_clients[client]->send_message(":irc 001 " + m_clients[client]->get_nick() + " :Welcome to the Jungle Network, " + m_clients[client]->get_nick() + "!" + m_clients[client]->get_user() + "@" + m_clients[client]->get_hostname());
				m_clients[client]->send_message(":irc 002 " + m_clients[client]->get_nick() + " :Your host is " + m_clients[client]->get_servername() + ", running version 4.2");
				m_clients[client]->send_message(":irc 003 " + m_clients[client]->get_nick() + " :This server was created " + m_date );
				m_clients[client]->send_message(":irc 004 " + m_clients[client]->get_nick() + " " + m_clients[client]->get_servername() + " 4.2 uMode:none cMode:+i,+t,+k,+o");
				m_clients[client]->send_message(":irc 375 " + m_clients[client]->get_nick() + " :- Welcome to the Jungle");
				m_clients[client]->send_message(":irc 372 " + m_clients[client]->get_nick() + " :- ");
				m_clients[client]->send_message(":irc 372 " + m_clients[client]->get_nick() + " :- try not to break anything");
				m_clients[client]->send_message(":irc 372 " + m_clients[client]->get_nick() + " :- ");
				m_clients[client]->send_message(":irc 376 " + m_clients[client]->get_nick() + " :- End of MOTD command");
				
				
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
void Server::write_to_client(int client, std::string msg)
{
	send(client, msg.c_str(), msg.size(), 0);
	send(client, "\r\n", 2, 0);
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

void	Server::add_channel(std::string name)
{
	if (m_channels.find(name) == m_channels.end())
		m_channels[name] = new Channel(name, this);
}


void	Server::add_channel(std::string name, std::map<char, int> modes)
{
	if (m_channels.find(name) == m_channels.end())
		m_channels[name] = new Channel(name, this, modes);
}


void	Server::parse_cmds(int client, std::string cmd)
{
	std::vector<std::string> split_cmd = split(cmd, " ");
	std::cout <<"split_cmd: |" << split_cmd << "|" << std::endl;
	if (m_cmds.find(split_cmd[0]) != m_cmds.end())
	{
		bool ret = (this->*(m_cmds[split_cmd[0]]))(client, cmd);
		if (!ret)
			m_clients[client]->send_message(":irc 421 " + m_clients[client]->get_nick() + " " + split_cmd[0] + " :Unknown command"); //??????
	}
	else if (m_clients[client]->parse_cmds(cmd) == false)
		m_clients[client]->send_message(":irc 421 " + m_clients[client]->get_nick() + " " + split_cmd[0] + " :Unknown command\n");
	else
		m_clients[client]->send_message(":irc 421 " + m_clients[client]->get_nick() + " " + split_cmd[0] + " :Unknown command\n");
}

//:juco!~juco@hostname.com PRIVMSG #channel :Hello world
//:your@hostmask PRIVMSG #channel :some long message
bool	Server::privmsg(int client, std::string message)
{
	size_t start = message.find("PRIVMSG");
	if (start == std::string::npos)
	{
		// handle error
		return false;
	}
	std::string msg = message.substr(start + strlen("PRIVMSG "));
	std::string to_send = ":" + m_clients[client]->get_nick()
		+ "!" + m_clients[client]->get_user() + "@" + m_clients[client]->get_hostname() + " PRIVMSG " + msg.substr(msg.find(" :"));
	
	std::string target_str = msg.substr(0, msg.find(" :"));
	std::vector<std::string> split_targets = split(target_str, ",");
	for (std::vector<std::string>::iterator it = split_targets.begin(); it != split_targets.end(); it++)
	{
		std::string spcific_send;
		switch ((*it)[0])
		{
			// send to channel
		case '#':
		case '&':
			// *it = (*it).substr(1);
			spcific_send = to_send.insert(to_send.find("PRIVMSG ") + strlen("PRIVMSG "), *it);
			if (!this->send_msg_to_channel(client, *it, spcific_send)) //-->below
				return false;
			break;
		default:
			if (*it == "coucou[bot]" || *it == "coucou")
			{
				m_coucou.parse_message(*m_clients[client], to_send.substr(to_send.find(" :") + 1, to_send.size()));
				return true;
			}
			// send to user
			spcific_send = to_send.insert(to_send.find("PRIVMSG ") + strlen("PRIVMSG "), *it);
			if (!this->get_client_by_nick(*it)->send_message(to_send))
				return false;
			break;
		}
	}
	return true;
}

bool	Server::send_msg_to_channel(int client, std::string channel, std::string msg)
{
	Channel *chan = this->get_channel(channel);
	if (!chan)
	{
		m_clients[client]->send_message(":irc 403 " + m_clients[client]->get_nick() + " " + channel + " :No such channel");
		return true;
	}
	std::set<std::string> clients = chan->get_clients();
	if (client == -1)
	{
		for (std::set<std::string>::iterator it = clients.begin(); it != clients.end(); it++)
		{
			if (!this->get_client_by_nick(*it)->send_message(msg))
				std::cout << "error sending message to " << *it << std::endl;
		}
		return true;
	}
	if (!chan->is_client_in(m_clients[client]->get_nick()))
	{
		m_clients[client]->send_message(":irc 442 " + m_clients[client]->get_nick() + " " + channel + " :You're not on that channel");
		return false;
	}
	clients.erase(m_clients[client]->get_nick());
	for (std::set<std::string>::iterator it = clients.begin(); it != clients.end(); it++)
	{
			if (!this->get_client_by_nick(*it)->send_message(msg))
				return false;
	}

	return true;
}


bool	Server::join(int client, std::string channel)
{
	std::vector<std::string> split_msg = split(channel, " ");
	std::vector<std::string> split_channel = split(split_msg[1], ",");
	std::vector<std::string> split_key;
	if (split_msg.size() > 2)
		split_key = split(split_msg[2], ",");
	else
		for (size_t j = 0; j < split_channel.size(); j++)
			split_key.push_back("");
	std::cout << "split_channel: " << split_channel << std::endl;
	std::cout << "split_key: " << split_key << std::endl;
	if (split_msg.size() < 2)
	{
		m_clients[client]->send_message(":irc 461 " + m_clients[client]->get_nick() + " JOIN :Not enough parameters");
		return true;
	}
	for (std::vector<std::string>::iterator it = split_channel.begin(); it != split_channel.end(); it++)
	{
		//se non c'e' un # all'inizio del nome del canale, dai errore
		if ((*it)[0] != '#' && (*it)[0] != '&')
		{
			m_clients[client]->send_message(":irc 403 " + m_clients[client]->get_nick() + " " + *it + " :No such channel");
			return true;
		}
	}


	
	int i = 0;
	for (std::vector<std::string>::iterator it = split_channel.begin(); ; it++)
	{
		if (it == split_channel.end())
		{
			std::cout << GREEN << m_channels << RESET << std::endl;
			return true;
		}
		std::string name = *it;
		if (m_channels.find(name) == m_channels.end())
		{
			std::cout << "creating channel: |" << *it <<"|" << std::endl;
			add_channel(name);
			m_channels[name]->add_client(m_clients[client]->get_nick());
			send_msg_to_channel(-1, name, ":" + m_clients[client]->get_nick() + "!irc@localhost JOIN " + name);
			m_channels[name]->add_op(m_clients[client]->get_nick());
		}
		else
			m_channels[name]->join_channel(*m_clients[client], split_key[i++]);
	}
	// TODO handle error
	return false;
}

bool	Server::part(int client, std::string channels)
{
	std::vector<std::string> split_msg = split(channels, " ");
	std::vector<std::string> split_channel = split(split_msg[1], ",");
	if (split_msg.size() < 2)
	{
		// TODO handle error
		return false;
	}
	for (std::vector<std::string>::iterator it = split_channel.begin(); it != split_channel.end(); it++)
	{
		Channel *chan = this->get_channel(*it);
		if (chan)
			chan->remove_client(m_clients[client]->get_nick());
		else
		{
			// TODO handle error
			return false;
		}
	}
	return true;
}

bool	Server::mode(int client, std::string message)
{
	// std::vector<std::string> l_command;
	// l_command.push_back(message.substr(message.find("MODE") + strlen("MODE"))); // "MODE"
	// message = message.substr(message.find("MODE") + strlen("MODE"));
	// l_command.push_back(message.substr(message.find(" ") + 1)); // "channel"
	// size_t pos = l_command[1].find(" ");
	// if (pos != std::string::npos)
	// 	l_command[1] = l_command[1].substr(0, pos);
	// message = message.substr(message.find(" ") + 1);
	// l_command.push_back(message.substr(message.find(" ") + 1)); // "operation"
	// pos = l_command[2].find(" ");
	// if (pos != std::string::npos)
	// 	l_command[2] = l_command[2].substr(0, pos);
	// message = message.substr(message.find(" ") + 1);
	// if (message.find(" ") != std::string::npos)
	// 	l_command.push_back(message.substr(message.find(" ") + 1)); // "arguments"
	// else
	// 	l_command.push_back("");
	// if (m_channels.find(l_command[1]) == m_channels.end())
	// {
	// 	// TODO
	// 	// std::cout << "channel not found" << std::endl;
	// 	return false;
	// }
	std::vector<std::string> l_command = split(message, " ");
	if (l_command.size() < 2)
	{
		m_clients[client]->send_message(":irc 461 " + m_clients[client]->get_nick() + " MODE :Not enough parameters");
		return true;
	}
	if (l_command.size() == 2)
	{
		Channel *chan = get_channel(l_command[1]);
		if (chan)
			chan->send_modes(*m_clients[client]);
		return true;
	}
	if (l_command.size() < 3)
		l_command.push_back("");
	std::cout << "l_command: " << l_command << "size is "<< l_command.size() << std::endl;
	bool ret = m_channels[l_command[1]]->modify_mode(l_command, *m_clients[client]);
	// TODO handle error
	return ret;
}

bool	Server::invite(int client, std::string message)
{
	std::vector<std::string> split_msg = split(message, " ");
	Channel *chan = get_channel(split_msg[2]);
	if (chan && chan->is_client_in(m_clients[client]->get_nick()))
	{
		// TODO handle error
		return false;
	}
	else if (chan && split_msg.size() == 3)
	{
		chan->add_invite(m_clients[client]->get_nick());
		send_msg_to_channel(-1, chan->get_name(), "INVITE " + split_msg[2] + " " + m_clients[client]->get_nick()); // TODO check if this is correct
		return true;
	}
	// TODO handle error
	return false;
}

bool	Server::quit(int client, std::string message)
{
	std::vector<std::string> split_msg = split(message, " ");
	if (split_msg.size() == 1)
	{
		m_clients[client]->send_message("QUIT :Leaving");
	}
	else
	{
		m_clients[client]->send_message("QUIT :" + message.substr(message.find(" :") + 2));
	}
	for (std::map<std::string, Channel*>::iterator it = m_channels.begin(); it != m_channels.end(); it++)
	{
		if (it->second->is_client_in(m_clients[client]->get_nick()))
			it->second->remove_client(m_clients[client]->get_nick());
	}
	std::vector<struct pollfd>::iterator it = std::find(client_fds.begin(), client_fds.end(), client);
	if (it != client_fds.end())
		client_fds.erase(it);
	close(client);
	delete m_clients[client];
	m_clients.erase(client);
	return true;
}

bool	Server::topic(int client, std::string params)
{
	std::string topic;
	if (params.find(":") != std::string::npos)
	{
		topic = params.substr(params.find(":") + 1);
		params = params.substr(0, params.find(":"));
		std::cout << "topic: " << topic << std::endl;
	}
	std::vector<std::string> split_msg = split(params, " ");
	std::cout << "split_msg_number: " << split_msg.size() << std::endl;
	for (std::vector<std::string>::iterator it = split_msg.begin(); it != split_msg.end(); it++)
		std::cout << "split_msg: " << *it << std::endl;
	std::cout << std::endl;
	if (split_msg.size() < 2) // TOPIC without #chan
	{
		write_to_client(client, ":irc 461 " + m_clients[client]->get_nick() + " TOPIC :Not enough parameters");
		return true;
	}
	Channel *chan = get_channel(split_msg[1]);
	if (!chan)
	{
		m_clients[client]->send_message(":irc 403 " + m_clients[client]->get_nick() + " " + split_msg[1] + " :No such channel");
		return true;
	}
	if (chan->is_client_in(m_clients[client]->get_nick()) == false)
	{
		m_clients[client]->send_message(":irc 442 " + m_clients[client]->get_nick() + " " + split_msg[1] + " :You're not on that channel");
		return true;
	}
	if (split_msg.size() == 2 + (topic.empty() ? 0 : 1))
	{
		chan->send_topic(*m_clients[client]);
	}
	else
	{
		chan->topuc(*m_clients[client], topic);

	}
	return true;
}

bool	Server::names(int client, std::string params)
{
	std::vector<std::string> split_msg = split(params, " ");
	if (split_msg.size() == 1)
	{
		// TODO handle error
		return false;
	}
	std::vector<std::string> split_channels = split(split_msg[1], ",");
	std::string message = "";
	for (std::vector<std::string>::iterator it = split_channels.begin(); it != split_channels.end(); it++)
	{
		Channel *chan = get_channel(*it);
		if (chan)
		{
			std::set<std::string> clients = chan->get_clients();
			message += ":irc 353 " + m_clients[client]->get_nick();
			message += chan->get_name() + " :";
			for(std::set<std::string>::iterator it = clients.begin(); it != clients.end(); ++it)
			{
				if (chan->get_ops().find(*it) != chan->get_ops().end())
					message += "@" + *it + " ";
				else
					message += *it + " ";
			}
			message += "\n";
		}
	}
	if (!message.empty())
	{
		if (client != -1)
			m_clients[client]->send_message(message);
		else
			send_msg_to_channel(client, split_msg[1], message);
		return true;
	}
	return false;
}

bool	Server::pass(int client, std::string message)
{
	std::vector<std::string> split_msg = split(message, " ");
	if (split_msg.size() >= 1)
		write_to_client(client, ":irc 462" + m_clients[client]->get_nick() + " :You may not reregister");
	return true;
}

bool	Server::user(int client, std::string message)
{
	(void)message;
	write_to_client(client, ":irc 462 :You may not reregister");
	return true;
}
bool	Server::kick(int client, std::string message)
{
	std::string kick_msg = "";
	if (message.find(":") != std::string::npos)
	{
		kick_msg = message.substr(message.find(":") + 1);
		message = message.substr(0, message.find(":"));
	}
	std::vector<std::string> split_msg = split(message, " ");
	if (split_msg.size() < 3)
	{
		write_to_client(client, ":irc 461 " + m_clients[client]->get_nick() + " KICK :Not enough parameters");
		return true;
	}
	Channel *chan = get_channel(split_msg[1]);
	if (!chan)
	{
		write_to_client(client, ":irc 403 " + m_clients[client]->get_nick() + " " + split_msg[1] + " :No such channel");
		return true;
	}
	if (!chan->is_client_in(m_clients[client]->get_nick()))
	{
		write_to_client(client, ":irc 442 " + m_clients[client]->get_nick() + " " + split_msg[1] + " :You're not on that channel");
		return true;
	}
	if(!chan->is_op(m_clients[client]->get_nick()))
	{
		write_to_client(client, ":irc 482 " + m_clients[client]->get_nick() + " " + split_msg[1] + " :You're not channel operator");
		return true;
	}
	if (!chan->is_client_in(split_msg[2]))
	{
		if (get_client_by_nick(split_msg[2]) == NULL)
			write_to_client(client, ":irc 401 " + m_clients[client]->get_nick() + " " + split_msg[2] + " :No such nick/channel");
		else
			write_to_client(client, ":irc 441 " + m_clients[client]->get_nick() + " " + split_msg[2] + " " + split_msg[1] + " :They aren't on that channel");
		return true;
	}
	chan->remove_client((split_msg[2]));
	chan->add_ban(split_msg[2]);
	write_to_client(get_client_by_nick(split_msg[1])->get_clientSocket(),":" + m_clients[client]->get_nick() + " KICK " + split_msg[1] + " " + split_msg[2] + " :" + kick_msg);
	send_msg_to_channel(-1, split_msg[1], "KICK " + split_msg[1] + " " + split_msg[2] + " :" + kick_msg);
	return true;
}

bool Server::cap(int client, std::string message)
{
    if (message.find("LS 302") != std::string::npos)
    {
        // Advertise supported capabilities
        write_to_client(client, "CAP * LS :multi-prefix away-notify invite-notify");
        return true;
    }
    else if (message.find("REQ") != std::string::npos)
    {
        // Extract the requested capabilities from the message
        std::string requested_caps = message.substr(message.find("REQ :") + 5);
		std::cout << "requested_caps: |" << requested_caps << "|" << std::endl;
        write_to_client(client, "CAP * ACK :" + requested_caps);
        return true;
    }
	else if (message.find("END") != std::string::npos)
	{
		write_to_client(client, "CAP * ACK :multi-prefix away-notify");
		return true;
	}
    write_to_client(client, "CAP * NAK :multi-prefix away-notify invite-notify");
    return true;
}

// bool	Server::cap(int client, std::string message)
// {
// 	if (message.find("LS 302") != std::string::npos)
// 	{
// 		write_to_client(client, "CAP * LS :multi-prefix away-notify invite-notify");
// 		return true;
// 	}
// 	if (message.find("REQ") != std::string::npos)
// 	{
// 		write_to_client(client, "CAP * ACK :multi-prefix away-notify");
// 		return true;
// 	}
// 	write_to_client(client, "CAP * NAK :multi-prefix away-notify invite-notify");
// 	return true;
// }

bool Server::ping(int client, std::string message)
{
	std::vector<std::string> split_msg = split(message, " ");
	if (split_msg.size() == 2)
	{
		if (m_clients[client]->send_message("PONG " + split_msg[1]))
			return true;
		else
			return false;
	}
	return false;
}

bool	Server::who(int client, std::string message)
{
	std::vector<std::string> split_msg = split(message, " ");
	if (split_msg.size() < 2)
	{
		write_to_client(client, ":irc 431 " + m_clients[client]->get_nick() + " WHO :No nickname given");
		return true;
	}
	if (get_client_by_nick(split_msg[1]) == NULL)
	{
		write_to_client(client, ":irc 401 " + m_clients[client]->get_nick() + " " + split_msg[1] + " :No such nick/channel");
		return true;
	}
	write_to_client(client, ":irc 352 " + m_clients[client]->get_nick() + " " + split_msg[1] + " " + m_clients[client]->get_user() + " " + m_clients[client]->get_hostname() + " " + m_clients[client]->get_servername() + " " + split_msg[1] + " H :0 " + m_clients[client]->get_realname());
	return true;
}

bool	Server::userhost(int client, std::string message)
{
	std::vector<std::string> split_msg = split(message, " ");
	if (split_msg.size() < 2)
	{
		write_to_client(client, ":irc 461 " + m_clients[client]->get_nick() + " USERHOST :Not enough parameters");
		return true;
	}
	std::string response = ":irc 302 " + m_clients[client]->get_nick() + " :";
	for (std::vector<std::string>::iterator it = split_msg.begin() + 1; it != split_msg.end(); it++)
	{
		if (get_client_by_nick(*it) != NULL)
			response += get_client_by_nick(*it)->get_nick() + "=" + get_client_by_nick(*it)->get_nick() + "@" + get_client_by_nick(*it)->get_hostname() + " ";
	}
	write_to_client(client, response);
	return true;

}

bool	Server::is_client_in_channel(std::string channel, std::string client)
{
	Channel *chan = get_channel(channel);
	if (chan)
		return chan->is_client_in(client);
	return false;
}

bool	Server::client_exist(const std::string& client) const
{
	for (std::map<int, Client *>::const_iterator it = m_clients.begin(); it != m_clients.end(); it++)
	{
		if (it->second->get_nick() == client)
			return true;
	}
	return false;
}
// write_to_client(client, ":irc 461 PASS :Not enough parameters"); //ERR_NEEDMOREPARAMS
