/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cuocuo.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/09 18:26:18 by lmicheli          #+#    #+#             */
/*   Updated: 2024/07/10 10:29:26 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

#include <server.hpp>

typedef enum e_relation
{
	HAPPY,
	NORMAL,
	ANGRY
}		relation;

class Coucou
{
	private:
		std::map<std::string, relation> m_relations;
		std::string m_name;
		std::string m_angry_name;
		std::string m_happy_name;
		std::vector<std::string> m_angry_words;
		std::vector<std::string> m_happy_words;
		std::vector<std::string> m_base_respones;
		std::map<std::string, bool> m_relations; // true = happy, false = angry
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
};