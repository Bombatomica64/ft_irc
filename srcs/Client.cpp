/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/28 11:41:18 by lmicheli          #+#    #+#             */
/*   Updated: 2024/08/06 17:03:06 by lmicheli         ###   ########.fr       */
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
	// m_cmds["NICK"] = &Client::nick;
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
		return false;
	}
	return true;
}

bool	Client::parse_cmds(std::string cmd)
{
	std::vector<std::string> split_cmd = split(cmd, " ");
	if (m_cmds.find(split_cmd[0]) != m_cmds.end())
	{
		if (!(this->*m_cmds[split_cmd[0]])(cmd))
			return false;
		else
			return true;
	}
	else
		return false;
}


// bool Client::nick(std::string new_nick)
// {
// 	std::vector<std::string> split_msg = split(new_nick, " ");
// 	if (split_msg.size() == 2)
// 	{
// 		m_nick = split_msg[1];
// 		return true;
// 	}
// 	if (split_msg.size() == 1)
// 	{
// 		this->send_message("431 :No nickname given");
// 		return false;
// 	}
// 	return false;
// }

bool	Client::away(std::string message)
{
	std::vector<std::string> split_msg = split(message, " ");
	if (split_msg.size() == 1)
	{
		send_message(":irc 461" + m_nick + " AWAY :Not enough parameters");
		return true;
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
	return false;
}
