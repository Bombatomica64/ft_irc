/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_utils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/28 11:38:59 by lmicheli          #+#    #+#             */
/*   Updated: 2024/09/16 12:07:45 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <main.hpp>

std::vector<std::string> cosplit(const std::string &instr, const std::string &delimiter)
{
    std::vector<std::string> vec;
    size_t start = 0;
    size_t it = instr.find(delimiter);
    while (it != std::string::npos)
	{
        std::string token = instr.substr(start, it - start);
        if (!token.empty())
		{
            vec.push_back(token);
        }
        start = it + delimiter.length();
        it = instr.find(delimiter, start);
    }
    std::string token = instr.substr(start);
    if (!token.empty())
	{
        vec.push_back(token);
    }
    return vec;
}

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
	return result;
}

std::string get_time(int flag)
{
	time_t now = time(0);
	char buf[80];
	tm *ltm = localtime(&now);
	std::string result;
	if (flag == 1)
		strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ltm);
	else
		strftime(buf, sizeof(buf), "%a, %d %b %Y at %H:%M:%S", ltm);
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

int getTerminalHeight() {
	struct winsize ws;
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) {
		// If ioctl fails, return a default height
		return 24;
	}
	return ws.ws_row;
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

    size_t terminalWidth = static_cast<size_t>(getTerminalWidth());
	std::string port_str = NumberToString(port);
	if (terminalWidth < 39)
	{
		std::cout << "IP: " << ip << std::endl;
		std::cout << "Port: " << port << std::endl;
	 	return;
	}

    const size_t artHeight = 22;
    size_t artWidth = 39;

	art[11].replace(13, ip.size(), ip);
	art[11].replace(29, port_str.size(), port_str);
	(void)ip;
	for (size_t i = 0; i < artHeight; ++i)
	{
		int padding = terminalWidth - artWidth;
		if (padding > 0)
			padding = padding / 2;
		else
			padding = 0;
		std::string spaces(padding, ' ');
		std::cout << CYAN << spaces << art[i] << RESET << std::endl;
    }
}

std::vector<std::string> scale_down(std::vector<std::string> art, int terminalWidth, int terminalHeight)
{
    size_t artWidth = art[0].length();
    size_t artHeight = art.size();
    std::vector<std::string> scaledLines;

    size_t scaleX = std::max((terminalWidth / artWidth), static_cast<size_t>(1));
    size_t scaleY = std::max((terminalHeight / artHeight), static_cast<size_t>(1));

    for (size_t i = 0; i < artHeight; i += scaleY)
    {
        std::string scaledRow;
        for (size_t j = 0; j < artWidth; j += scaleX)
        {
            scaledRow += art[i][j];
        }
        scaledLines.push_back(scaledRow);
    }

    return scaledLines;
}

void printQrCode()
{
	std::string art[] = {
	"█████████████████████████████████",
	"██ ▄▄▄▄▄ ██▄▄ ▀█▄█▀  █▀█ ▄▄▄▄▄ ██",
	"██ █   █ █▀▄  █▀▄▀▄▀▄▀ █ █   █ ██",
	"██ █▄▄▄█ █▄▀ █▄▀▄▄▄█ ▄ █ █▄▄▄█ ██",
	"██▄▄▄▄▄▄▄█▄▀▄█ █▄█ █▄▀ █▄▄▄▄▄▄▄██",
	"██▄ █▀▀ ▄▄▀▄ █▄▄  ▄▄█ █▀█▀ ▄▀▀███",
	"███▀ █▀█▄▄▄▄▀ ▄▄██▄█▀▀█▀██▄▀█████",
	"███▀▄█▀▀▄█▀  █▀ ▄▄█▀▀▄ █  ▀██▀ ██",
	"██▄██▀█▀▄ ▄▄▄█▄   █▄█▀▄▄█▀ ▄ █▀██",
	"██ ▀▄ ▄▄▄▄ ▄███▄█▄█ ▀ █▄▀ ▀▄ ▀▀██",
	"██ ▄ ▀▀█▄▄▀▀ ▀▄▄ ▄ ▄█▄▀█▀█▄▄██▄██",
	"██▄█▄▄█▄▄▄▀▄█▄▀▀█   ▀▄ ▄▄▄ ▀▄▄▄██",
	"██ ▄▄▄▄▄ █ ▄▄█▀ ▄ ▀█▄  █▄█ ▄▄▀▀██",
	"██ █   █ ██▄▀▄▄▄█ ██▀   ▄▄▄ ▀█ ██",
	"██ █▄▄▄█ █▀▄▀ ▄██▄ ▄▄▄▄▄  ▄  ▄▀██",
	"██▄▄▄▄▄▄▄█▄▄▄▄███▄█▄█▄██▄▄▄█▄████",
	"█████████████████████████████████",
	};

	size_t terminalWidth = static_cast<size_t>(getTerminalWidth());
	if (terminalWidth < 33)
	{
		std::cout << "https://github.com/Bombatomica64/ft_irc" << std::endl;
		return;
	}

    const size_t artHeight = 17;
    size_t artWidth = 33;

    for (size_t i = 0; i < artHeight; ++i)
	{
		int padding = terminalWidth - artWidth;
		if (padding > 0)
			padding = padding / 2;
		else
			padding = 0;
        std::string spaces(padding, ' ');
        std::cout << spaces << art[i] << std::endl;
    }
}

std::string decode_ip(std::string ip)
{
	std::string result;
	int ip_int = std::strtol(ip.c_str(), NULL, 10);
	struct in_addr addr;
	addr.s_addr = htonl(ip_int);
	result = inet_ntoa(addr);
	return result;
}
