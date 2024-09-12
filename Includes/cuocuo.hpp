/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cuocuo.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/09 18:26:18 by lmicheli          #+#    #+#             */
/*   Updated: 2024/09/12 17:36:14 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

#include <Client.hpp>

typedef enum e_relation
{
	HAPPY,
	NORMAL,
	ANGRY
}		relation;

class Coucou
{
	//network stuff
	private:
		std::vector<pollfd>		m_fds;
		int			m_socket;
		// int			m_port;
		std::string	m_ip;
		struct sockaddr_in	m_server_addr;

	public:
		void start_network(std::string info[3]);
		void recieve_message( void );

	//bot stuff
	private:
		std::string m_name;
		std::string m_angry_name;
		std::string m_happy_name;
		std::vector<std::string> m_angry_words;
		std::vector<std::string> m_happy_words;
		std::vector<std::string> m_base_respones;
		std::map<std::string, relation> m_relations;
		std::set<std::string> m_angry_input;
		std::set<std::string> m_happy_input;
	public:
		Coucou(std::string info[3]);
		~Coucou();
		void	send_message(Client& Client, std::string message);
		void	parse_message(Client& Client, std::string message);
		void	change_relation(Client& client, std::vector<std::string> words, std::string message);
		void	domain_expansion(Client& client);
		void	print_relations(Client& client);
		std::string get_name(Client& client) const;
};
		//possible functions

		// void	add_angry_word(std::string word);
		// void	add_happy_word(std::string word);
		// void	remove_angry_word(std::string word);
		// void	remove_happy_word(std::string word);
