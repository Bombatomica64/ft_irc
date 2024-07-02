/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/02 12:37:05 by lmicheli          #+#    #+#             */
/*   Updated: 2024/07/02 12:52:17 by lmicheli         ###   ########.fr       */
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

bool	Channel::send_message(Client sender, std::string message)
{
	for (std::vector<Client>::iterator it = m_clients.begin(); it != m_clients.end(); ++it)
	{
		if (it->get_clientSocket() != sender.get_clientSocket())
		{
			//it->send_message(sender.get_nick(), message);
			std::cout << "Message sent to " << it->get_nick() << message << std::endl;
		}
	}
	return true;
}

void	Channel::join_channel(Client client)
{
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
	std::cout << command[1] << client <<std::endl;
	return true;
}