/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/02 12:37:05 by lmicheli          #+#    #+#             */
/*   Updated: 2024/07/09 11:53:04 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Channel.hpp>

Channel::Channel(std::string name, Server *server) : m_server(server)
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

Channel::Channel(std::string name, Server *server, std::map<char ,int> modes) : m_server(server)
{
	m_name = name;
	m_modes['i'] = modes['i'];
	m_modes['t'] = modes['t'];
	m_modes['k'] = modes['k'];
	m_modes['o'] = modes['o'];
	m_modes['l'] = modes['l'];
	m_mode_funcs['i'] = &Channel::modify_invite;
	m_mode_funcs['t'] = &Channel::modify_topic_mode;
	m_mode_funcs['k'] = &Channel::modify_key_mode;
	m_mode_funcs['o'] = &Channel::modify_op;
	m_mode_funcs['l'] = &Channel::modify_limit;
}

Channel::Channel(Channel const &src)
{
	*this = src;
}

Channel& Channel::operator=(Channel const &src)
{
	m_clients = src.m_clients;
	m_modes = src.m_modes;
	m_mode_funcs = src.m_mode_funcs;
	m_bans = src.m_bans;
	m_invites = src.m_invites;
	m_ops = src.m_ops;
	m_name = src.m_name;
	m_topic = src.m_topic;
	m_limit = src.m_limit;
	m_key = src.m_key;
	m_server = src.m_server;
	return *this;
}

Channel::~Channel()
{
	m_clients.clear();
	m_mode_funcs.clear();
	std::cout << "Channel " << m_name << " destroyed" << std::endl;
}

void	Channel::add_client(Client* client)
{
	m_clients.insert(client->get_nick());
	std::cout << "Current channel" << m_clients << std::endl;
}

void	Channel::remove_client(Client client)
{
	if (m_clients.find(client.get_nick()) != m_clients.end())
		m_clients.erase(client.get_nick());
}

bool	Channel::is_client_in(Client *client) const
{
	if (m_clients.find(client->get_nick()) != m_clients.end())
		return true;
	return false;
}
// moved to server.cpp
// bool	Channel::send_message(Client sender, std::string message)
// {
// 	for (std::set<std::string>::iterator it = m_clients.begin(); it != m_clients.end(); ++it)
// 	{
// 		if (*it != sender.get_nick())
// 		{
// 			sender.send_message(message);
// 			std::cout << "Message sent to " << it->second->get_nick() << message << std::endl;
// 		}
// 	}
// 	return true;
// }

void	Channel::join_channel(Client *client, std::string parameters)
{
	if (m_modes['l'] && m_clients.size() >= static_cast<size_t>(m_modes['l']))
	{
		// TODO send error message
		return;
	}
	if (m_modes['k'] && parameters != m_key)
	{
		// TODO send error message
		return;
	}
	if (m_modes['i'] && m_invites.find(*client) == m_invites.end())
	{
		// TODO send error message
		return;
	}
	this->add_client(client);
}

void	Channel::join_channel(Client *client)
{
	if (m_modes['l'] && m_clients.size() >= static_cast<size_t>(m_modes['l']))
	{
		// TODO send error message
		return;
	}
	// if (m_modes['k'] && m_key != "")
	// {
	// 	// TODO send error message
	// 	return;
	// }
	if (m_modes['i'] && m_invites.find(*client) == m_invites.end())
	{
		// TODO send error message
		return;
	}
	this->add_client(client);

}

// void	Channel::leave_channel(Client client)
// {
// 	std::vector<Client>::iterator it =m_clients.begin(), m_clients.end(), client);
// 	if (it != m_clients.end())
// 		m_clients.erase(it);
// }
bool	Channel::modify_mode(std::vector<std::string> command, Client client)
{
	int i = 0;
	std::cout << "command[2] = " << command[2] << std::endl;
	std::cout << "the modes are: " << m_mode_funcs << std::endl;
	while (command[2][i] != '\0' && command[2][i] != ' ' && command[2][i] != '\r')
	{
	switch (command[2][i])
	{
		case '+':
		if (command[2].size() > 1)
			for(std::string::iterator it = command[2].begin() + 1; it != command[2].end(); ++it)
			{
				if (m_mode_funcs.find(*it) != m_mode_funcs.end())
					(this->*m_mode_funcs[*it])(client, command[2].substr(command[2].find(" ")), true);
				else
				{
					std::cerr << "Unknown mode: " << *it << std::endl; // TODO send error message
					return false;
				}
			}
		case '-':
		if (command[2].size() > 1)
			for(std::string::iterator it = command[2].begin() + 1; it != command[2].end(); ++it)
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
	}
	return true;
}

void	Channel::modify_invite(Client client, std::string parameters, bool what)
{
	(void)parameters;
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
		client.send_message(":" + client.get_nick() + "!" + client.get_user() + "@" + client.get_hostname() + " MODE " + m_name + " +k " + m_key + "\r\n");
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

void	Channel::modify_topic_mode(Client client, std::string parameters, bool what)
{
	(void)parameters;
	bool is_mod = m_ops.find(client) != m_ops.end();
	if (what && is_mod)
	{
		m_modes['t'] = 1;
		client.send_message(":" + client.get_nick() + "!" + client.get_user() + "@" + client.get_hostname() + " TOPIC " + m_name + " :" + m_topic + "\r\n");
	}
	else if (!what && is_mod)
	{
		m_modes['t'] = 0;
		m_topic = "";
	}
	else
	{
		std::cerr << "You are not an operator" << std::endl; // TODO send error message
	}
}

void	Channel::modify_limit(Client client, std::string parameters, bool what)
{
	bool is_mod = m_ops.find(client) != m_ops.end();
	if (what && is_mod)
	{
		try
		{
			m_modes['l'] = std::strtol(parameters.c_str(), NULL, 10);
			m_server->send_msg_to_channel(-1 , this->get_name() ,":" + client.get_nick() + "!" + client.get_user() + "@" + client.get_hostname() + " MODE " + m_name + " +l " + parameters + "\r\n");
		}
		catch(const std::exception& e)
		{
			std::cerr << "Invalid limit" << std::endl; // TODO send error message
		}
	}
	else if (!what && is_mod)
	{
		m_modes['l'] = 0;
	}
	else
	{
		std::cerr << "You are not an operator" << std::endl; // TODO send error message
	}
}

void	Channel::modify_op(Client client, std::string parameters, bool what)
{
	bool is_mod = m_ops.find(client) != m_ops.end();
	if (what && is_mod)
	{
		Client *to_add = m_server->get_client_by_nick(parameters);
		if (m_clients.find(to_add->get_nick()) != m_clients.end())
		{
			m_ops.insert(*to_add);
			m_server->send_msg_to_channel(-1 , this->get_name() ,":" + client.get_nick() + "!" + client.get_user() + "@" + client.get_hostname() + " MODE " + m_name + " +o " + parameters + "\r\n");
		}
		else if (m_ops.size() >= 3)
		{
			std::cerr << "Too many operators" << std::endl; // TODO send error message
		}
		else
		{
			std::cerr << "Client not found" << std::endl; // TODO send error message
		}
	}
	else if (!what && is_mod)
	{
		if (m_ops.find(client) != m_ops.end())
		{
			m_ops.erase(client);
			m_server->send_msg_to_channel(-1 , this->get_name() ,":" + client.get_nick() + "!" + client.get_user() + "@" + client.get_hostname() + " MODE " + m_name + " -o " + parameters + "\r\n");
		}
	}
	else
	{
		std::cerr << "You are not an operator" << std::endl; // TODO send error message
	}
}