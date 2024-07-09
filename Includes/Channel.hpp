/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/26 17:01:54 by lmicheli          #+#    #+#             */
/*   Updated: 2024/07/09 11:12:15 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include "server.hpp"

class Server;

class Channel
{
	private:
		std::set<std::string> m_clients;
		std::map<char, int> m_modes;
		std::map<char, void (Channel::*)(Client, std::string, bool)> m_mode_funcs;
		std::vector<std::string>	m_bans;
		std::set<Client>	m_invites;
		std::set<Client>	m_ops;
		std::string			m_name;
		std::string 		m_topic;
		int					m_limit;
		std::string 		m_key;
		Server				*m_server;
		Channel() {}

	public:
		Channel(Channel const &src);
		Channel &operator=(Channel const &src);
		Channel(std::string name, Server *server);
		Channel(std::string name, Server *server,std::map<char, int> modes);
		~Channel();
		void	add_client(Client* client);
		void	remove_client(Client client);
		void	join_channel(Client *client);
		void	join_channel(Client *client, std::string parameters);
		bool	modify_mode(std::vector<std::string> command, Client client);
		void	modify_op(Client client, std::string parameters, bool what);
		void	modify_limit(Client client, std::string parameters, bool what);
		void	modify_invite(Client client, std::string parameters, bool what);
		void	modify_key_mode(Client client, std::string parameters, bool what);
		void	modify_topic_mode(Client client, std::string parameters, bool what);

		// accessors
		public:
		void	set_name(std::string name) { m_name = name; }
		std::string	get_name() { return m_name; }
		std::set<std::string>	get_clients() { return m_clients; }
		std::string	get_topic() { return m_topic; }
		std::string	get_key() { return m_key; }
		int	get_limit() { return m_limit; }
		std::vector<std::string>	get_bans() { return m_bans; }
		std::set<Client>	get_invites() { return m_invites; }
		std::set<Client>	get_ops() { return m_ops; }
		std::map<char, int>	get_modes() { return m_modes; }
		std::map<char, void (Channel::*)(Client, std::string, bool)>	get_mode_funcs() { return m_mode_funcs; }
		void	set_topic(std::string topic) { m_topic = topic; }
		void	set_key(std::string key) { m_key = key; }
		void	set_limit(int limit) { m_limit = limit; }
		void	set_bans(std::vector<std::string> bans) { m_bans = bans; }
		void	set_invites(std::set<Client> invites) { m_invites = invites; }
		void	set_ops(std::set<Client> ops) { m_ops = ops; }
		void	set_modes(std::map<char, int> modes) { m_modes = modes; }
		void	add_ban(std::string ban) { m_bans.push_back(ban); }
		void	add_invite(Client invite) { m_invites.insert(invite);}
		void	add_op(Client op) { m_ops.insert(op); }
		bool	is_client_in(Client *client) const;
};

inline std::ostream &operator<<(std::ostream &o, std::map<std::string, Channel*> const &v) {
	for (std::map<std::string, Channel*>::const_iterator it = v.begin(); it != v.end(); ++it)
		o << "Channel: "<<it->first << " Clients: " << it->second->get_clients() << std::endl;

	o << std::endl;
	return o;
}

inline std::ostream &operator<<(std::ostream &o, std::map<char, void (Channel::*)(Client, std::string, bool)> const &v) {
	for (std::map<char, void (Channel::*)(Client, std::string, bool)>::const_iterator it = v.begin(); it != v.end(); ++it)
		o << "\""<<it->first << "\" ";

	o << std::endl;
	return o;
}