/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/28 11:41:18 by lmicheli          #+#    #+#             */
/*   Updated: 2024/07/01 18:29:09 by lmicheli         ###   ########.fr       */
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
	m_oper = false;
	Server::get_cmds();
	m_cmds["QUIT"] = &Client::quit;
	// m_cmds["JOIN"] = &Client::join;
	// m_cmds["PART"] = &Client::part;
	m_cmds["PRIVMSG"] = &Client::privmsg;
	m_cmds["NICK"] = &Client::nick;
	// m_cmds["OPER"] = &Client::oper;
	// m_cmds["MODE"] = &Client::mode;
	// m_cmds["INVITE"] = &Client::invite;
	// m_cmds["KICK"] = &Client::kick;
	// m_cmds["PING"] = &Client::ping;
	// m_cmds["PONG"] = &Client::pong;
	// m_cmds["TOPIC"] = &Client::topic;

}

Client::~Client()
{
	close(m_clientSocket);
}

std::ostream &operator<<(std::ostream &out, Client const &src)
{
	out << "Client socket: " << src.get_clientSocket() << std::endl;
	out << "Client nick: " << src.get_nick() << std::endl;
	out << "Client user: " << src.get_user() << std::endl;
	out << "Client registered: " << src.get_registered() << std::endl;
	out << "Client connected: " << src.get_connected() << std::endl;
	out << "Client registration steps: " << src.get_reg_steps() << std::endl;
	return out;
}

void	Client::parse_cmds(std::string cmd)
{
	std::vector<std::string> split_cmd = split(cmd, " ");
	if (m_cmds.find(split_cmd[0]) != m_cmds.end())
	{
		if (!(this->*m_cmds[split_cmd[0]])(cmd))
			write_to_client( m_clientSocket,"421 " + m_nick + " " + split_cmd[0] + " :Unknown command");
	}
	else
		write_to_client( m_clientSocket, "421 " + m_nick + " " + split_cmd[0] + " :Unknown command");
	//TODO error sending
}

bool	Client::quit(std::string message)
{
	std::vector<std::string> split_msg = split(message, " ");
	if (split_msg.size() == 1)
	{
		write_to_client(m_clientSocket, m_nick);
	}
	else
	{
		std::string msg;
		for (size_t i = 1; i < split_msg.size(); i++)
			msg += split_msg[i] + " ";
		write_to_client(m_clientSocket, msg);
	}
}

bool	Client::privmsg(std::string message)
{
	Server::get_id();
}
