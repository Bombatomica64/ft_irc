/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_utils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mruggier <mruggier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/28 11:38:59 by lmicheli          #+#    #+#             */
/*   Updated: 2024/07/30 17:19:43 by mruggier         ###   ########.fr       */
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

int getTerminalWidth() {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) {
        // If ioctl fails, return a default width
        return 80;
    }
    return ws.ws_col;
}

void printLogo(std::string ip, int port) {
    std::string art[] = {
	"┌──┬───────────────────────────────┬──┐ ",
	"│  │                               │  │",
	"│▇▇│                               │▇▇│",
	"│  │      ██╗██████╗  ██████╗      │  │",
	"│  │      ██║██╔══██╗██╔════╝      │  │",
	"│  │      ██║██████╔╝██║           │  │",
	"│  │      ██║██╔══██╗██║           │  │",
	"│  │      ██║██║  ██║╚██████╗      │  │",
	"│  │      ╚═╝╚═╝  ╚═╝ ╚═════╝      │  │",
	"│  │                               │  │",
	"│  │     IP:             Port:     │  │",
	"│  │                               │  │",
	"│  │                               │  │",
	"│  ╰───────────────────────────────╯  │",
	"│       ╭─────────────────────┬────╮  │",
	"│       │  ┌───┐              │    │  │",
	"│       │  │   │              │    │  │",
	"│ ╔═╓ ╖ │  │   │              │    │  │",
	"│ ║ ╠═╣ │  │   │              │    ││││",
	"│ ╚═╙ ╜ │  │   │              │    │◥◤│",
	"│       │  └───┘              │    │  │",
	" ╲_____╱──────────────────────┴────│⎯⎯⎭",
    };
	//sources:
	//Font Name: ANSI Shadow
	//https://patorjk.com/software/taag/#p=testall&f=Graffiti&t=IRC

    int terminalWidth = getTerminalWidth();

    const int artHeight = 22;
    int artWidth = 35;

	std::string port_str = NumberToString(port);
	art[11].replace(13, ip.size(), ip);
	art[11].replace(29, port_str.size(), port_str);
    for (int i = 0; i < artHeight; ++i)
	{
        int padding = (terminalWidth - artWidth) / 2;
        if (padding < 0)
			padding = 0;
        std::string spaces(padding, ' ');
        std::cout <<CYAN<< spaces << art[i] <<RESET<< std::endl;
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
			scaledRow += art[i][j];
		}
		scaledLines.push_back(scaledRow);
	}

	return scaledLines;
}
