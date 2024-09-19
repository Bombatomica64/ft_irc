/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coucou.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/09 18:25:55 by lmicheli          #+#    #+#             */
/*   Updated: 2024/09/19 14:14:17 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cuocuo.hpp>

Coucou::Coucou(std::string info[3])
{
	srand(time(NULL));

	m_new_nick = "Coucou";
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
	
	start_network(info);
}

Coucou::~Coucou()
{
	m_angry_words.clear();
	m_happy_words.clear();
	
}

void	Coucou::change_relation(Client& client, std::vector<std::string> words, std::string message)
{
	if (m_relations.find(client.get_nick()) == m_relations.end())
	{
		m_relations.insert(std::pair<std::string, relation>(client.get_nick(), NORMAL));
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


// :cazzo!cazzo@cazzo PRIVMSG Coucou :RELATION
void	Coucou::recieve_message( void )
{
	char buffer[1024];
	ssize_t bytes = recv(m_socket, buffer, 1024, 0);
	if (bytes == -1)
	{
		throw std::runtime_error("Failed to recieve message.");
	}
	buffer[bytes] = '\0';
	std::string message(buffer);
	if (message.find("PRIVMSG") != std::string::npos)
	{
		std::vector<std::string> split_msg = split(message, " ");
		std::string nick = split_msg[0].substr(1, split_msg[0].find("!") - 1);
		std::string user = split_msg[0].substr(split_msg[0].find("!") + 1, split_msg[0].find("@") - split_msg[0].find("@") - 1);
		std::string host = split_msg[0].substr(split_msg[0].find("@") + 1, split_msg[0].find(" ") - split_msg[0].find("@") - 1);
		Client client(nick, user, host);
		parse_message(client, message.substr(message.find_last_of(":") + 1, message.size()));
	}
	else
	{
		if (message.find("433") != std::string::npos)
		{
			
			m_new_nick = m_new_nick + "_";
			std::string nickname = "NICK " + m_new_nick + "\r\n";
			
			if (send(m_socket, nickname.c_str(), nickname.size(), 0) == -1)
			{
				throw std::runtime_error("Failed to authenticate.");
			}
		}
		else if (message.find(":irc 004") != std::string::npos)
		{
			std::string me_bot = "BOTINFO Coucou 1.0\r\n";
			if (send(m_socket, me_bot.c_str(), me_bot.size(), 0) == -1)
			{
				throw std::runtime_error("Failed to authenticate.");
			}
		}
	}
}

void	Coucou::parse_message(Client& client, std::string message)
{
	std::vector<std::string> words = split(message, " ");
	std::cerr << words << std::endl;
	if (words[0] == "DOMAIN")
	{
		domain_expansion(client);
		return;
	}
	else if (words[0] == "RELATION")
	{
		print_relations(client);
		return;
	}
	
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

//todo add back :Coucou!coucou@Coucou 
void	Coucou::send_message(Client& client, std::string message)
{
	std::cerr << "sending message: "<< message << std::endl;
	if (client.get_nick() == m_new_nick)
		return;
	if (message.find("DELIMITER") == std::string::npos)
	{
		message.insert(0, "PRIVMSG " + client.get_nick() + " :");
		message.append("\r\n");
		if (send(m_socket, message.c_str(), message.size(), 0) == -1)
		{
			throw std::runtime_error("Failed to send message.");
		}
		return;
	}
	std::vector<std::string> split_msg = split(message, "DELIMITER");
	for (std::vector<std::string>::iterator it = split_msg.begin(); it != split_msg.end(); it++)
	{
		it->insert(0, "PRIVMSG " + client.get_nick() + " :");
		it->append("\r\n");
		if (send(m_socket, it->c_str(), it->size(), 0) == -1)
		{
			throw std::runtime_error("Failed to send message.");
		}
	}
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

//todo fix
void	Coucou::print_relations(Client& client)
{
	std::string response;
	std::map<std::string, relation>::iterator it = m_relations.begin();
	while (it != m_relations.end())
	{
		response += "I'm ";
		if (it->second == HAPPY)
			response += "happy with " + it->first + "DELIMITER";
		else if (it->second == ANGRY)
			response += "angry with " + it->first + "DELIMITER";
		else
			response += "normal with " + it->first + "DELIMITER";
		it++;
	}
	send_message(client, response);
}

void Coucou::start_network(std::string info[3])
{
	m_server_addr.sin_family = AF_INET;
	m_server_addr.sin_port = htons(std::strtol(info[1].c_str(), NULL, 10));
	m_server_addr.sin_addr.s_addr = inet_addr(info[0].c_str());
	// if (inet_pton(AF_INET, info[0].c_str(), &m_server_addr.sin_addr) <= 0)
	// {
	// 	throw std::runtime_error("Invalid address/Address not supported.");
	// }
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == -1)
	{
		throw std::runtime_error("Failed to create socket.");
	}
	if (connect(m_socket, reinterpret_cast<struct sockaddr*>(&m_server_addr), sizeof(m_server_addr)) < 0)	{
		throw std::runtime_error("Connection failed.");
	}
	if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &m_server_addr, sizeof(m_server_addr)) < 0)
	{
		throw std::runtime_error("Failed to set socket options.");
	}
	if (fcntl(m_socket, F_SETFL, O_NONBLOCK) < 0)
	{
		throw std::runtime_error("Failed to set socket to non-blocking.");
	}

	std::string startUpMessage = "PASS " + info[2] + "\r\nNICK Coucou\r\nUSER Coucou the best :Coucou\r\n";
	
	if (send(m_socket, startUpMessage.c_str(), startUpMessage.size(), 0) == -1)
	{
		throw std::runtime_error("Failed to authenticate.");
	}
	
	pollfd poll_fd = {m_socket, POLLIN, 0};
	m_fds.push_back(poll_fd);
	while (true)
	{
		if(poll(m_fds.data(), m_fds.size(), -1) == -1)
		{
			throw std::runtime_error("Poll failed.");
		}
		for (std::vector<pollfd>::iterator it = m_fds.begin(); it != m_fds.end(); it++)
		{
			if (it->revents & POLLIN)
			{
				try
				{
					recieve_message();
				}
				catch(const std::exception& e)
				{
					if (e.what() == std::string("Bot closed"))
						throw std::runtime_error("Bot disconnected");
					std::cerr << e.what() << '\n';
				}
			}
		}
	}
	
}
