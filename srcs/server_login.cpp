#include <server.hpp>

Server::~Server()
{
	close(m_socket);
	for (std::map<int, Client *>::iterator it = m_clients.begin(); it != m_clients.end(); ++it)
	{
		it->second->send_message("Server shutting down");
		close(it->first);
		delete it->second;
	}
	m_clients.clear();
	m_client_fds.clear();
	for (std::map<std::string, Channel *>::iterator it = m_channels.begin(); it != m_channels.end(); ++it)
		delete it->second;
	m_channels.clear();
}

Server::Server(std::string port, std::string psw)
{
	// Variables
	m_coucou = Coucou();
	get_cmds();
	get_cmds_help();
	m_date = get_time(0);

	// Input validation
	check_input(port, psw);
	m_port = std::strtold(port.c_str(), NULL);

	// Password hashing
	m_salt = generate_salt(16);
	m_hash = hash_password(psw.c_str(), m_salt);
	create_socket();
}

void Server::create_socket(void)
{
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == -1)
	{
		std::cerr << "Error: socket creation failed" << std::endl;
		exit(1);
	}
	bind_socket();
}

// inet_pton(AF_INET, "10.12.2.10", &m_addr.sin_addr);
// if (bind(m_socket, (struct sockaddr *)&m_addr, sizeof(m_addr)) == -1)
// {
// 	std::cerr << "Error: socket binding failed - " << strerror(errno) << std::endl;
// 	throw Server::BindException();
// }

void Server::bind_socket(void)
{
	int optval = 1;
	setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	m_addr.sin_family = AF_INET;
	m_addr.sin_port = htons(m_port);

	char hostname[256];
	if (gethostname(hostname, sizeof(hostname)) == -1)
	{
		std::cerr << "Error: gethostname failed - " << strerror(errno) << std::endl;
		throw std::runtime_error("Hostname retrieval failed");
	}

	struct hostent *host = gethostbyname(hostname);
	if (host == NULL)
	{
		std::cerr << "Error: gethostbyname failed - " << hstrerror(h_errno) << std::endl;
		throw std::runtime_error("Host information retrieval failed");
	}

	struct in_addr **addr_list = reinterpret_cast<struct in_addr **>(host->h_addr_list);
	if (addr_list[0] == NULL)
	{
		std::cerr << "Error: no IP addresses found for host" << std::endl;
		throw std::runtime_error("No IP addresses found");
	}

	m_addr.sin_addr = *addr_list[0];

	if(bind(m_socket, reinterpret_cast<struct sockaddr *>(&m_addr), sizeof(m_addr)) == -1)
	{
		std::cerr << "Error: socket binding failed - " << strerror(errno) << std::endl;
		throw Server::BindException();
	}

	if (listen(m_socket, 10) == -1)
	{
		std::cerr << "Error: socket listening failed - " << strerror(errno) << std::endl;
		throw Server::ListenException();
	}
	if (fcntl(m_socket, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "Error: setting socket to non-blocking failed - " << strerror(errno) << std::endl;
		throw Server::FcntlException();
	}
}

void Server::accept_connection()
{
	// these do stuff don't touch
	int ret = 0;
	int optval = 1;
	struct sockaddr_in client_addr = {};
	socklen_t client_addr_size;
	int client_socket;
	struct pollfd client_fd = {};

	// add server to poll
	m_server_fd.fd = m_socket;
	m_server_fd.events = POLLIN;
	m_client_fds.push_back(m_server_fd);
	m_ip = inet_ntoa(m_addr.sin_addr);
	printLogo(m_ip, m_port);

	while (true)
	{
		// poll for events
		ret = poll(m_client_fds.data(), m_client_fds.size(), -1);
		if (ret == -1)
			throw Server::PollException();

		// check for events
		for (size_t i = 0; i < m_client_fds.size(); ++i)
		{
			if ((m_client_fds[i].revents & POLLIN))
			{
				// new connection
				if (m_client_fds[i].fd == m_socket)
				{
					client_addr_size = sizeof(client_addr);
					client_socket = accept(m_socket, reinterpret_cast<struct sockaddr *>(&client_addr), &client_addr_size);
					if (client_socket == -1)
					{
						continue;
					}
					setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
					if (fcntl(client_socket, F_SETFL, O_NONBLOCK) == -1)
					{
						close(client_socket);
						throw Server::FcntlException();
					}
					client_fd.fd = client_socket;
					client_fd.events = POLLIN;
					m_client_fds.push_back(client_fd);
					m_clients[client_socket] = new Client(client_socket, client_addr);
				}
				// existing connection
				else
				{
					try
					{
						if (m_clients[m_client_fds[i].fd]->get_registered() == false || m_clients[m_client_fds[i].fd]->get_connected() == false)
							register_client(m_client_fds[i].fd);
						else
						{
							read_from_client(m_client_fds[i].fd);
						}
					}
					catch (const std::exception &e)
					{
						std::cerr << e.what() << std::endl;
						if (typeid(e) == typeid(Server::ClientException))
						{
							this->quit(m_client_fds[i].fd, "QUIT");
						}
						std::string msg(e.what());
						if (msg == "close")
							throw std::runtime_error("close");
						// return;
					}
				}
			}
		}
	}
}

void Server::read_from_client(int client)
{
	if (m_clients[client]->get_registered() == false || m_clients[client]->get_connected() == false)
	{
		register_client(client);
		return;
	}
	char temp[BUFFER_SIZE] = {0};
	int total_ret;

	std::string msg = "";
	total_ret = recv(client, temp, BUFFER_SIZE, 0);
	std::cerr << total_ret << std::endl;
	if (total_ret == -1)
	{
		std::cout << "Error: mannagia a cristo" << strerror(errno) << std::endl;
		throw Server::ClientException();
	}
	temp[total_ret] = '\0';
	msg.append(temp);

	std::cerr << total_ret << std::endl;
	// if (total_ret == -1)
	// {
	// 	std::cout << "Error: mannagia a cristo b" << strerror(errno) << std::endl;
	// } perche' ce ne erano 2?
	if (total_ret == 0)
		throw Server::ClientException();
	if (msg.empty())
	{
		std::cerr << "🥍haha, i'm in danger 🚌️🤸️" << std::endl;
		throw Server::ClientException();
		return;
	}
	if (msg == "\n")
		return;

	if (msg.size() >= 2 && msg.substr(msg.size() - 2) == "\r\n")
	{
		msg = msg.substr(0, msg.size() - 2);
	}
	else if (msg.size() >= 1 && msg.substr(msg.size() - 1) == "\n")
	{
		msg = msg.substr(0, msg.size() - 1);
	}

	std::cout << GREEN "Received: [" << msg.substr(0, msg.size()) << "]" << RESET << std::endl; // TODO remove
	if (msg.find("\r\n") != std::string::npos)
	{
		std::vector<std::string> multiple_msg = cosplit(msg, "\r\n");
		for (std::vector<std::string>::iterator it = multiple_msg.begin(); it != multiple_msg.end(); it++)
		{
			*it += "\r\n";
			msg = *it;
			std::cout << RED "Received: {" << msg.substr(0, msg.size() - 2) << "}" RESET << std::endl;
			parse_cmds(client, trimString(msg));
		}
	}
	else
	{
		msg += "\r\n";
		std::cout << RED "Received: {" << msg.substr(0, msg.size() - 2) << "}" RESET << std::endl;
		parse_cmds(client, trimString(msg));
	}

	// msg += "\r\n";
	// std::cout << RED "Received: {" << msg.substr(0, msg.size() - 2) << "}" RESET << std::endl;
	// std::cout << BLUE << " " << msg << RESET << std::endl;
}

void Server::login(int client, std::string msg)
{
	std::cout << RED "Received: [" << msg.substr(0, msg.size() - 2) << "]" << RESET << std::endl; // TODO remove
	if (msg == "\r\n")
	{
		//write_to_client(client, ":irc PASS :You must send a password first");
		return;
	}
	std::vector<std::string> split_msg = split(msg, " ");
	if (split_msg.size() > 0 && split_msg[0] == "CAP")
	{
		cap(client, msg);
		return;
	}
	switch (m_clients[client]->get_reg_steps())
	{
	case 0:
		if (split_msg[0] == "PASS")
		{
			if (split_msg.size() > 1 && split_msg[1].find(":") == 0)
				split_msg[1].erase(0, 1);
			if (split_msg.size() >= 2 && verify_password(split_msg[1], m_hash, m_salt))
				m_clients[client]->set_reg(1);
			else if (split_msg.size() == 1)
			{
				write_to_client(client, ":irc 461 PASS :Not enough parameters"); // ERR_NEEDMOREPARAMS
			}
			else
			{
				write_to_client(client, "Wrong password");
				// std::cerr << BRIGHT_MAGENTA"|" << split_msg[1] <<"|" RESET << std::endl;
			}
		}
		else
		{
			//per quale cavolo di motivo qui non posso scrivere You must send a password first TODO
			write_to_client(client, ":irc 461 PASS :Not enough parameters"); // ERR_NEEDMOREPARAMS
		}
		break;
	case 1:
		if (split_msg[0] == "NICK")
		{
			if (split_msg.size() == 2)
			{
				if (get_client_by_nick(split_msg[1]) != NULL || split_msg[1].find("[bot]") != std::string::npos || split_msg[1] == "coucou")
				{
					write_to_client(client, ":irc 433 " + split_msg[1] + " :Nickname already in use"); // ERR_NICKNAMEINUSE
					m_clients[client]->set_nick_failed(true);
					break;
				}
				if (split_msg[1].find(":@#&") != std::string::npos)
				{
					write_to_client(client, ":irc 432 " + split_msg[1] + " :Erroneous nickname"); // ERR_ERRONEUSNICKNAME
					break;
				}
				m_clients[client]->set_nick(split_msg[1]);
				m_clients[client]->set_reg(2);
			}
			else
				write_to_client(client, ":irc 461 NICK :No nickname given"); // ERR_NONICKNAMEGIVEN
		}
		else
			write_to_client(client, ":irc NICK :You must send a nickname first");
		break;
	case 2:
		if (split_msg[0] == "USER")
		{
			if (split_msg.size() >= 5)
			{

				std::string realname;
				size_t i = 4;
				while (i < split_msg.size() && split_msg[i].find(":") != 0)
					i++;
				if (i == split_msg.size())
				{
					m_clients[client]->send_message("USER :Not enough parameters");
				}
				while (i < split_msg.size())
				{
					if (split_msg[i].find(":") == 0)
						split_msg[i].erase(0, 1);

					realname.append(split_msg[i]);
					if (i != split_msg.size() - 1)
						realname.append(" ");
					i++;
				}
				m_clients[client]->set_user(split_msg[1]);
				m_clients[client]->set_hostname(split_msg[2]);
				m_clients[client]->set_servername(split_msg[3]);
				m_clients[client]->set_realname(realname);
				m_clients[client]->set_reg(3);
				m_clients[client]->set_connected(true);
				m_clients[client]->set_registered(true);

				m_clients[client]->send_message(":irc 001 " + m_clients[client]->get_nick() + " :Welcome to the Jungle Network, " + m_clients[client]->get_nick() + "!" + m_clients[client]->get_user() + "@" + m_clients[client]->get_hostname());
				m_clients[client]->send_message(":irc 002 " + m_clients[client]->get_nick() + " :Your host is irc, running version 4.2");
				m_clients[client]->send_message(":irc 003 " + m_clients[client]->get_nick() + " :This server was created " + m_date);
				m_clients[client]->send_message(":irc 004 " + m_clients[client]->get_nick() + " " + m_clients[client]->get_servername() + " 4.2 uMode:none cMode:+i,+t,+k,+o");
				m_clients[client]->send_message(":irc 375 " + m_clients[client]->get_nick() + " :- Welcome to the Jungle");
				m_clients[client]->send_message(":irc 372 " + m_clients[client]->get_nick() + " :- ");
				m_clients[client]->send_message(":irc 372 " + m_clients[client]->get_nick() + " :- try not to break anything");
				m_clients[client]->send_message(":irc 372 " + m_clients[client]->get_nick() + " :- ");
				m_clients[client]->send_message(":irc 376 " + m_clients[client]->get_nick() + " :- End of MOTD command");
				m_clients[client]->send_message(":Coucou!coucou@Coucou PRIVMSG " + m_clients[client]->get_nick() + " :Coucou is here to help you");
			}
			else
				write_to_client(client, ":irc USER :You must send a username first");
			break;
		}
		else
		{
			write_to_client(client, ":irc USER :You must send a username first");
			break;
		}
	default:
		std::cerr << " how do you get here? " << std::endl;
		close(client);
		exit(1);
		break;
	}
}

void Server::register_client(int client)
{
	char temp[BUFFER_SIZE] = {0};
	int total_ret;
	std::string msg = "";
	std::string temp_msg = "";

	do
    {
        total_ret = recv(client, temp, BUFFER_SIZE - 1, 0);
		if (total_ret == -1)
			continue;
        if (total_ret <= 0)
        {
            // Gestisci la disconnessione o l'errore
            std::cout << "Error: " << strerror(errno) << std::endl;
            // throw Server::ClientException();
        }

        temp[total_ret] = '\0'; // Assicurati che il buffer sia terminato con null
        std::string temp_str(temp);

        // Rimuovi tutti i caratteri Ctrl+D dall'input
        temp_str.erase(std::remove(temp_str.begin(), temp_str.end(), '\004'), temp_str.end());

        temp_msg += temp_str;
    }
    while (temp_msg.find('\n') == std::string::npos);


	// do
	// {
	// 	total_ret = recv(client, temp, BUFFER_SIZE, 0);
	// 	temp[total_ret] = '\0';
	// 	temp_msg.append(temp);
	// 	if (total_ret == 0)
	// 		break;
	// }
	// while (temp_msg.find('\n') == std::string::npos || temp_msg.find('\004') != std::string::npos);




	std::cerr << "size:" << total_ret << std::endl;
	if (total_ret == -1 || total_ret == 0)
	{
		std::cout << "Error: mannagia a cristo" << strerror(errno) << std::endl;
		throw Server::ClientException();
	}
	msg.append(temp_msg);
	if (msg.empty())
	{
		std::cerr << "haha, i'm in danger 🚌️🤸️" << std::endl;
	}

	//hexchat
	if (msg.size() >= 2 && msg.substr(msg.size() - 2) == "\r\n")
	{
		msg = msg.substr(0, msg.size() - 2);
	}
	else if (msg.size() >= 1 && msg.substr(msg.size() - 1) == "\n")
	{
		msg = msg.substr(0, msg.size() - 1);
	}
	std::cout << GREEN "Received: [" << msg.substr(0, msg.size()) << "]" << RESET << std::endl; // TODO remove
	if (msg.find("\r\n") != std::string::npos)
	{
		std::vector<std::string> multiple_msg = cosplit(msg, "\r\n");
		for (std::vector<std::string>::iterator it = multiple_msg.begin(); it != multiple_msg.end(); it++)
		{
			*it += "\r\n";
			if ((*it).find("USER") != std::string::npos)
				{m_clients[client]->set_str_user(*it);
				std::cerr <<YELLOW "userstr:" << m_clients[client]->get_str_user() << RESET << std::endl;}
			login (client, *it);
			if (m_clients[client]->get_nick_failed() == true && !m_clients[client]->get_str_user().empty() && m_clients[client]->get_reg_steps() == 2)
				login (client, m_clients[client]->get_str_user());
		}
	}
	else
	{
		msg += "\r\n";
		login(client, msg);
		if (m_clients[client]->get_nick_failed() == true && !m_clients[client]->get_str_user().empty() && m_clients[client]->get_reg_steps() == 2)
		{
			login (client, m_clients[client]->get_str_user());
		}

	}
}

void Server::get_cmds_help( void )
{
	m_cmds_help["INFO"] = "INFO :Lists all the available commands";
	m_cmds_help["PASS"] = "PASS <password> :Registers a password";
	m_cmds_help["NICK"] = "NICK <nickname> :Used to either change or register a nickname";
	m_cmds_help["USER"] = "USER <username> <hostname> <servername> <realname> :Registers a user";
	m_cmds_help["PRIVMSG"] = "PRIVMSG <target>{,<target>} :<message> :Sends messages to the target(s)"; 
	m_cmds_help["JOIN"] = "JOIN <channel>{,<channel>} [<key>{,<key>}] :Joins a channel(s), the channel names must start with # or &. \n If the channel is password protected, the key must be provided";
	m_cmds_help["PART"] = "PART <channel>{,<channel>} [:<message>] :Leaves a channel(s) with an optional message";
	m_cmds_help["QUIT"] = "QUIT [:<message>] :Disconnects from the server with an optional message";
	m_cmds_help["KICK"] = "KICK <channel> <user> [:<message>] :Kicks a user from a channel with an optional message";
	m_cmds_help["INVITE"] = "INVITE <user> <channel> :Invites a user to a channel, required if the channel is invite-only";
	m_cmds_help["TOPIC"] = "TOPIC <channel> [:<topic>] :Without a topic, it will display the current topic. With a topic, it will set the topic";
	m_cmds_help["NAMES"] = "NAMES [<channel>] :Lists all the users in a channel {with @ if they are operators}, if no channel is provided, it will list all the users and all the users in all the channels";
	m_cmds_help["CAP"] = "CAP <subcommand> :Used to negotiate capabilities with the server";
	m_cmds_help["WHO"] = "WHO <target> : Gives information about the target.";
	m_cmds_help["PING"] = "PING <target> <message> : Pings the target with a message expecting a PONG reply";
	m_cmds_help["USERHOST"] = "USERHOST <nickname>{<nickname>} : Returns the hostname of the nickname(s)";
	m_cmds_help["LIST"] = "LIST [<channel>{,<channel>}] : Lists all the channels on the server, if a channel is provided, it will give information about that channel";
	m_cmds_help["ISON"] = "ISON <nickname>{<nickname>} : Checks if the nickname(s) are online";
}
