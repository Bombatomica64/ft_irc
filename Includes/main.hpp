/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 10:33:03 by lmicheli          #+#    #+#             */
/*   Updated: 2024/07/04 18:37:00 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAIN_HPP
# define MAIN_HPP

# include <iostream>
# include <arpa/inet.h>
# include <fcntl.h>
# include <unistd.h>
# include <sys/socket.h>
# include <cstdlib>
# include <poll.h>
# include <vector>
# include <netinet/in.h> 
# include <map>
# include <set>
# include <string>
# include <algorithm>
# include <colours.h>
# include <errno.h>
# include <cstring>
# include <csignal>

# define BUFFER_SIZE 2048

void	check_input(char *host, char *port);

std::vector<std::string> split(std::string str, std::string token);

inline std::ostream &operator<<(std::ostream &o, std::vector<std::string> const &v) {
	for (std::vector<std::string>::const_iterator it = v.begin(); it != v.end(); ++it)
		o << *it << std::endl;
	return o;
}


#endif