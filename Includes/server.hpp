/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 11:01:39 by lmicheli          #+#    #+#             */
/*   Updated: 2024/06/25 12:40:43 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <main.hpp>

class Server
{
	private:
		int			m_socket;
		int			m_port;
		std::string m_psw;
		Server() {}
		Server(Server const &src);
		Server &operator=(Server const &src);

	public:
		Server(char *port, char *psw);
		~Server();
		void	create_socket();
		void	bind_socket();
};

#endif