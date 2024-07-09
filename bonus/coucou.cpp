/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coucou.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/09 18:25:55 by lmicheli          #+#    #+#             */
/*   Updated: 2024/07/09 18:35:38 by lmicheli         ###   ########.fr       */
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
}

Coucou::~Coucou()
{
	m_angry_words.clear();
	m_happy_words.clear();
}