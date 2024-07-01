/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/28 11:41:18 by lmicheli          #+#    #+#             */
/*   Updated: 2024/07/01 11:15:09 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Client.hpp>

Client::Client(int clientSocket, struct sockaddr_in clientAddr)
{
	m_clientSocket = clientSocket;
	m_clientAddr = clientAddr;
	m_registered = false;
	m_connected = false;
	m_reg_steps = 0;
}

Client::~Client()
{
	close(m_clientSocket);
}

std::ostream &operator<<(std::ostream &out, Client const &src)
{
	out << "Client socket: " << src.get_clientSocket() << std::endl;
	out << "Client nick: " << src.get_nick() << std::endl;
	out << "Client user: " << src.get_user() << std::endl;
	out << "Client registered: " << src.get_registered() << std::endl;
	out << "Client connected: " << src.get_connected() << std::endl;
	out << "Client registration steps: " << src.get_reg_steps() << std::endl;
	return out;
}