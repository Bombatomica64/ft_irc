/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coucou.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/09 18:25:55 by lmicheli          #+#    #+#             */
/*   Updated: 2024/09/11 15:23:13 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cuocuo.hpp>

Coucou::Coucou()
{
	m_name = "Coucou";
	m_angry_name = "Anouk";
	m_happy_name = "lollo";
	
	m_angry_words.push_back("Why would you say that?");
	m_angry_words.push_back("I'm not happy with you");
	m_angry_words.push_back("I'm angry");
	m_angry_words.push_back("I can't believe you did that!");
	m_angry_words.push_back("That was uncalled for!");
	m_angry_words.push_back("How could you?");
	m_angry_words.push_back("c'e' troppa frociaggine in questo canale");
	
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
	m_base_respones.push_back("Ok");


	m_angry_input.insert("scemo");
	m_angry_input.insert("stupido");
	m_angry_input.insert("idiota");
	m_angry_input.insert("cretino");
	m_angry_input.insert("deficiente");
	m_angry_input.insert("imbecille");
	m_angry_input.insert("scemotto");
	m_angry_input.insert("scemone");
	m_angry_input.insert("scemino");
	m_angry_input.insert("scemuccio");
	m_angry_input.insert("scemarello");
	m_angry_input.insert("stronzo");
	m_angry_input.insert("vaffanculo");
	m_angry_input.insert("cazzo");
	m_angry_input.insert("merda");
	m_angry_input.insert("coglione");
	m_angry_input.insert("frocio"); //l'ha detto il papa quindi è ok
	m_angry_input.insert("rincoglionito");
	m_angry_input.insert("puttana");
	m_angry_input.insert("troia");
	m_angry_input.insert("zoccola");
	m_angry_input.insert("cagna");
	m_angry_input.insert("pirla");
	m_angry_input.insert("ritardato");
	m_angry_input.insert("mongolo");
	m_angry_input.insert("acustico");
	m_angry_input.insert("stupid");
	m_angry_input.insert("idiot");
	m_angry_input.insert("cretin");
	m_angry_input.insert("moron");
	m_angry_input.insert("dumb");
	m_angry_input.insert("retarded");
	m_angry_input.insert("fool");
	m_angry_input.insert("dunce");
	m_angry_input.insert("imbecile");
	m_angry_input.insert("python");

	m_happy_input.insert("bravo");
	m_happy_input.insert("ottimo");
	m_happy_input.insert("fantastico");
	m_happy_input.insert("eccellente");
	m_happy_input.insert("meraviglioso");
	m_happy_input.insert("splendido");
	m_happy_input.insert("stupendo");
	m_happy_input.insert("straordinario");
	m_happy_input.insert("eccezionale");
	m_happy_input.insert("magnifico");
	m_happy_input.insert("superlativo");
	m_happy_input.insert("incredibile");
	m_happy_input.insert("impressionante");
	m_happy_input.insert("formidabile");
	m_happy_input.insert("favoloso");
	m_happy_input.insert("spettacolare");
	m_happy_input.insert("sorprendente");
	m_happy_input.insert("strabiliante");
	m_happy_input.insert("sbalorditivo");
	m_happy_input.insert("grazie");
	m_happy_input.insert("thank");
	m_happy_input.insert("thanks");
	m_happy_input.insert("gracias");
}

Coucou::~Coucou()
{
	m_angry_words.clear();
	m_happy_words.clear();
	
}

void	Coucou::change_relation(Client& client, std::vector<std::string> words, std::string message) //controlla lo stato se va cambiato o no? AAAAAAA o offese generiche. 
{
	if (m_relations.find(client.get_nick()) == m_relations.end())
	{
		m_relations.insert(std::pair<std::string, relation>(client.get_nick(), NORMAL));
	}
	if (words[0] == "DOMAIN")
	{
		domain_expansion(client);
	}
	else if (words[0] == "RELATION")
	{
		print_relations(client);
	}

	int nUpper = 0;
	for (std::string::iterator it = message.begin(); it != message.end(); it++)
	{
		if (isupper(*it))
			nUpper++;
	}
	if (nUpper > 5)
	{
		m_relations[client.get_nick()] = ANGRY;
		return;
	}
	
	for (std::vector<std::string>::iterator it = words.begin(); it != words.end(); it++)
	{
		if (m_angry_input.find(*it) != m_angry_input.end())
		{
			m_relations[client.get_nick()] = ANGRY;
			return;
		}
	}
	for (std::vector<std::string>::iterator it = words.begin(); it != words.end(); it++)
	{
		if (m_happy_input.find(*it) != m_happy_input.end())
		{
			m_relations[client.get_nick()] = HAPPY;
			return;
		}
	}
	m_relations[client.get_nick()] = NORMAL;
	
}

void	Coucou::parse_message(Client& client, std::string message)
{
	std::vector<std::string> words = split(message, " ");
	std::cerr << words << std::endl;
	
	change_relation(client, words, message);
	
	switch (m_relations[client.get_nick()])
	{
		case HAPPY:
			send_message(client, m_happy_words[rand() % m_happy_words.size()]);
			break;
		case ANGRY:
			send_message(client, m_angry_words[rand() % m_angry_words.size()]);
			break;
		case NORMAL:
			send_message(client, m_base_respones[rand() % m_base_respones.size()]);	
			break;
		default:
			std::cout << "default" << std::endl;
	}
}

void	Coucou::send_message(Client& client, std::string message)
{
	message.insert(0, ":Coucou!coucou@Coucou PRIVMSG " + client.get_nick() + " :");
	client.send_message(message);
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

void	Coucou::domain_expansion(Client& client)
{
	std::cerr << "DOMAIN EXPANSION" << std::endl;
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
