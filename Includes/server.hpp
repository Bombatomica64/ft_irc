/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mruggier <mruggier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 11:01:39 by lmicheli          #+#    #+#             */
/*   Updated: 2024/09/11 17:44:02 by mruggier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include <main.hpp>
# include <Client.hpp>
# include <Channel.hpp>
# include <cuocuo.hpp>
# include <ctime>

class Client;
class Channel;

class Server
{
	private:
		//password
		std::string					m_salt;
		std::string					m_hash;

		// clients
		std::vector<struct pollfd>	m_client_fds;
		std::map<int, Client*>		m_clients;
		
		//channels
		std::map<std::string, Channel*> m_channels;
		
		//commands
		std::map<std::string, bool (Server::*)(int, std::string)>	m_cmds;
		std::map<std::string, std::string> 							m_cmds_help;
		
		//Net stuff
		struct pollfd		m_server_fd;
		int					m_socket;
		int					m_port;
		struct sockaddr_in	m_addr;
		std::string			m_ip;

		//bot
		Coucou				m_coucou;
		
		//other
		std::string			m_date;
		std::vector<t_file> m_files;

	public:
		/**
		 * @brief Construct a new Server object
		 * 
		 * @warning 🟥 ctrl c con nc manda messaggio error: client failed, ah, e se sei dentro un canale crasha, forse.
		 * @warning 🟨 missing NOTICE, WHOIS, LIST, ISON
		 * 
		 * @note 🟩 info (help) done
		 * @note 🟨 se spammi ping crasha se non ottimizzato
		 * @note 🟩 aprendo 2 hexchat, il secondo dovra' cambiare nick, 
		 * facendo crahare il server. ora semplicemente non contina con USER? perche' semplicemente hexchat l'aveva gia mandato, quindi sono io stronzo che dopo il suo secondo tentativo di NICK devo rimandare USER
		 * @note 🟩 se il terminale e' troppo piccolo, crasha all'avvio. anche se si avvia e poi lo rimpicciolisci, alla chiusura crasha 😂 perche' size_t ha deciso di interpretare -2 come 18446744073709551614
		 * @note 🟨 dopo il login scritte a caso vengono tagliate da received {c} da capire se ci sono problemi con il fatto che modifico i comandi perche' abbiano sempre \r\n alla fine (duplicato nella registrazione e dopo)
		 * @note 🟩 dopo join, hexchat manda mode e who insieme, quindi ora tutti i comandi vengono controllati se sono multipli.
		 */
		Server() {} //useless
		Server(std::string port, std::string psw);
		~Server();

	public:
		void	create_socket(void);
		void	bind_socket(void);
		void	accept_connection( void );
		void	get_cmds( void );
		void	get_cmds_help( void );

	/**
	 * @brief server functions
	 */
	public:
		Channel*	get_channel(std::string name);
		void		add_channel(std::string name);
		void		add_channel(std::string name, std::map<char, int> modes);
		bool		send_msg_to_channel(int client, std::string Channel ,std::string msg);
		void		remove_channel(std::string name);
		std::string	get_ip( void ) const ;
		std::string	get_date( void ) const { return m_date; }
		int			get_port( void ) const ;
		// bool		send_msg_to_all(int client, std::string msg);
	/**
	 * @brief client functions
	 */
	public:
		void	read_from_client(int client);
		void	write_to_client(int client, std::string msg);
		void	register_client(int client);
		void	parse_cmds(int client, std::string cmd);
		void	send_msg_to_set(std::set<std::string> clients, std::string msg);
		Client*	get_client_by_nick(std::string nick);
		bool	is_client_in_channel(std::string nick, std::string channel);
		bool	client_exist(const std::string &nick) const;
		std::string	getNamesMessage(Channel* chan, int client, std::set<std::string>& client_names);
		void	login(int client, std::string msg);
		bool	kick_in_loop(int client, std::string kick_chan, std::string kick_client, std::string kick_msg);

		
		//🟥 🟧 🟨 🟩

		/**
		 * @brief Registers a password.
		 * 
		 * @note PASSㅤ<password>
		 * @note 🟩 crash if not enough parameters (PASS e basta) basic_string::_M_construct null not valid, ho semplicemente invertito i controlli di un if
		 * 
		 * @warning
		 */
		bool pass(int client, std::string cmd);
		
		/**
		 * @brief Registers a nickname.
		 * 
		 * @note NICKㅤ<nickname>
		 * @note 🟩 non crasha piu', ho aggiunto modify_client_nick
		 * @note 🟨 nick da vedere se funziona su hexchat per il cambio di nome. GARBAGE: mruggier_ changed his nickname to mark. messo il messaggio corretto anche se fuori da qualsiasi canale.
		 * 
		 */
		bool nick(int client, std::string cmd);
		
		/**
		 * @brief Registers a user.
		 * 
		 * @note USERㅤ<username>ㅤ<hostname>ㅤ<servername>ㅤ<realname>
		 * @note 🟨 se dopo NICK non scrivo qualcosa che non sia USER crasha, aggiunto qualcosa alla fine di USER
		 * 
		 * @warning
		 */
		bool user(int client, std::string cmd);
		
		/**
		 * @brief Sends a private message to a client.
		 * 
		 * @note PRIVMSGㅤ<receiver>{,<receiver>}ㅤ:<message>
		 * @note users or channels
		 * 
		 * @warning
		 */
		bool privmsg(int client, std::string cmd);
		
		/**
		 * @brief Sends a message to a channel.
		 * 
		 * @note JOINㅤ#<channel>{,#<channel>}ㅤ[<key>{,<key>}]
		 * @note 🟨 se dopo essere entrato nel canale scrivo NAMES #chan sembra che entri veramente nel canale. hexchat vuole names anche quando crei il canale. vorra' anche modes e topic?
		 * @note 🟧 se crei un canale con valgrind crasha, penso per modify mode, insieme al problema sotto, ho palesmente rotto qualcosa
		 * 
		 * @warning
		 */
		bool join(int client, std::string cmd);
		
		/**
		 * @brief Leaves a channel.
		 * 
		 * @note PARTㅤ#<channel>{,#<channel>} [:<message>]
		 * @note 🟨 se esce l'operatore non viene dato a nessun altro (va rimosso dalla lista degli operatori). ok, ora manda anche il messaggio e il messaggio di part in remove_client. il messaggio di part va mandato li?
		 * @note 🟩 aggiunti i messaggi per far sapere al client e al server che hai fatto part in remove_client
		 * @warning 🟨 non funziona perche' hexchat manda part #chan :motivo, anche se il motivo non lo metti, scrive :leaving (se hexchat non vanno messi i :, li mette lui). ora quando esci manda anche il motivo a tutti
		 * 
		 * @warning
		 */
		bool part(int client, std::string cmd);
		
		/**
		 * @brief Disconnects a client from the server.
		 * 
		 * @note QUITㅤ[:<message>]
		 * @note 🟧 hexchat entrato in un canale, poi premendo x per chiudere tutto, crasha se sono solo nel canale. insomma in remove_client ho aggiunto un else if, cosi' magari non controlla se ci sono operatori in un canale appena cancellato. poi in quit ho messo it come tmp, poi it++ e poi cancellato tmp, senno cancellavamo l'iteratore e non potevamo andare avanti
		 * @note 🟩 ora manda il messaggio di quit (spostato da remove client), default: Leaving
		 * 
		 * @warning
		 */
		bool quit(int client, std::string cmd);
		
		/**
		 * @brief Kicks a client from a channel.
		 * 
		 * @note KICKㅤ#<channel>ㅤ<user>ㅤ[:<message>]
		 * @warning 🟩 non manda ancora il messaggio giusto a remove client e crasha il server 😁. ops per ora ho commentato i messaggi di kick per usare quelli di part. spostato i messaggi di part fuori da remove client. riscritti i messaggi di kick
		 * 
		 * @warning 🟥 #<channel>{,#<channel>} <user>{,<user>} [:<comment>]
		 * @warning 🟥 non puoi kickarti da solo, secondo Gu. ma secondo me e lore si 
		 */
		bool kick(int client, std::string cmd);
		
		/**
		 * @brief Invites a client to a channel.
		 * 
		 * @note INVITEㅤ<nickname>ㅤ#<channel>
		 * 
		 */
		bool invite(int client, std::string cmd);
		
		/**
		 * @brief Changes the topic of a channel.
		 * 
		 * @note TOPICㅤ#<channel>ㅤ[:<topic>]
		 * 
		 */
		bool topic(int client, std::string cmd);
		
		/**
		 * @brief Changes the mode of a channel or user.
		 * 
		 * @note MODEㅤ#<channel>ㅤ{[+|-]|o|i|t|k|l} [<limit>] [<user>]
		 * @note o: operator, i: invite only, t: topic, k: key, l: limit
		 * @note 🟨 -o ora funziona
		 * @warning 🟨 puoi rimuovere tutti da operatore, incluso te stesso (il canale rimane senza operatore). se poi uno esce l'altro diventa operatore (anche se quello uscito non lo era). aggiunto il controllo, con la frase a casp: You're the only operator
		 * @warning 🟧 vedi te se cambiare il controllo dei numeri negativi e 0 con il controllo se < degli utenti nel canale, allora errore. per me e' piu' corretto cosi'
		 * @warning 🟨 i messaggi di mode mandano a tutto il canale qualcosa? solo +o lo fa (giustamente). +k +l non mandano la modifica effettuata a titti, per qualche motivo aggiungo anche gli altri
		 * 
		 */
		bool mode(int client, std::string cmd);
		
		/**
		 * @brief Lists the clients in a channel.
		 * 
		 * @note NAMESㅤ[#<channel>{,#<channel>}]
		 * @note 🟩 siamo di nuovo tutti amministratori. ho messo get->ops() fuori dal ciclo for.
		 * @note 🟩 completamente sballato su hexchat (ora 353 ha un brutto = in mezzo)
		 * 
		 * @warning 🟥 non puoi fare NAMES su hexchat se al di fuori di un canale, quindi non gli arriva il fatto che sia un canale pubblico
		 */
		bool names(int client, std::string cmd);
		
		/**
		 * @brief Registers a nickname.
		 * 
		 * @note konversation
		 * 
		 * @warning 
		 */
		bool cap(int client, std::string cmd);
		
		/**
		 * @brief Retrieves information about a client.
		 * 
		 * @note WHOㅤ<client>ㅤ(parziale)
		 * 
		 * @warning 🟥 * #chan :No such nick/channel appena crei un canale. if (get_client_by_nick(split_msg[1]) == NULL) dovrebbe essere m_clients[client]->get_nick()?
		 */
		bool who(int client, std::string cmd);
		
		/**
		 * @brief Pings a client.
		 * 
		 * @note PINGㅤ<client>ㅤ(parziale)
		 * @note 🟩 ping non funziona con hexchat che dopo molti secondi si chiude
		 * 
		 * @warning
		 */
		bool ping(int client, std::string cmd);
		
		/**
		 * @brief Retrieves the user host information.
		 * 
		 * @note USERHOSTㅤ<nickname> (parziale)
		 * 
		 * @warning 
		 */
		bool userhost(int client, std::string cmd);

		/**
		 * @brief Lists the channel on the server.
		 * 
		 * @note LISTㅤ[#<channel>{,#<channel>}]
		 */
		bool list(int client, std::string cmd);

		bool info(int client, std::string cmd);
		bool ison(int client, std::string cmd);
	public:
		class clientSocketException : public std::exception
		{
			virtual const char *what() const throw()
			{
				return "Error: client socket non-blocking failed";
			}
		};
		class PollException : public std::exception
		{
			virtual const char *what() const throw()
			{
				return "Error: poll failed";
			}
		};
		class ClientException : public std::exception
		{
			virtual const char *what() const throw()
			{
				return "Error: client failed";
			}
		};
		class BindException : public std::exception
		{
			virtual const char *what() const throw()
			{
				return "Error: bind failed";
			}
		};

		class ListenException : public std::exception
		{
			virtual const char *what() const throw()
			{
				return "Error: listen failed";
			}
		};

		class FcntlException : public std::exception
		{
			virtual const char *what() const throw()
			{
				return "Error: fcntl failed";
			}
		};
};
