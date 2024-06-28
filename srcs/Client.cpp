/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/28 11:41:18 by lmicheli          #+#    #+#             */
/*   Updated: 2024/06/28 11:42:04 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Client.hpp>

Client::Client(int clientSocket, struct sockaddr_in clientAddr)
{
	m_clientSocket = clientSocket;
	m_clientAddr = clientAddr;
	m_registered = false;
	m_connected = false;
	m_nick = "";
	m_user = "";
}