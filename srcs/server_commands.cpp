/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_commands.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mruggier <mruggier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 11:59:47 by lmicheli          #+#    #+#             */
/*   Updated: 2024/08/23 16:32:45 by mruggier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <server.hpp>

void Server::get_cmds()
{
	m_cmds["NICK"] = &Server::nick;
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

void Server::write_to_client(int client, std::string msg)
{
	msg.append("\r\n");
	send(client, msg.c_str(), msg.size(), 0);
}

Client *Server::get_client_by_nick(std::string nick)
{
	for (std::map<int, Client *>::iterator it = m_clients.begin(); it != m_clients.end(); ++it)
	{
		if (it->second->get_nick() == nick)
			return it->second;
	}
	return NULL;
}

Channel *Server::get_channel(std::string name)
{
	if (m_channels.find(name) != m_channels.end())
		return m_channels[name];
	return NULL;
}

void Server::add_channel(std::string name)
{
	if (m_channels.find(name) == m_channels.end())
		m_channels[name] = new Channel(name, this);
}

void Server::add_channel(std::string name, std::map<char, int> modes)
{
	if (m_channels.find(name) == m_channels.end())
		m_channels[name] = new Channel(name, this, modes);
}

void Server::parse_cmds(int client, std::string cmd)
{
	std::vector<std::string> split_cmd = split(cmd, " ");
	std::cout << "split_cmd: |" << split_cmd << "|" << std::endl;
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

//: juco!~juco@hostname.com PRIVMSG #channel :Hello world
//: your@hostmask PRIVMSG #channel :some long message

bool Server::privmsg(int client, std::string message)
{
	size_t start = message.find("PRIVMSG");
	bool flag = false;
	for (std::string::iterator it = message.begin() + start; it != message.end(); it++)
	{
		if (*it != ' ' || (*it > 6 && *it < 14))
		{
			flag = true;
			break;
		}
	}
	if (start == std::string::npos && !flag)
	{
		m_clients[client]->send_message(":irc 411 " + m_clients[client]->get_nick() + " :No recipient given (PRIVMSG)");
		return true;
	}
	std::string msg = message.substr(start + strlen("PRIVMSG "));

	if (msg.find(" :") == std::string::npos)
	{
		m_clients[client]->send_message(":irc 412 " + m_clients[client]->get_nick() + " :No text to send");
		return true;
	}

	std::string to_send = ":" + m_clients[client]->get_nick() + "!" + m_clients[client]->get_user() + "@" + m_clients[client]->get_hostname() + " PRIVMSG " + msg.substr(msg.find(" :"));

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
			spcific_send = to_send.insert(to_send.find("PRIVMSG ") + strlen("PRIVMSG "), *it);
			std::cout << "spcific_send: " << spcific_send << std::endl;
			if (!this->send_msg_to_channel(client, *it, spcific_send)) //-->below
				return false;
			break;
		default:
			if (*it == "coucou[bot]" || *it == "coucou")
			{
				m_coucou.parse_message(*m_clients[client], to_send.substr(to_send.find(" :") + 2, to_send.size()));
				return true;
			}
			// send to user
			spcific_send = to_send.insert(to_send.find("PRIVMSG ") + strlen("PRIVMSG "), *it);
			std::cout << "spcific_send: " << spcific_send << std::endl;
			if (spcific_send.find(" :DCC") == std::string::npos)
			{
				if (!this->get_client_by_nick(*it)->send_message(to_send))
					return false;
			}
			else
			{
				std::cout << "DCC" << std::endl;
				handle_file_request(client, spcific_send, *it);
			}
			break;
			spcific_send = "";
		}
	}
	return true;
}

bool Server::send_msg_to_channel(int client, std::string channel, std::string msg)
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

bool Server::join(int client, std::string channel)
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
		// se non c'e' un # all'inizio del nome del canale, dai errore
		if ((*it)[0] != '#' && (*it)[0] != '&')
		{
			m_clients[client]->send_message(":irc 403 " + m_clients[client]->get_nick() + " " + *it + " :No such channel");
			return true;
		}
	}

	int i = 0;
	for (std::vector<std::string>::iterator it = split_channel.begin();; it++)
	{
		if (it == split_channel.end())
		{
			std::cout << GREEN << m_channels << RESET << std::endl;
			return true;
		}
		std::string name = *it;
		if (m_channels.find(name) == m_channels.end())
		{
			std::cout << "creating channel: |" << *it << "|" << std::endl;
			add_channel(name);
			m_channels[name]->add_client(m_clients[client]->get_nick());
			send_msg_to_channel(-1, name, ":" + m_clients[client]->get_nick() + "!irc@" + inet_ntoa(m_addr.sin_addr) + " JOIN " + name);

			m_channels[name]->add_op(m_clients[client]->get_nick());
		}
		else
			m_channels[name]->join_channel(*m_clients[client], split_key[i++]);
	}
	return false;
}

bool Server::part(int client, std::string channels)
{
	std::vector<std::string> split_msg = split(channels, " ");
	if (split_msg.size() < 2)
	{
		m_clients[client]->send_message(":irc 461 " + m_clients[client]->get_nick() + " PART :Not enough parameters");
		return true;
	}
	else if (split_msg.size() > 2)
	{
		m_clients[client]->send_message(":irc 400 " + m_clients[client]->get_nick() + " PART :Too many parameters");
		return true;
	}
	std::vector<std::string> split_channel = split(split_msg[1], ",");
	for (std::vector<std::string>::iterator it = split_channel.begin(); it != split_channel.end(); it++)
	{
		Channel *chan = this->get_channel(*it);
		if (chan && chan->is_client_in(m_clients[client]->get_nick()))
		{
			chan->remove_client(m_clients[client]->get_nick());
		}
		else if (chan && chan->is_client_in(m_clients[client]->get_nick()) == false)
		{
			m_clients[client]->send_message(":irc 442 " + m_clients[client]->get_nick() + " " + *it + " :You're not on that channel");
			return true;
		}
		else
		{
			m_clients[client]->send_message(":irc 403 " + m_clients[client]->get_nick() + " " + *it + " :No such channel");
			return true;
		}
	}
	return true;
}

bool Server::mode(int client, std::string message)
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
	std::cout << "l_command: " << l_command << "size is " << l_command.size() << std::endl;
	bool ret = m_channels[l_command[1]]->modify_mode(l_command, *m_clients[client]);
	// TODO handle error
	return ret;
}

//TODO add away reply
bool Server::invite(int client, std::string message)
{
	std::vector<std::string> split_msg = split(message, " ");
	Channel *chan = get_channel(split_msg[2]);
	if (split_msg.size() < 3)
	{
		m_clients[client]->send_message(":irc 461 " + m_clients[client]->get_nick() + " INVITE :Not enough parameters");
		return true;
	}
	if (!chan)
	{
		m_clients[client]->send_message(":irc 403 " + m_clients[client]->get_nick() + " " + split_msg[2] + " :No such channel");
		return true;
	}
	if (!get_client_by_nick(split_msg[1]))
	{
		m_clients[client]->send_message(":irc 401 " + m_clients[client]->get_nick() + " " + split_msg[1] + " :No such nick/channel");
		return true;
	}
	if (!chan->is_client_in(m_clients[client]->get_nick()))
	{
		m_clients[client]->send_message(":irc 442 " + m_clients[client]->get_nick() + " " + split_msg[2] + " :You're not on that channel");
		return true;
	}
	if (chan->is_client_in(split_msg[1]))
	{
		m_clients[client]->send_message(":irc 443 " + m_clients[client]->get_nick() + " " + split_msg[1] + " " + split_msg[2] + " :is already on channel");
		return true;
	}
	if (m_channels[split_msg[2]]->is_op(m_clients[client]->get_nick()) == false && m_channels[split_msg[2]]->is_ban(split_msg[1]) == true)
	{
		m_clients[client]->send_message(":irc 482 " + m_clients[client]->get_nick() + " " + split_msg[2] + " :You're not channel operator");
		return true;
	}
	if (split_msg.size() == 3)
	{
		chan->add_invite(split_msg[1]);
		get_client_by_nick(split_msg[1])->send_message(":" + m_clients[client]->get_nick() + " 341 " + split_msg[1] + " " + split_msg[2]);
		// send_msg_to_channel(-1, chan->get_name(), "INVITE " + split_msg[2] + " " + m_clients[client]->get_nick());
		return true;
	}

	return false;
}

bool Server::quit(int client, std::string message)
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
	for (std::map<std::string, Channel *>::iterator it = m_channels.begin(); it != m_channels.end(); it++)
	{
		if (it->second->is_client_in(m_clients[client]->get_nick()))
			it->second->remove_client(m_clients[client]->get_nick());
	}
	std::vector<struct pollfd>::iterator it = std::find(m_client_fds.begin(), m_client_fds.end(), client);
	if (it != m_client_fds.end())
		m_client_fds.erase(it);
	close(client);
	delete m_clients[client];
	m_clients.erase(client);
	return true;
}

bool Server::topic(int client, std::string params)
{
	std::string topic = "";
	if (params.find(":") != std::string::npos)
	{
		topic = params.substr(params.find(":") + 1);
		params = params.substr(0, params.find(":"));
		std::cout << "topic: " << topic << std::endl;
	}
	std::vector<std::string> split_msg = split(params, " ");
	
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

std::string Server::getNamesMessage(Channel* chan, int client, std::set<std::string>& client_names)
{
	std::set<std::string> clients = chan->get_clients();
	std::string message = ":irc 353 " + m_clients[client]->get_nick();
	message += " " + chan->get_name() + " :";
	for (std::set<std::string>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (chan->get_ops().find(*it) != chan->get_ops().end())
			message += "@" + *it + " ";
		else
			message += *it + " ";
		std::cout << "client_names: " << *it ;
		if (client_names.find(*it) != client_names.end())
			{client_names.erase(*it); 
			std::cout << " found" << std::endl;}
	}
	return message;
}

bool Server::names(int client, std::string params)
{
	std::vector<std::string> split_msg = split(params, " ");
	//std::string message = "";
	std::set<std::string> client_names;
	std::cout << "e' grossso cosi: " << split_msg.size() << std::endl;
	if (split_msg.size() == 1)
	{
		for (std::map<int, Client *>::iterator it = m_clients.begin(); it != m_clients.end(); it++)
		{
			client_names.insert(it->second->get_nick());
		}
		for (std::map<std::string, Channel *>::iterator it = m_channels.begin(); it != m_channels.end(); it++)
		{
			m_clients[client]->send_message(getNamesMessage(it->second, client, client_names));
		}
		if (!client_names.empty())
		{
			std::string message = ":irc 353 " + m_clients[client]->get_nick() + " *" + " :";
			for (std::set<std::string>::iterator it = client_names.begin(); it != client_names.end(); it++)
			{
				message += *it + " ";
			}
			m_clients[client]->send_message(message);
		}
		m_clients[client]->send_message("ciao");
		m_clients[client]->send_message(":irc 366 " + m_clients[client]->get_nick() + " * :End of /NAMES list");
	}
	else
	{
		std::vector<std::string> split_channels = split(split_msg[1], ",");
		for (std::vector<std::string>::iterator it = split_channels.begin(); it != split_channels.end(); it++)
		{
			Channel *chan = get_channel(*it); 
			if (chan)
				m_clients[client]->send_message(getNamesMessage(chan, client, client_names));
			else
				m_clients[client]->send_message(":irc 403 " + m_clients[client]->get_nick() + " " + *it + " :No such channel");
		}
		m_clients[client]->send_message(":irc 366 " + m_clients[client]->get_nick() + " * :End of /NAMES list");
	}
	// if (!message.empty())
	// {
	// 	if (client != -1)
	// 		m_clients[client]->send_message(message);
	// 	else
	// 		send_msg_to_channel(client, split_msg[1], message);
	// 	return true;
	// }
	return true;
}

bool Server::pass(int client, std::string message)
{
	std::vector<std::string> split_msg = split(message, " ");
	if (split_msg.size() >= 1)
		write_to_client(client, ":irc 462" + m_clients[client]->get_nick() + " :You may not reregister");
	return true;
}

bool Server::user(int client, std::string message)
{
	(void)message;
	write_to_client(client, ":irc 462 :You may not reregister");
	return true;
}

bool Server::kick(int client, std::string message)
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
	if (!chan->is_op(m_clients[client]->get_nick()))
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
	write_to_client(get_client_by_nick(split_msg[1])->get_clientSocket(), ":" + m_clients[client]->get_nick() + " KICK " + split_msg[1] + " " + split_msg[2] + " :" + kick_msg);
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

bool Server::who(int client, std::string message)
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

bool Server::userhost(int client, std::string message)
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

bool Server::nick(int client, std::string message)
{
	std::vector<std::string> split_msg = split(message, " ");
	if (split_msg.size() < 2)
	{
		write_to_client(client, ":irc 431 " + m_clients[client]->get_nick() + " NICK :No nickname given");
		return true;
	}
	if (get_client_by_nick(split_msg[1]) != NULL)
	{
		write_to_client(client, ":irc 433 " + m_clients[client]->get_nick() + " " + split_msg[1] + " :Nickname is already in use");
		return true;
	}
	if (split_msg[1].find(":@#&") != std::string::npos)
	{
		write_to_client(client, ":irc 432 " + m_clients[client]->get_nick() + " " + split_msg[1] + " :Erroneous nickname");
		return true;
	}
	m_clients[client]->set_nick(split_msg[1]);
	m_clients[client]->send_message(":" + m_clients[client]->get_nick() + " NICK " + split_msg[1]);
	return true;
}

bool Server::is_client_in_channel(std::string channel, std::string client)
{
	Channel *chan = get_channel(channel);
	if (chan)
		return chan->is_client_in(client);
	return false;
}

bool Server::client_exist(const std::string &client) const
{
	for (std::map<int, Client *>::const_iterator it = m_clients.begin(); it != m_clients.end(); it++)
	{
		if (it->second->get_nick() == client)
			return true;
	}
	return false;
}

void	Server::remove_channel(std::string name)
{
	if (m_channels.find(name) != m_channels.end())
	{
		delete m_channels[name];
		m_channels.erase(name);
	}
}
void	Server::send_msg_to_set(std::set<std::string> clients, std::string msg)
{
	for (std::set<std::string>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		if (get_client_by_nick(*it))
			get_client_by_nick(*it)->send_message(msg);
	}
}

int	Server::get_port() const
{
	return m_port;
}

std::string	Server::get_ip() const
{
	return m_ip;
}

void	Server::handle_file_request(int client, std::string message, std::string target)
{
	std::vector<std::string> split_cmd = split(message.substr(message.find(":DCC")), " ");
	if (split_cmd.size() < 3)
	{
		write_to_client(client, ":irc 461 " + m_clients[client]->get_nick() + " DCC :Not enough parameters");
		return;
	}
	if (split_cmd[1] == "SEND")
	{
		if (split_cmd.size() < 6)
		{
			write_to_client(client, ":irc 461 " + m_clients[client]->get_nick() + " DCC :Not enough parameters");
			return;
		}
		std::string filename = split_cmd[2];
		std::string ip = decode_ip(split_cmd[3]);
		int port = std::strtol(split_cmd[4].c_str(), NULL, 10);
		double filesize = std::strtod(split_cmd[5].c_str(), NULL);
		t_file file = {client, get_client_by_nick(target)->get_clientSocket() , filename, ip, port, filesize};
		m_files.push_back(file);
		get_client_by_nick(target)->send_message(":" + m_clients[client]->get_nick() + " DCC SEND " + filename + " " + split_cmd[3] + " " + split_cmd[4] + " " + split_cmd[5]); 
	}
}

/*Received: {PRIVMSG gu :DCC SEND ComicMono.ttf 8 1026 18724}
split_cmd: |PRIVMSG
gu
:DCC
SEND
ComicMono.ttf
8
1026
18724
|
 PRIVMSG gu :DCC SEND ComicMono.ttf 8 1026 18724*/