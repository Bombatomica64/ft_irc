/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mruggier <mruggier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 11:01:39 by lmicheli          #+#    #+#             */
/*   Updated: 2024/08/23 17:33:52 by mruggier         ###   ########.fr       */
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
		
		//password
		std::string					m_salt;
		std::string					m_hash;

		// clients
		std::vector<struct pollfd>	m_client_fds;
		std::map<int, Client*>		m_clients;
		
		//channels
		std::map<std::string, Channel*> m_channels;
		
		//commands
		std::map<std::string, bool (Server::*)(int, std::string)> m_cmds;
		
		//Net stuff
		struct pollfd		m_server_fd;
		int					m_socket;
		int					m_port;
		struct sockaddr_in	m_addr;
		std::string			m_ip;

		//bot
		Coucou				m_coucou;
		
		//other
		std::string			m_date;
		std::vector<t_file> m_files;

	public:
		Server() {} //useless
		Server(std::string port, std::string psw);
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
		void		remove_channel(std::string name);
		std::string	get_ip( void ) const ;
		std::string	get_date( void ) const { return m_date; }
		int			get_port( void ) const ;
		// bool		send_msg_to_all(int client, std::string msg);
	/**
	 * @brief client functions
	 */
	public:
		void	read_from_client(int client);
		void	write_to_client(int client, std::string msg);
		void	register_client(int client);
		void	parse_cmds(int client, std::string cmd);
		void	send_msg_to_set(std::set<std::string> clients, std::string msg);
		Client*	get_client_by_nick(std::string nick);
		bool	is_client_in_channel(std::string nick, std::string channel);
		bool	client_exist(const std::string &nick) const;
		std::string	getNamesMessage(Channel* chan, int client, std::set<std::string>& client_names);
		
		
		/**
		 * @brief Sends a private message to a client.
		 * 
		 * @note PRIVMSGㅤ<receiver>{,<receiver>}ㅤ:<message>
		 * @note users or channels
		 * 
		 * @warning 
		 */
		bool privmsg(int client, std::string cmd);
		
		/**
		 * @brief Sends a message to a channel.
		 * 
		 * @note JOINㅤ#<channel>{,#<channel>}ㅤ[<key>{,<key>}]
		 * 
		 * @warning 
		 */
		bool join(int client, std::string cmd);
		
		/**
		 * @brief Leaves a channel.
		 * 
		 * @note PARTㅤ#<channel>{,#<channel>}
		 * 
		 * @warning 
		 */
		bool part(int client, std::string cmd);
		
		/**
		 * @brief Changes the mode of a channel or user.
		 * 
		 * @note MODEㅤ#<channel>ㅤ{[+|-]|o|i|t|k} [<user>]
		 * 
		 * @warning
		 */
		bool mode(int client, std::string cmd);
		
		/**
		 * @brief Disconnects a client from the server.
		 * 
		 * @note QUITㅤ[:<message>]
		 * 
		 * @warning
		 */
		bool quit(int client, std::string cmd);
		
		/**
		 * @brief Invites a client to a channel.
		 * 
		 * @note INVITEㅤ<nickname>ㅤ#<channel>
		 * 
		 * @warning 
		 */
		bool invite(int client, std::string cmd);
		
		/**
		 * @brief Changes the topic of a channel.
		 * 
		 * @note TOPICㅤ#<channel>ㅤ[:<topic>]
		 * 
		 * @warning
		 */
		bool topic(int client, std::string cmd);
		
		/**
		 * @brief Kicks a client from a channel.
		 * 
		 * @note KICKㅤ#<channel>ㅤ<user>ㅤ[:<message>]
		 * 
		 * @warning #<channel>{,#<channel>} <user>{,<user>} [:<comment>]
		 */
		bool kick(int client, std::string cmd);
		
		/**
		 * @brief Lists the clients in a channel.
		 * 
		 * @note NAMESㅤ[#<channel>{,#<channel>}]
		 * 
		 * @warning 
		 */
		bool names(int client, std::string cmd);
		
		/**
		 * @brief Registers a password.
		 * 
		 * @note PASSㅤ<password>
		 * 
		 * @warning crash if not enough parameters (PASS e basta)
		 */
		bool pass(int client, std::string cmd);
		
		/**
		 * @brief Registers a user.
		 * 
		 * @note USERㅤ<username>ㅤ<hostname>ㅤ<servername>ㅤ<realname>
		 * 
		 * @warning
		 */
		bool user(int client, std::string cmd);
		
		/**
		 * @brief Registers a nickname.
		 * 
		 * @note konversation
		 * 
		 * @warning 
		 */
		bool cap(int client, std::string cmd);
		
		/**
		 * @brief Retrieves information about a client.
		 * 
		 * @note WHOㅤ<client>ㅤ(parziale)
		 * 
		 * @warning 
		 */
		bool who(int client, std::string cmd);
		
		/**
		 * @brief Pings a client.
		 * 
		 * @note PINGㅤ<client>ㅤ(parziale)
		 * 
		 * @warning 
		 */
		bool ping(int client, std::string cmd);
		
		/**
		 * @brief Retrieves the user host information.
		 * 
		 * @note NICKㅤ<nickname>
		 * 
		 * @warning 
		 */
		bool nick(int client, std::string cmd);
		
		/**
		 * @brief Retrieves the user host information.
		 * 
		 * @note USERHOSTㅤ<nickname> (parziale)
		 * 
		 * @warning 
		 */
		bool userhost(int client, std::string cmd);

		void	handle_file_request(int client, std::string cmd, std::string receiver);
		void	transfer_file(t_file file);
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
		class BindException : public std::exception
		{
			virtual const char *what() const throw()
			{
				return "Error: bind failed";
			}
		};

		class ListenException : public std::exception
		{
			virtual const char *what() const throw()
			{
				return "Error: listen failed";
			}
		};

		class FcntlException : public std::exception
		{
			virtual const char *what() const throw()
			{
				return "Error: fcntl failed";
			}
		};
};
