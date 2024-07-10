/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coucou.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/09 18:25:55 by lmicheli          #+#    #+#             */
/*   Updated: 2024/07/10 10:33:34 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cuocuo.hpp>

Coucou::Coucou()
{
	srand(time(NULL));
	m_name = "Coucou";
	m_angry_name = "Anouk";
	m_happy_name = "lollo";
	m_angry_words.push_back("Why would you say that?");
	m_angry_words.push_back("I'm not happy with you");
	m_angry_words.push_back("I'm angry");
	m_angry_words.push_back("I can't believe you did that!");
	m_angry_words.push_back("That was uncalled for!");
	
	
	m_happy_words.push_back("I'm happy");
	m_happy_words.push_back("I'm glad you're here");
	m_happy_words.push_back("You made my day!");
	m_happy_words.push_back("I'm so happy to see you!");
	m_happy_words.push_back("I'm so happy to hear that!");
	
	
	m_base_respones.push_back("I don't know what to say");
	m_base_respones.push_back("Sure");
	m_base_respones.push_back("I'm not sure");
	m_base_respones.push_back("It's a possibility");
	m_base_respones.push_back("Zzzzzzz....");
}

Coucou::~Coucou()
{
	m_angry_words.clear();
	m_happy_words.clear();
}

void	Coucou::parse_message(Client client, std::string message)
{
	std::vector<std::string> words = split(message, " ");
	if (m_relations.find(client.get_nick()) == m_relations.end())
	{
		m_relations[client.get_nick()] = NORMAL;
	}
	switch (m_relations[client.get_nick()])
	{
		case HAPPY:
			if (std::find(m_angry_words.begin(), m_angry_words.end(), words[0]) != m_angry_words.end())
			{
				change_relation(client.get_nick());
			}
			break;
		case ANGRY:
			if (std::find(m_happy_words.begin(), m_happy_words.end(), words[0]) != m_happy_words.end())
			{
				change_relation(client.get_nick());
			}
			break;
		case NORMAL:
			if (std::find(m_angry_words.begin(), m_angry_words.end(), words[0]) != m_angry_words.end())
			{
				change_relation(client.get_nick());
			}
			else if (std::find(m_happy_words.begin(), m_happy_words.end(), words[0]) != m_happy_words.end())
			{
				change_relation(client.get_nick());
			}
			break;
	}
}