/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 10:33:03 by lmicheli          #+#    #+#             */
/*   Updated: 2024/06/25 16:32:00 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAIN_HPP
# define MAIN_HPP

# include <iostream>
# include <server.hpp>
# include <arpa/inet.h>
# include <fcntl.h>
# include <unistd.h>
# include <sys/socket.h>
# include <cstdlib>

void	check_input(char *host, char *port);

#endif