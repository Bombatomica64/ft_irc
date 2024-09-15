/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mruggier <mruggier@student.42firenze.it    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/26 16:20:05 by lmicheli          #+#    #+#             */
/*   Updated: 2024/09/15 21:12:50 by mruggier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include <main.hpp>

class Client
{
	private:

		//client identity
		std::string m_nick;
		std::string str_user;
		bool nickfailed;
		
		std::string ctrlD_msg;
		
		//client internet stuff
		int					m_clientSocket;
		struct sockaddr_in	m_clientAddr;
		std::map<std::string, bool (Client::*)(std::string)> m_cmds;

		//registration stuff
		int			m_reg_steps;
		bool		m_registered;
		bool		m_connected; //true and based
		std::string m_user; //rarely used
		std::string m_hostname; //rarely used
		std::string m_realname; //not used
		std::string m_servername; //not used
		
		//away (not fully implemented)
		bool		m_away;
		std::string m_away_msg;

		//useless
		Client() {}
	public:
		//constructors
		Client(Client const &src);
		Client(std::string nick, std::string user, std::string host);
		Client(int clientSocket, struct sockaddr_in clientAddr);
		~Client();
		Client &operator=(Client const &src);

		//operators they are REALLY useful
		bool	operator==(std::string const &src) const { return (m_nick == src); }
		bool	operator==(Client* const src) const { return (m_nick == src->get_nick()); }
		bool	operator==(Client const &src) const { return (m_clientSocket == src.get_clientSocket()); }
		bool	operator!=(Client const &src) const { return !(*this == src); }
		bool	operator<(Client const &src) const { return (m_clientSocket < src.get_clientSocket()); }
		bool	operator>(Client const &src) const { return (m_clientSocket > src.get_clientSocket()); }
		static bool	is_me( Client* me, Client* other) { return (me->get_nick() == other->get_nick()); }
		static bool	is_me( Client* me, std::string other) { return (me->get_nick() == other); }
		
		//send message to itself or to another client
		bool	send_message(std::string message);
		bool	send_message(Client receiver, std::string message);

		//commands, most of them are implemented in the server
		bool	parse_cmds(std::string cmd);
		// bool	nick(std::string new_nick);
		bool	away(std::string message);

		
	//accessors
	public:
		std::string get_nick( void ) const { return m_nick; }
		std::string get_user( void ) const { return m_user; }
		std::string get_realname( void ) const { return m_realname; }
		std::string get_hostname( void ) const { return m_hostname; }
		std::string get_servername( void ) const { return m_servername; }
		std::string get_away_msg( void ) const { return m_away_msg; }
		int			get_clientSocket( void ) const { return m_clientSocket; }
		int			get_reg_steps( void ) const { return m_reg_steps; }
		bool		get_registered( void ) const { return m_registered; }
		bool		get_connected( void ) const { return m_connected; }
		bool		is_away( void ) const { return m_away; }
		bool		get_nick_failed( void ) const { return nickfailed; }
		std::string get_str_user( void ) const { return str_user; }
		std::string get_ctrlD_msg( void ) const { return ctrlD_msg; }

		//mutators
		void		set_reg(int step) { m_reg_steps = step; }
		void		set_nick(std::string nick) { m_nick = nick; }
		void		set_user(std::string user) { m_user = user; }
		void		set_registered(bool reg) { m_registered = reg; }
		void		set_connected(bool conn) { m_connected = conn; }
		void		set_hostname(std::string hostname) { m_hostname = hostname; }
		void		set_servername(std::string servername) { m_servername = servername; }
		void		set_realname(std::string realname) { m_realname = realname; }
		void		set_nick_failed(bool failed) { nickfailed = failed; }
		void		set_str_user(std::string user) { str_user = user; }
		void		append_ctrlD_msg(std::string msg) { ctrlD_msg = ctrlD_msg + msg; }
		void		clear_ctrlD_msg( void ) { ctrlD_msg.clear(); }
};

//prints alle the information about the client
std::ostream &operator<<(std::ostream &out, Client const &src);

//prints all the client's nick in a vector
inline std::ostream &operator<<(std::ostream &out, std::vector<Client*> clients) {
	for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		out << " ["<<(*it)->get_nick() << "] ";
	}
	return out;
}

//prints all the clients in a map
inline std::ostream &operator<<(std::ostream &o, std::map<std::string, Client*> const &v) {
	for (std::map<std::string, Client*>::const_iterator it = v.begin(); it != v.end(); ++it)
		o << it->first;

	o << std::endl;
	return o;
}

//prints all the clients in a set
inline std::ostream &operator<<(std::ostream &o, std::set<std::string> const &v) {
	for (std::set<std::string>::const_iterator it = v.begin(); it != v.end(); ++it)
		o <<"["<< *it <<"]";
		
	o << std::endl;
	return o;
}

