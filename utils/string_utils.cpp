/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_utils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/28 11:38:59 by lmicheli          #+#    #+#             */
/*   Updated: 2024/07/30 13:18:24 by lmicheli         ###   ########.fr       */
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
	// result.erase(std::remove(result.begin(), result.end(), ' '), result.end());
	return result;
}

std::string get_time(void)
{
	time_t now = time(0);
	char buf[80];
	tm *ltm = localtime(&now);
	std::string result;
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ltm);
	result = buf;
	return result;
}

std::vector<std::string> scale_down(std::vector<std::string> art, int terminalWidth, int terminalHeight)
{
	int artWidth = art[0].length();
	int artHeight = art.size();
	std::vector<std::string> scaledLines;

	double scaleX = static_cast<double>(terminalWidth) / artWidth;
	double scaleY = static_cast<double>(terminalHeight) / artHeight;

	std::vector<std::string> scaledLines;
	for (int i = 0; i < artHeight; i += scaleY)
	{
		std::string scaledRow;
		for (int j = 0; j < artWidth; j += scaleX)
		{
			// Simple downscaling: take the first character in the block
			scaledRow += art[i][j];
		}
		scaledLines.push_back(scaledRow);
	}

	return scaledLines;
}
