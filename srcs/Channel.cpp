/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/02 12:37:05 by lmicheli          #+#    #+#             */
/*   Updated: 2024/07/02 18:26:01 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <server.hpp>

Channel::Channel(std::string name)
{
	m_name = name;
	m_modes['i'] = 0;
	m_modes['t'] = 0;
	m_modes['k'] = 0;
	m_modes['o'] = 0;
	m_modes['l'] = 0;
	m_mode_funcs['i'] = &Channel::modify_invite;
	m_mode_funcs['t'] = &Channel::modify_topic_mode;
	m_mode_funcs['k'] = &Channel::modify_key_mode;
	m_mode_funcs['o'] = &Channel::modify_op;
	m_mode_funcs['l'] = &Channel::modify_limit;
}

Channel::~Channel()
{
}

void	Channel::add_client(Client client)
{
	m_clients.push_back(client);
}

void	Channel::remove_client(Client client)
{
	std::vector<Client>::iterator it = std::find(m_clients.begin(), m_clients.end(), client);
	if (it != m_clients.end())
		m_clients.erase(it);
}

bool	Channel::send_message(std::string message)
{
	for (std::vector<Client>::iterator it = m_clients.begin(); it != m_clients.end(); ++it)
	{
		it->send_message(message);
		std::cout << "Message sent to " << it->get_nick() << message << std::endl;
	}
	return true;
}

bool	Channel::send_message(Client sender, std::string message)
{
	for (std::vector<Client>::iterator it = m_clients.begin(); it != m_clients.end(); ++it)
	{
		if (it->get_clientSocket() != sender.get_clientSocket())
		{
			sender.send_message(*it, message);
			std::cout << "Message sent to " << it->get_nick() << message << std::endl;
		}
	}
	return true;
}

void	Channel::join_channel(Client client, std::string parameters)
{
	if (m_modes['l'] && m_clients.size() >= m_modes['l'])
	{
		// TODO send error message
		return;
	}
	if (m_modes['k'] && parameters != m_key)
	{
		// TODO send error message
		return;
	}
	if (m_modes['i'] && m_invites.find(client) == m_invites.end())
	{
		// TODO send error message
		return;
	}
	m_clients.push_back(client);
}

void	Channel::leave_channel(Client client)
{
	std::vector<Client>::iterator it = std::find(m_clients.begin(), m_clients.end(), client);
	if (it != m_clients.end())
		m_clients.erase(it);
}
bool	Channel::modify_mode(std::vector<std::string> command, Client client)
{
	switch (command[2][0])
	{
		case '+':
			for(std::string::iterator it = command[2].begin(); it != command[2].end(); ++it)
			{
				if (m_mode_funcs.find(*it) != m_mode_funcs.end())
					(this->*m_mode_funcs[*it])(client, command[3], true);
				else
				{
					std::cerr << "Unknown mode: " << *it << std::endl; // TODO send error message
					return false;
				}
			}
		case '-':
			for(std::string::iterator it = command[2].begin(); it != command[2].end(); ++it)
			{
				if (m_mode_funcs.find(*it) != m_mode_funcs.end())
					(this->*m_mode_funcs[*it])(client, command[3], false);
				else
				{
					std::cerr << "Unknown mode: " << *it << std::endl; // TODO send error message
					return false;
				}
			}
	}
	return true;
}

void	Channel::modify_invite(Client client, std::string parameters, bool what)
{
	bool is_mod = m_ops.find(client) != m_ops.end();
	if (what && is_mod)
		m_modes['i'] = 1;
	else if (!what && is_mod)
		m_modes['i'] = 0;
	else
	{
		std::cerr << "You are not an operator" << std::endl; // TODO send error message
	}
}


void	Channel::modify_key_mode(Client client, std::string parameters, bool what)
{
	bool is_mod = m_ops.find(client) != m_ops.end();
	if (what && is_mod)
	{
		m_modes['k'] = 1;
		m_key = parameters;
		this->send_message(":" + client.get_nick() + "!" + client.get_user() + "@" + client.get_hostname() + " MODE " + m_name + " +k " + m_key + "\r\n");
	}
	else if (!what && is_mod)
	{
		m_modes['k'] = 0;
		m_key = "";
	}
	else
	{
		std::cerr << "You are not an operator" << std::endl; // TODO send error message
	}
}