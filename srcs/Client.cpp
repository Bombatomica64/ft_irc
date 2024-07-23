/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mruggier <mruggier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/28 11:41:18 by lmicheli          #+#    #+#             */
/*   Updated: 2024/07/23 18:41:22 by mruggier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Client.hpp>

Client::Client(int clientSocket, struct sockaddr_in clientAddr)
{
	m_clientSocket = clientSocket;
	m_clientAddr = clientAddr;
	m_registered = false;
	m_connected = false;
	m_reg_steps = 0;
	m_cmds["NICK"] = &Client::nick;
	// m_cmds["OPER"] = &Client::oper;
	// m_cmds["KICK"] = &Client::kick;
	// m_cmds["PING"] = &Client::ping;
	m_cmds["AWAY"] = &Client::away;
	// m_cmds["TOPIC"] = &Client::topic;
}

Client::Client(Client const &src)
{
	*this = src;
}

Client& Client::operator=(Client const &src)
{
	m_clientSocket = src.m_clientSocket;
	m_clientAddr = src.m_clientAddr;
	m_registered = src.m_registered;
	m_connected = src.m_connected;
	m_reg_steps = src.m_reg_steps;
	m_nick = src.m_nick;
	m_user = src.m_user;
	m_realname = src.m_realname;
	m_hostname = src.m_hostname;
	m_servername = src.m_servername;
	return *this;
}

// bool Client::operator==(Client &src) const
// {
// 	return (m_clientSocket == src.get_clientSocket());
// }

Client::~Client()
{
	std::cout << BLUE << "Client " << m_nick << " disconnected: " << __FILE__ << " at line " << __LINE__ << RESET<< std::endl;
}

std::ostream &operator<<(std::ostream &out, Client const &src)
{
	out << "Client socket: " << src.get_clientSocket() << std::endl;
	out << "Client nick: " << src.get_nick() << std::endl;
	out << "Client registered: " << src.get_registered() << std::endl;
	out << "Client connected: " << src.get_connected() << std::endl;
	out << "Client registration steps: " << src.get_reg_steps() << std::endl;
	out << "Client username: " << src.get_user() << std::endl;
	out << "Client hostname: " << src.get_hostname() << std::endl;
	out << "Client servername: " << src.get_servername() << std::endl;
	out << "Client realname: " << src.get_realname() << std::endl;
	return out;
}

bool Client::send_message(std::string message)
{
	message += "\r\n";
	if (send(m_clientSocket, message.c_str(), message.size(), 0) == -1)
	{
		std::cerr << "Error: send failed" << std::endl; // TODO handle error
		return false;
	}
	return true;
}

bool Client::send_message(Client receiver, std::string message)
{
	std::string return_msg;
	if (receiver.is_away())
	{
		return_msg = "AWAY " + receiver.get_away_msg() + " : " + message;
		this->send_message(return_msg);
		return true;
	}
	if (send(receiver.get_clientSocket(), message.c_str(), message.size(), 0) == -1)
	{
		std::cerr << "Error: send failed" << std::endl; // TODO handle error
		return false;
	}
	return true;
}

bool	Client::parse_cmds(std::string cmd)
{
	std::vector<std::string> split_cmd = split(cmd, " ");
	if (m_cmds.find(split_cmd[0]) != m_cmds.end())
	{
		std::cout << "Command: |" << split_cmd[0] << "|" << std::endl;
		if (!(this->*m_cmds[split_cmd[0]])(cmd))
			{
				std::cerr << "Error: command failed" << std::endl; // TODO handle error
				return false;
			}
		else
			return true;
	}
	else
	{
		std::cerr << "Error: command not found" << std::endl; // TODO handle error
		return false;
	}
	// TODO error sending
}


bool Client::nick(std::string new_nick)
{
	std::vector<std::string> split_msg = split(new_nick, " ");
	if (split_msg.size() == 2)
	{
		m_nick = split_msg[1];
		return true;
	}
	if (split_msg.size() == 1)
	{
		this->send_message("431 :No nickname given");
		return false;
	}
	return false;
}

bool	Client::away(std::string message)
{
	std::vector<std::string> split_msg = split(message, " ");
	if (split_msg.size() == 1)
	{
		// TODO handle error
		return false;
	}
	else
	{
		std::string msg;
		for (size_t i = 1; i < split_msg.size(); i++)
			msg += split_msg[i] + " ";
		m_away = true;
		m_away_msg = msg;
		return true;
	}
	// TODO handle error
	return false;
}

// moved to server
// bool Client::quit(std::string message)
// {
// 	std::vector<std::string> split_msg = split(message, " ");
// 	if (split_msg.size() == 1)
// 	{
// 		send_message(m_nick);
// 	}
// 	else
// 	{
// 		std::string msg;
// 		for (size_t i = 1; i < split_msg.size(); i++)
// 			msg += split_msg[i] + " ";
// 		send_message(msg);
// 	}
// 	close(m_clientSocket);
// 	return true;
// }
// moved to server
// bool Client::privmsg(std::string message)
// {
// 	size_t start = message.find("PRIVMSG");
// 	if (start == std::string::npos)
// 	{
// 		// handle error
// 		return false;
// 	}
// 	std::string msg = message.substr(start + strlen("PRIVMSG "));
// 	std::string to_send = msg.substr(msg.find(" :") + 2);
// 	std::string target_str = msg.substr(0, msg.find(" :"));
// 	std::vector<std::string> split_targets = split(target_str, ",");
// 	for (std::vector<std::string>::iterator it = split_targets.begin(); it != split_targets.end(); it++)
// 	{
// 		switch ((*it)[0])
// 		{
// 			// send to channel
// 		case '#':
// 		case '&':
// 			if (!m_server->get_channel(*it)->send_message(*this, to_send))
// 				return false;
// 			break;
// 		default:
// 			// send to user
// 			if (!m_server->get_client_by_nick(*it)->send_message(to_send))
// 				return false;
// 			break;
// 		}
// 	}
// 	return true;
// }

// moved to server
// bool Client::join(std::string channel)
// {
// 	std::vector<std::string> split_msg = split(channel, " ");
// 	std::vector<std::string> split_channel = split(split_msg[1], ",");
// 	std::vector<std::string> split_key;
// 	int i = 0;
// 	if (split_msg.size() < 2)
// 	{
// 		//  handle error
// 		return false;
// 	}
// 	for (std::vector<std::string>::iterator it = split_channel.begin(); it != split_msg.end(); it++)
// 	{
// 		Channel *chan = m_server->get_channel(*it);
// 		if (chan)
// 			chan->join_channel(*this, split_key[i++]);
// 		else
// 		{
// 			m_server->add_channel(*it);
// 			chan = m_server->get_channel(*it);
// 			chan->add_client(*this);
// 			chan->add_op(*this);
// 		}
// 		if (it == split_channel.end())
// 			return true;
// 	}
// 	//  handle error
// 	return false;
// }
// moved to server
// bool Client::part(std::string channels)
// {
// 	std::vector<std::string> split_msg = split(channels, " ");
// 	std::vector<std::string> split_channel = split(split_msg[1], ",");
// 	if (split_msg.size() < 2)
// 	{
// 		//  handle error
// 		return false;
// 	}
// 	for (std::vector<std::string>::iterator it = split_channel.begin(); it != split_channel.end(); it++)
// 	{
// 		Channel *chan = m_server->get_channel(*it);
// 		if (chan)
// 			chan->leave_channel(*this);
// 		else
// 		{
// 			//  handle error
// 			return false;
// 		}
// 	}
// 	return true;
// }

// moved to server
// bool Client::mode(std::string message)
// {
// 	std::vector<std::string> l_command;
// 	l_command.push_back(message.substr(message.find("MODE") + strlen("MODE"))); // "MODE"
// 	message = message.substr(message.find("MODE") + strlen("MODE"));
// 	l_command.push_back(message.substr(message.find(" ") + 1)); // "channel"
// 	message = message.substr(message.find(" ") + 1);
// 	l_command.push_back(message.substr(message.find(" ") + 1)); // "operation"
// 	message = message.substr(message.find(" ") + 1);
// 	if (message.find(" ") != std::string::npos)
// 		l_command.push_back(message.substr(message.find(" ") + 1)); // "arguments"
// 	if (m_server->get_channel(l_command[1])->modify_mode(l_command, *this))
// 		return true;
// 	// handle error
// 	return false;
// }
// moved to server
// bool Client::invite(std::string message)
// {
// 	std::vector<std::string> split_msg = split(message, " ");
// 	if (split_msg.size() == 3)
// 	{
// 		m_server->get_channel(split_msg[1])->add_client(*m_server->get_client_by_nick(split_msg[2]));
// 		return true;
// 	}
// 	// handle error
// 	return false;
// }