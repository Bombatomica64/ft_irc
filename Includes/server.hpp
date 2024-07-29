/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 11:01:39 by lmicheli          #+#    #+#             */
/*   Updated: 2024/07/29 12:58:15 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include <main.hpp>
# include <Client.hpp>
# include <Channel.hpp>
# include <cuocuo.hpp>
# include <ctime>

class Client;
class Channel;

class Server
{
	private:
		std::string					m_salt;
		std::string					m_hash;
		std::vector<struct pollfd>	client_fds;
		struct pollfd 				server_fd;
		std::map<int, Client*>		m_clients;
		std::map<std::string, Channel*> m_channels;
		std::map<std::string, bool (Server::*)(int, std::string)> m_cmds;
		std::string					m_date;
		Coucou						m_coucou;

	protected :
		std::set<std::string> m_commands;
		int			m_socket;
		int			m_port;
		struct sockaddr_in m_addr;
		Server() {}
		//Server(Server const &src);
		//Server &operator=(Server const &src);

	public:
		Server(char *port, char *psw);
		~Server();

	public:
		void	create_socket(void);
		void	bind_socket(void);
		void	accept_connection( void );
		void	get_cmds();

	/**
	 * @brief server functions
	 */
	public:
		Channel*	get_channel(std::string name);
		void		add_channel(std::string name);
		void		add_channel(std::string name, std::map<char, int> modes);
		bool		send_msg_to_channel(int client, std::string Channel ,std::string msg);
	/**
	 * @brief client functions
	 */
	public:
		void	read_from_client(int client);
		void	write_to_client(int client, std::string msg);
		void	register_client(int client);
		void	parse_cmds(int client, std::string cmd);
		Client*	get_client_by_nick(std::string nick);
		bool	is_client_in_channel(std::string nick, std::string channel);
		bool	client_exist(const std::string &nick) const;
		bool	privmsg(int client, std::string cmd);
		bool	join(int client, std::string cmd);
		bool	part(int client, std::string cmd);
		bool	mode(int client, std::string cmd);
		bool	quit(int client, std::string cmd);
		bool	invite(int client, std::string cmd);
		bool	topic(int client, std::string cmd);
		bool	kick(int client, std::string cmd);
		bool	names(int client, std::string cmd);
		bool	pass(int client, std::string cmd);
		bool	user(int client, std::string cmd);
		bool	cap(int client, std::string cmd);
		bool	who(int client, std::string cmd);
		bool	ping(int client, std::string cmd);
		bool	userhost(int client, std::string cmd);


	public:
		class clientSocketException : public std::exception
		{
			virtual const char *what() const throw()
			{
				return "Error: client socket non-blocking failed";
			}
		};
		class PollException : public std::exception
		{
			virtual const char *what() const throw()
			{
				return "Error: poll failed";
			}
		};
		class ClientException : public std::exception
		{
			virtual const char *what() const throw()
			{
				return "Error: client failed";
			}
		};
};