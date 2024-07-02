/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/26 17:01:54 by lmicheli          #+#    #+#             */
/*   Updated: 2024/07/02 18:17:15 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <Client.hpp>

class Client;

class Channel
{
	private:
		std::vector<Client>			m_clients;
		std::map<char, int> m_modes;
		std::map<char, void (Channel::*)(Client, std::string, bool)> m_mode_funcs;
		std::vector<std::string>	m_bans;
		std::set<Client>	m_invites;
		std::set<Client>	m_ops;
		std::string			m_name;
		std::string 		m_topic;
		int					m_limit;
		std::string 		m_key;
		Channel() {}
		Channel(Channel const &src);
		Channel &operator=(Channel const &src);

	public:
		Channel(std::string name);
		// Channel(std::string name, std::map<char, int> modes);
		~Channel();
		void	add_client(Client client);
		void	remove_client(Client client);
		bool	send_message(Client sender, std::string message);
		bool	send_message(std::string message);
		void	join_channel(Client client, std::string parameters);
		void	leave_channel(Client client);
		bool	modify_mode(std::vector<std::string> command, Client client);
		void	modify_invite(Client client, std::string parameters, bool what);
		void	modify_topic_mode(Client client, std::string parameters, bool what);
		void	modify_key_mode(Client client, std::string parameters, bool what);
		void	modify_op(Client client, std::string parameters, bool what);
		void	modify_limit(Client client, std::string parameters, bool what);
};

#endif