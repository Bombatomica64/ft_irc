/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/26 17:01:54 by lmicheli          #+#    #+#             */
/*   Updated: 2024/06/26 17:45:21 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <main.hpp>

class Channel
{
	private:
		std::string m_name;
		std::vector<Client> m_clients;
		std::map<char, bool> m_modes; 
		Channel() {}
		Channel(Channel const &src);
		Channel &operator=(Channel const &src);

	public:
		Channel(std::string name, std::map<char, bool> modes);
		~Channel();
		void	add_client(Client client);
		void	remove_client(Client client);
		void	send_message(Client sender, std::string message);
		void	join_channel(Client client);
		void	leave_channel(Client client);
		bool	modify_mode(char mode, bool value, std::string param, Client client);
};

#endif