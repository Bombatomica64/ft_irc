/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 10:33:03 by lmicheli          #+#    #+#             */
/*   Updated: 2024/09/19 13:42:18 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAIN_HPP
#define MAIN_HPP

#include <iostream>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <cstdlib>
#include <poll.h>
#include <vector>
#include <netinet/in.h>
#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <colours.hpp>
#include <errno.h>
#include <cstring>
#include <csignal>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <typeinfo>
#include <netdb.h>

#define BUFFER_SIZE 2048

template <typename T>
std::string NumberToString(T Number)
{
	std::ostringstream ss;
	ss << Number;
	return ss.str();
}

typedef struct s_file
{
	int sender;
	int receiver;
	std::string filename;
	std::string ip;
	int port;
	double filesize;
} t_file;

void check_input(std::string host, std::string port);

std::vector<std::string> split(std::string str, std::string token);
std::vector<std::string> cosplit(const std::string &s, const std::string &delimiter);
std::string trimString(std::string nick);
std::string generate_salt(size_t length);
std::string hash_password(const std::string &password, const std::string &salt);
bool verify_password(const std::string &entered_password, const std::string &stored_hash, const std::string &salt);
std::string get_time( int flag );
void printLogo(std::string ip, int port);
void printQrCode( void );
std::vector<std::string> scale_down(std::vector<std::string> art, int terminalWidth, int terminalHeight);
int getTerminalWidth();
int getTerminalHeight();
std::string decode_ip(std::string ip);


inline std::ostream &operator<<(std::ostream &o, std::vector<std::string> const &v)
{
	for (std::vector<std::string>::const_iterator it = v.begin(); it != v.end(); ++it)
		o << *it << std::endl;
	return o;
}

inline bool operator==(const struct pollfd& pfd, int fd)
{
	return pfd.fd == fd;
}

#endif
