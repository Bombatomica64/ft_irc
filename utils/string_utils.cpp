/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_utils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/28 11:38:59 by lmicheli          #+#    #+#             */
/*   Updated: 2024/07/26 16:53:07 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <main.hpp>

std::vector<std::string> split(std::string str, std::string token)
{
	std::vector<std::string> result;
	str = trimString(str);
	while (str.size())
	{
		std::size_t index = str.find(token);
		if (index != std::string::npos)
		{
			result.push_back(str.substr(0, index));
			str = str.substr(index + token.size());
		}
		else
		{
			result.push_back(str);
			str = "";
		}
	}
	return result;
}

std::string trimString(std::string nick)
{
	std::string result = nick;
    result.erase(std::remove(result.begin(), result.end(), '\r'), result.end());
    result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
    //result.erase(std::remove(result.begin(), result.end(), ' '), result.end());
    return result;
}


std::string	get_time( void )
{
	time_t	now = time(0);
	char	buf[80];
	tm		*ltm = localtime(&now);
	std::string result;
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ltm);
	result = buf;
	return result;
}