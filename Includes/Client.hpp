/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/26 16:20:05 by lmicheli          #+#    #+#             */
/*   Updated: 2024/07/02 12:54:30 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <server.hpp>

class Server;

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
		Server		*m_server;

	public:
		Client() {}
		Client(Client const &src);
		Client &operator=(Client const &src);
		bool	operator==(Client const &src) const { return m_clientSocket == src.m_clientSocket; }
		Client(int clientSocket, struct sockaddr_in clientAddr, Server *server);
		~Client();
		bool	send_message(std::string message);
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
		void	parse_cmds(std::string cmd);
		bool	quit(std::string message);
		bool	join(std::string channel);
		bool	part(std::string channels);
		bool	privmsg(std::string message);
		bool	nick(std::string new_nick);
		// bool	oper(std::string user);
		bool	mode(std::string message);
		bool	invite(std::string message);
		// bool	kick(std::string message);
		bool	ping(std::string message);
		// bool	topic(std::string message);


	//accessors
	public:
		std::string get_nick( void ) const { return m_nick; }
		std::string get_user( void ) const { return m_user; }
		int			get_clientSocket( void ) const { return m_clientSocket; }
		int			get_reg_steps( void ) const { return m_reg_steps; }
		bool		get_registered( void ) const { return m_registered; }
		bool		get_connected( void ) const { return m_connected; }
		void		set_reg(int step) { m_reg_steps = step; }
		void		set_nick(std::string nick) { m_nick = nick; }
		void		set_user(std::string user) { m_user = user; }
		void		set_registered(bool reg) { m_registered = reg; }
		void		set_connected(bool conn) { m_connected = conn; }
		void		set_realname(std::string realname) { m_realname = realname; std::cout << "realname: " << m_realname << std::endl;}
		void		set_hostname(std::string hostname) { m_hostname = hostname; std::cout << "hostname: " << m_hostname << std::endl;}
		void		set_servername(std::string servername) { m_servername = servername; std::cout << "servername: " << m_servername << std::endl;}
};

std::ostream &operator<<(std::ostream &out, Client const &src);

#endif