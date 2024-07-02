/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/26 17:01:54 by lmicheli          #+#    #+#             */
/*   Updated: 2024/07/02 12:01:29 by lmicheli         ###   ########.fr       */
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
		bool	send_message(Client sender, std::string message);
		void	join_channel(Client client);
		void	leave_channel(Client client);
		bool	modify_mode(std::vector<std::string> command, Client client);
};

#endif