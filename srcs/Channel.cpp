/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mruggier <mruggier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/02 12:37:05 by lmicheli          #+#    #+#             */
/*   Updated: 2024/09/05 15:52:05 by mruggier         ###   ########.fr       */
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
	m_modes['b'] = 0;
	m_mode_funcs['i'] = &Channel::modify_invite;
	m_mode_funcs['t'] = &Channel::modify_topic_mode;
	m_mode_funcs['k'] = &Channel::modify_key_mode;
	m_mode_funcs['o'] = &Channel::modify_op;
	m_mode_funcs['l'] = &Channel::modify_limit;
	m_mode_funcs['b'] = &Channel::modify_ban;
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

void	Channel::add_client(std::string client)
{
	m_clients.insert(client);
	std::cout << "Current channel " << m_name << " is :" << m_clients << std::endl;
}

void	Channel::remove_client(std::string client_nick, Client &client)
{
	if (m_clients.find(client_nick) != m_clients.end())
	{
		m_clients.erase(client_nick);
	}
	if (m_ops.find(client_nick) != m_ops.end())
		m_ops.erase(client_nick);
	if (this->size() == 0)
		m_server->remove_channel(m_name);
	else if (m_ops.size() == 0 && !m_clients.empty())
	{
		try
		{
			std::set<std::string>::iterator it = m_clients.begin();
			std::advance(it, rand() % m_clients.size());
			m_ops.insert(*it);

			m_server->send_msg_to_channel(-1 , this->get_name() ,":" + client.get_nick() + "!" + client.get_user() + "@" + client.get_hostname() + " MODE " + m_name + " +o " + *it + "\r\n");
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
	}	
}

bool	Channel::is_client_in(std::string client) const
{
	if (m_clients.find(client) != m_clients.end())
		return true;
	return false;
}

void	Channel::join_channel(Client& client, std::string parameters)
{
	if (m_modes['l'] && m_clients.size() >= static_cast<size_t>(m_modes['l']))
	{
		client.send_message(":irc 471 " + client.get_nick() + " " + m_name + " :Cannot join channel (+l)");
		return;
	}
	if (m_modes['i'] && m_invites.find(client.get_nick()) == m_invites.end())
	{
		client.send_message(":irc 473 " + client.get_nick() + " " + m_name + " :Cannot join channel (+i)");
		return;
	}
	if (m_bans.find(client.get_nick()) != m_bans.end())
	{
		client.send_message(":irc 474 " + client.get_nick() + " " + m_name + " :Cannot join channel (+b)");
		return;
	}
	if (m_modes['k'] && parameters != m_key)
	{
		client.send_message(":irc 475 " + client.get_nick() + " " + m_name + " :Cannot join channel (+k)");
		return;
	}
	this->add_client(client.get_nick());
	this->m_server->send_msg_to_channel(-1 , this->get_name() ,":" + client.get_nick() + "!" + client.get_user() + "@" + client.get_hostname() + " JOIN " + m_name + "\r\n");
	this->send_modes(client);
	this->send_topic(client);
	this->m_server->names(client.get_clientSocket(), "NAMES " + m_name);
}

bool	Channel::modify_mode(std::vector<std::string> command, Client &client)
{
	int i = 0;
	std::cout << "Modifiying mode on channel |" << m_name << "|" << std::endl;

	std::cout << "command[2] = " << command[2] << std::endl;

	std::cout << "the modes are: " << m_mode_funcs << std::endl;
	if (command.size() > 4)
	{
		// std::cerr << "Too many arguments" << std::endl; there isn't an error message
		return false;
	}
	if (command.size() < 4)
		command.push_back("");
	while (command[2][i] != '\0' && command[2][i] != ' ' && command[2][i] != '\r')
	{
	switch (command[2][i])
	{
		case '+':
		if (command[2].size() > 1)
			for(std::string::iterator it = command[2].begin() + 1; it != command[2].end(); ++it)
			{
				if (m_mode_funcs.find(*it) != m_mode_funcs.end())
					(this->*m_mode_funcs[*it])(client, command[3], true);
				else
				{
					client.send_message(":irc 472 " + client.get_nick() + " " + m_name + " :Unknown mode");
					// std::cerr << "Unknown mode: " << *it << std::endl;
					return true;
				}
			}
		break;
		case '-':
		if (command[2].size() > 1)
			for(std::string::iterator it = command[2].begin() + 1; it != command[2].end(); ++it)
			{
				if (m_mode_funcs.find(*it) != m_mode_funcs.end())
					(this->*m_mode_funcs[*it])(client, command[3], false);
				else
				{
					client.send_message(":irc 472 " + client.get_nick() + " " + m_name + " :Unknown mode");
					// std::cerr << "Unknown mode: " << *it << std::endl;
					return false;
				}
			}
		break;
	}
	i++;
	}
	return true;
}

void	Channel::modify_invite(Client &client, std::string parameters, bool what)
{
	(void)parameters;
	bool is_mod = m_ops.find(client.get_nick()) != m_ops.end();
	if (what && is_mod)
		m_modes['i'] = 1;
	else if (!what && is_mod)
		m_modes['i'] = 0;
	else
	{
		client.send_message(":irc 482 " + client.get_nick() + " " + m_name + " :You're not a channel operator");
		// std::cerr << "You are not an operator" << std::endl;
	}
}

void	Channel::modify_key_mode(Client &client, std::string parameters, bool what)
{
	bool is_mod = m_ops.find(client.get_nick()) != m_ops.end();
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
		client.send_message(":irc 482 " + client.get_nick() + " " + m_name + " :You're not a channel operator");
		// std::cerr << "You are not an operator" << std::endl;
	}
}
/**
 * @brief changes the topic of the channel and sends it to the client
 */
void	Channel::topuc(Client &client, std::string parameters)
{
	bool is_mod = m_ops.find(client.get_nick()) != m_ops.end();
	if (parameters.find(":") != std::string::npos)
		parameters = parameters.substr(parameters.find(":") + 1, parameters.size());
	if (m_modes['t'] == 0)
		m_topic = parameters;
	else if (is_mod)
		m_topic = parameters;
	else
	{
		client.send_message(":irc 482 " + client.get_nick() + " " + m_name + " :You're not a channel operator");
		return;
	}
	send_topic(client);
}

void	Channel::modify_topic_mode(Client &client, std::string parameters, bool what)
{
	bool is_mod = m_ops.find(client.get_nick()) != m_ops.end();
	if (what && is_mod)
	{
		m_modes['t'] = 1;
		if (!parameters.empty())
			m_topic = parameters;
		// client.send_message(":" + client.get_nick() + "!" + client.get_user() + "@" + client.get_hostname() + " TOPIC " + m_name + " :" + m_topic + "\r\n");
		send_topic(client);
	}
	else if (!what && is_mod)
	{
		m_modes['t'] = 0;
		if (parameters.find(" ") != std::string::npos)
			m_topic = parameters.substr(parameters.find(" ") + 1);
	}
	else if (m_modes['t'] == 0)
	{
		if (parameters.find(" ") != std::string::npos)
			m_topic = parameters.substr(parameters.find(" ") + 1);
	}
	else
	{
		client.send_message(":irc 482 " + client.get_nick() + " " + m_name + " :You're not a channel operator");
		// std::cerr << "You are not an operator" << std::endl;
	}
}

void	Channel::modify_limit(Client &client, std::string parameters, bool what)
{
	bool is_mod = m_ops.find(client.get_nick()) != m_ops.end();
	if (what && is_mod)
	{
		try
		{
			m_modes['l'] = std::strtol(parameters.c_str(), NULL, 10);
			m_server->send_msg_to_channel(-1 , this->get_name() ,":" + client.get_nick() + "!" + client.get_user() + "@" + client.get_hostname() + " MODE " + m_name + " +l " + parameters + "\r\n");
		}
		catch(const std::exception& e)
		{
			client.send_message(":irc 400 " + client.get_nick() + " " + m_name + " :Invalid limit");
			// std::cerr << "Invalid limit" << std::endl;
		}
	}
	else if (!what && is_mod)
	{
		m_modes['l'] = 0;
	}
	else
	{
		client.send_message(":irc 482 " + client.get_nick() + " " + m_name + " :You're not a channel operator");
		// std::cerr << "You are not an operator" << std::endl;
	}
}

void	Channel::modify_op(Client &client, std::string parameters, bool what)
{
	bool is_mod = m_ops.find(client.get_nick()) != m_ops.end();
	if (what && is_mod)
	{
		if (!m_server->client_exist(parameters))
		{
			client.send_message(":irc 401 " + client.get_nick() + " " + m_name + " :No such nick/channel");
			// std::cerr << "Client not found" << std::endl;
			return;
		}
		Client *to_add = m_server->get_client_by_nick(parameters);
		if (m_clients.find(to_add->get_nick()) != m_clients.end())
		{
			m_ops.insert(to_add->get_nick());
			m_server->send_msg_to_channel(-1 , this->get_name() ,":" + client.get_nick() + "!" + client.get_user() + "@" + client.get_hostname() + " MODE " + m_name + " +o " + parameters + "\r\n");
		}
		else if (m_ops.size() >= 3)
		{
			client.send_message(":irc 400 " + client.get_nick() + " " + m_name + " :Too many operators");
			// std::cerr << "Too many operators" << std::endl;
		}
		else
		{
			client.send_message(":irc 441 " + client.get_nick() + " " + m_name + " :They aren't on that channel");
			// std::cerr << "Client not found" << std::endl;
		}
	}
	else if (!what && is_mod)
	{
		if (m_ops.find(parameters) != m_ops.end())
		{
			m_ops.erase(parameters);
			m_server->send_msg_to_channel(-1 , this->get_name() ,":" + client.get_nick() + "!" + client.get_user() + "@" + client.get_hostname() + " MODE " + m_name + " -o " + parameters + "\r\n");
		}
		else
		{
			//TODO send error message per dire che non è un op e quindi non può essere rimosso dagli op
		}
	}
	else
	{
		client.send_message(":irc 482 " + client.get_nick() + " " + m_name + " :You're not a channel operator");
		// std::cerr << "You are not an operator" << std::endl; 
	}
}

void	Channel::send_modes(Client &client)
{
	std::string start = ":irc 324 " + client.get_nick() + " " + this->get_name();
	std::string modes = " +";
	for (std::map<char, int>::iterator it = m_modes.begin(); it != m_modes.end(); ++it)
	{
		if (it->second != 0)
		{
			modes += it->first;
		}
	}
	if (m_modes['l'])
		modes += " " + NumberToString(m_modes['l']);
	if (m_modes['k'])
		modes += " " + m_key;
	if (modes == " +")
		modes = " none";
	client.send_message(start + modes);
}

bool	Channel::send_topic(Client &client)
{
	if (m_topic.empty())
		return(client.send_message(":irc 331 " + client.get_nick() + " " + m_name + " :No topic is set")); 
	else
		return(client.send_message(":irc 332 " + client.get_nick() + " " + m_name + " :" + m_topic));
}

bool	Channel::send_topic_all( void )
{
	if (m_topic.empty())
		return false;
	m_server->send_msg_to_set(m_clients, ":irc 332 " + m_name + " :" + m_topic);
	return true;
}


bool	Channel::is_op(std::string client) const
{
	if (m_ops.find(client) != m_ops.end())
		return true;
	return false;
}

void	Channel::modify_ban(Client &client, std::string parameters, bool what)
{
	bool is_mod = m_ops.find(client.get_nick()) != m_ops.end();
	if (parameters.empty())
	{
		this->send_bans(client);
		return;
	}
	
	if (what && is_mod)
	{
		m_bans.insert(parameters);
		m_server->send_msg_to_channel(-1 , this->get_name() ,":" + client.get_nick() + "!" + client.get_user() + "@" + client.get_hostname() + " MODE " + m_name + " +b " + parameters + "\r\n");
	}
	else if (!what && is_mod)
	{
		if (m_bans.find(parameters) != m_bans.end())
		{
			m_bans.erase(parameters);
			m_server->send_msg_to_channel(-1 , this->get_name() ,":" + client.get_nick() + "!" + client.get_user() + "@" + client.get_hostname() + " MODE " + m_name + " -b " + parameters + "\r\n");
		}
	}
	else
	{
		client.send_message(":irc 482 " + client.get_nick() + " " + m_name + " :You're not a channel operator");
		// std::cerr << "You are not an operator" << std::endl;
	}
}

void	Channel::send_bans(Client &client)
{
	for (std::set<std::string>::iterator it = m_bans.begin(); it != m_bans.end(); ++it)
	{
		client.send_message(":" + client.get_nick() + "!" + client.get_user() + "@" + client.get_hostname() + " MODE " + m_name + " +b " + *it + "\r\n");
	}
}

bool	Channel::is_ban(std::string client) const
{
	if (m_bans.find(client) != m_bans.end())
		return true;
	return false;
}

int	Channel::get_mode(char mode)
{
	if(m_modes.find(mode) == m_modes.end())
		return m_modes[mode];
	return -1;
}

void	Channel::modify_client_nick(std::string old_nick, std::string new_nick)
{
	if (m_clients.find(old_nick) != m_clients.end())
	{
		m_clients.erase(old_nick);
		m_clients.insert(new_nick);
	}
}
