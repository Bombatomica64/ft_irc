/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/26 17:01:54 by lmicheli          #+#    #+#             */
/*   Updated: 2024/09/11 12:24:21 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include "server.hpp"

class Server;

class Channel
{
	private:
		
		//channel info
		std::string			m_name;
		std::string 		m_topic;
		std::string 		m_key;
		int					m_limit;

		//channel lists
		std::set<std::string> m_clients;
		std::set<std::string>	m_bans;
		std::set<std::string>	m_invites;
		std::set<std::string>	m_ops;
		
		//channel modes and functions
		std::map<char, int> m_modes;
		std::map<char, void (Channel::*)(Client&, std::string, bool)> m_mode_funcs;
		
		//pointer to server
		Server		*m_server;

		//useless
		Channel() {}

	public:

		//constructors
		Channel(Channel const &src);
		Channel &operator=(Channel const &src);
		Channel(std::string name, Server *server);
		Channel(std::string name, Server *server,std::map<char, int> modes);
		~Channel();

		//client movement
		void	add_client(std::string client);
		void	remove_client(std::string client_nick, Client &client);
		void	join_channel(Client& client, std::string parameters);

		//mode functions
		bool	modify_mode(std::vector<std::string> command, Client& client);
		void	modify_op(Client& client, std::string parameters, bool what);
		void	modify_ban(Client& client, std::string parameters, bool what);
		void	modify_limit(Client& client, std::string parameters, bool what);
		void	modify_invite(Client& client, std::string parameters, bool what);
		void	modify_key_mode(Client& client, std::string parameters, bool what);
		void	modify_topic_mode(Client& client, std::string parameters, bool what);
		void	modify_client_nick(std::string old_nick, std::string new_nick);
		
		//send functions
		bool	send_topic(Client &client);
		bool	send_topic_all( void );
		void	send_modes(Client &client);
		void	send_bans(Client &client);
		void	topuc(Client &client, std::string parameters); //it


		public:
		// accessors
		int	size( void ) { return static_cast<int>(m_clients.size()); }
		int	get_limit( void ) { return m_limit; }
		int	get_mode(char mode);
		std::string	get_key( void ) { return m_key; }
		std::string	get_name( void ) { return m_name; }
		std::string	get_topic( void ) { return m_topic; }
		std::set<std::string>	get_clients( void ) { return m_clients; }
		std::set<std::string>	get_bans( void ) { return m_bans; }
		std::set<std::string>	get_invites( void ) { return m_invites; }
		std::set<std::string>	get_ops( void ) { return m_ops; }
		std::map<char, int>		get_modes( void ) { return m_modes; }

		// mutators
		void	set_name(std::string name) { m_name = name; }
		void	set_topic(std::string topic) { m_topic = topic; }
		void	set_key(std::string key) { m_key = key; }
		void	set_limit(int limit) { m_limit = limit; }
		void	set_bans(std::set<std::string> bans) { m_bans = bans; }
		void	set_modes(std::map<char, int> modes) { m_modes = modes; }
		void	add_ban(std::string ban) { m_bans.insert(ban); }
		void	add_invite(std::string invite) { m_invites.insert(invite);}
		void	add_op(std::string op) { m_ops.insert(op); }
		bool	is_client_in(std::string client) const;
		bool	is_op(std::string client) const;
		bool	is_ban(std::string client) const;
};

///Prints all the cannels and their clients
inline std::ostream &operator<<(std::ostream &o, std::map<std::string, Channel*> const &v) {
	for (std::map<std::string, Channel*>::const_iterator it = v.begin(); it != v.end(); ++it)
		o << "Channel: "<<it->first << " Clients: " << it->second->get_clients() << std::endl;

	o << std::endl;
	return o;
}

///Prints all the modes of a channel
inline std::ostream &operator<<(std::ostream &o, std::map<char, void (Channel::*)(Client&, std::string, bool)> const &v) {
	for (std::map<char, void (Channel::*)(Client&, std::string, bool)>::const_iterator it = v.begin(); it != v.end(); ++it)
		o << "\""<<it->first << "\" ";

	o << std::endl;
	return o;
}

