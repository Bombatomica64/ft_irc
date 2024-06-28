/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_utils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/28 11:38:59 by lmicheli          #+#    #+#             */
/*   Updated: 2024/06/28 15:00:57 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <main.hpp>

std::vector<std::string> split(std::string str, std::string token)
{
	std::vector<std::string>result;
	while(str.size())
	{
		int index = str.find(token);
		if(index != std::string::npos)
		{
			result.push_back(str.substr(0,index));
			str = str.substr(index + token.size());
			if(str.size() == 0)result.push_back(str);
		}
		else
		{
			result.push_back(str);
			str = "";
		}
	}
	return result;
}