/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 11:01:39 by lmicheli          #+#    #+#             */
/*   Updated: 2024/06/25 19:01:28 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <server.hpp>
# include <arpa/inet.h>
# include <fcntl.h>
# include <unistd.h>
# include <sys/socket.h>
# include <cstdlib>
# include <poll.h>
# include <vector>
# include <netinet/in.h> 
# include <main.hpp>

class Server
{
	private:
		int			m_socket;
		int			m_port;
		std::string m_psw;
		struct sockaddr_in m_addr;
		Server() {}
		Server(Server const &src);
		Server &operator=(Server const &src);

	public:
		Server(char *port, char *psw);
		~Server();
		void	create_socket( void );
		void	bind_socket( void );
		void	accept_connection( void );
		void	read_from_client(int client);

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
};

#endif