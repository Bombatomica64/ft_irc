/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coucou.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mruggier <mruggier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/09 18:25:55 by lmicheli          #+#    #+#             */
/*   Updated: 2024/07/29 17:56:29 by mruggier         ###   ########.fr       */
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

void	change_relation(std::string name) //controlla lo stato se va cambiato o no? AAAAAAA o offese generiche. 
{
	(void)name;
}

void	Coucou::parse_message(Client& client, std::string message)
{
	std::vector<std::string> words = split(message, " ");
	std::cerr << words << std::endl;
	return;
	
	if (m_relations.find(client.get_nick()) == m_relations.end())
	{
		m_relations[client.get_nick()] = NORMAL;
	}
	if ( words[1] == "DOMAIN")
	{
		domain_expansion(client, message);
	}
	else if (words[1] == "RELATION")
	{
		print_relations(client);
	}
	
	switch (m_relations[client.get_nick()])
	{
		case HAPPY:
			
			break;
		case ANGRY:
			
			break;
		case NORMAL:
			
			break;
		default:
			std::cout << "default" << std::endl;
	}
}

void	Coucou::domain_expansion(Client& client, std::string msg)
{
	std::vector<std::string> words = split(msg, " ");
	std::string response;
	if (m_relations[client.get_nick()] == HAPPY)
	{
		std::fstream file("docs/meme", std::ios::in);
		std::getline(file, response, '\0');
		response.insert(0, "Through heaven and earth i alone am the honored one");
	}
	else if (m_relations[client.get_nick()] == ANGRY)
	{
		std::fstream file("docs/A_resp", std::ios::in);
		std::getline(file, response, '\0');
		response.insert(0, "DOMAIN EXPANSION:  \n");
		response.append("ENGLISH OR SPANISH?\n");
	}
	else
	{
		std::fstream file("docs/Nah_id_win", std::ios::in);
		std::getline(file, response, '\0');
	}
	send_message(client, response);
}

void	Coucou::send_message(Client& client, std::string message)
{
	message.insert(0, ":" + this->get_name(client) + "!" + client.get_nick()  );
	client.send_message(message);
}

void	Coucou::print_relations(Client& client)
{
	std::string response;
	std::map<std::string, relation>::iterator it = m_relations.begin();
	while (it != m_relations.end())
	{
		response += it->first + " is ";
		if (it->second == HAPPY)
			response += "happy with them\n";
		else if (it->second == ANGRY)
			response += "angry with them\n";
		else
			response += "normal with them\n";
		it++;
	}
	send_message(client, response);
}

std::string Coucou::get_name(Client& client) const
{
	relation rel = m_relations.at(client.get_nick());
	switch (rel)
	{
	case HAPPY:
		return m_happy_name;
	case ANGRY:
		return m_angry_name;
	case NORMAL:
		return m_name;
		break;	
	default:
		return m_name;
		std::cout << "????????????????" << std::endl;
		break;
	}
}