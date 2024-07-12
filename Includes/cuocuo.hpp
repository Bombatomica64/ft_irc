/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cuocuo.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mruggier <mruggier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/09 18:26:18 by lmicheli          #+#    #+#             */
/*   Updated: 2024/07/12 12:51:42 by mruggier         ###   ########.fr       */
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
	private:
		std::string m_name;
		std::string m_angry_name;
		std::string m_happy_name;
		std::vector<std::string> m_angry_words;
		std::vector<std::string> m_happy_words;
		std::vector<std::string> m_base_respones;
		std::map<std::string, relation> m_relations; // true = happy, false = angry
	public:
		Coucou();
		~Coucou();
		void	send_message(Client Client, std::string message);
		void	parse_message(Client Client, std::string message);
		void	change_relation(std::string name);
		void	add_angry_word(std::string word);
		void	add_happy_word(std::string word);
		void	remove_angry_word(std::string word);
		void	remove_happy_word(std::string word);
		void	domain_expansion(Client client, std::string message);
		void	print_relations(Client client);
		std::string get_name( Client client ) const;
};