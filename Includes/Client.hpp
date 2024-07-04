/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/26 16:20:05 by lmicheli          #+#    #+#             */
/*   Updated: 2024/07/04 18:36:45 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include <main.hpp>

class Client
{
	private:
		int			m_clientSocket;
		struct sockaddr_in m_clientAddr;
		int	m_reg_steps;
		std::string m_nick;
		std::string m_user;
		std::map<std::string, bool (Client::*)(std::string)> m_cmds;
		bool		m_registered;
		bool		m_connected;
		std::string m_realname;
		std::string m_hostname;
		std::string m_servername;

	public:
		Client() {}
		Client(Client const &src);
		Client &operator=(Client const &src);
		bool	operator==(std::string const &src) const { return (m_nick == src); }
		bool	operator==(Client* const &src) const { return (m_nick == src->get_nick()); }
		bool	operator==(Client const &src) const { return (m_clientSocket == src.get_clientSocket()); }
		bool	operator!=(Client const &src) const { return !(*this == src); }
		bool	operator<(Client const &src) const { return (m_clientSocket < src.get_clientSocket()); }
		bool	operator>(Client const &src) const { return (m_clientSocket > src.get_clientSocket()); }
		Client(int clientSocket, struct sockaddr_in clientAddr);
		~Client();
		bool	send_message(std::string message);
		bool	send_message(Client receiver, std::string message);
		bool	parse_cmds(std::string cmd);
		bool	quit(std::string message);
		bool	nick(std::string new_nick);
		bool	ping(std::string message);
		/*
		void	receive_message( void );
		void	connect_to_channel( void );
		bool	list(std::string message);
		bool	away(std::string message);
		bool	admin(std::string message);
		bool	summon(std::string message);
		bool	users(std::string message);
		bool	notice(std::string message);
		idk what to do with these functions yet
		*/
		// bool	oper(std::string user);
		// bool	kick(std::string message);
		// bool	topic(std::string message);
		static bool	is_me( Client* me, Client* other) { return (me->get_nick() == other->get_nick()); }
		static bool	is_me( Client* me, std::string other) { return (me->get_nick() == other); }
	//accessors
	public:
		std::string get_nick( void ) const { return m_nick; }
		std::string get_user( void ) const { return m_user; }
		std::string get_realname( void ) const { return m_realname; }
		std::string get_hostname( void ) const { return m_hostname; }
		std::string get_servername( void ) const { return m_servername; }
		int			get_clientSocket( void ) const { return m_clientSocket; }
		int			get_reg_steps( void ) const { return m_reg_steps; }
		bool		get_registered( void ) const { return m_registered; }
		bool		get_connected( void ) const { return m_connected; }
		void		set_reg(int step) { m_reg_steps = step; }
		void		set_nick(std::string nick) { m_nick = nick; }
		void		set_user(std::string user) { m_user = user; }
		void		set_registered(bool reg) { m_registered = reg; }
		void		set_connected(bool conn) { m_connected = conn; }
		void		set_hostname(std::string hostname) { m_hostname = hostname; }
		void		set_servername(std::string servername) { m_servername = servername; }
		void		set_realname(std::string realname) { m_realname = realname; }
};

std::ostream &operator<<(std::ostream &out, Client const &src);

inline std::ostream &operator<<(std::ostream &out, std::vector<Client*> clients) {
	for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		out << " ["<<(*it)->get_nick() << "] ";
	}
	return out;
}
