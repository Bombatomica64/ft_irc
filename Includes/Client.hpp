/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/26 16:20:05 by lmicheli          #+#    #+#             */
/*   Updated: 2024/06/28 17:56:29 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <main.hpp>

class Client : public Server
{
	private:
		int			m_clientSocket;
		struct sockaddr_in m_clientAddr;
		int	m_reg_steps;
		std::string m_nick;
		std::string m_user;
		bool		m_registered;
		bool		m_connected;
		Client() {}
		Client(Client const &src);
		Client &operator=(Client const &src);

	public:
		Client(int clientSocket, struct sockaddr_in clientAddr);
		~Client();
		void	connect_to_server( void );
		void	send_message( void );
		void	receive_message( void );
		void	connect_to_channel( void );
		void	quit( void );

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
};

#endif