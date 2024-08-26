/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_utils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mruggier <mruggier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/28 11:38:59 by lmicheli          #+#    #+#             */
/*   Updated: 2024/08/26 15:40:21 by mruggier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <main.hpp>

std::vector<std::string> cosplit(const std::string &s, const std::string &delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = s.find(delimiter);
    while (end != std::string::npos) {
        std::string token = s.substr(start, end - start);
        if (!token.empty()) {
            tokens.push_back(token);
        }
        start = end + delimiter.length();
        end = s.find(delimiter, start);
    }
    std::string token = s.substr(start);
    if (!token.empty()) {
        tokens.push_back(token);
    }
    return tokens;
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
	// result.erase(std::remove(result.begin(), result.end(), ' '), result.end());
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

    const size_t artHeight = 22;
    size_t artWidth = 39;

	std::string port_str = NumberToString(port);
	art[11].replace(13, ip.size(), ip);
	art[11].replace(29, port_str.size(), port_str);
    for (size_t i = 0; i < artHeight; ++i)
	{
        size_t padding = (terminalWidth - artWidth) / 2;
        if (padding < 0)
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
	"███▀ █▀█▄▄▄▄▀ ▄▄▀ ▀█▀▀█▀██▄▀█████",
	"███▀▄█▀▀▄█▀  █▀ ▄  ▀▀▄ █  ▀██▀ ██",
	"██▄██▀█▀▄ ▄▄▄█▀   ▄▄█▀▄▄█▀ ▄ █▀██",
	"██ ▀▄ ▄▄▄▄ ▄██▄█▄▀█ ▀ █▄▀ ▀▄ ▀▀██",
	"██ ▄ ▀▀█▄▄▀▀ ▀▄▄ ▄ ▄█▄▀█▀█▄▄██▄██",
	"██▄█▄▄█▄▄▄▀▄█▄▀▀█   ▀▄ ▄▄▄ ▀▄▄▄██",
	"██ ▄▄▄▄▄ █ ▄▄█▀ ▄ ▀█▄  █▄█ ▄▄▀▀██",
	"██ █   █ ██▄▀▄▄▄█ ██▀   ▄▄▄ ▀█ ██",
	"██ █▄▄▄█ █▀▄▀ ▄██▄ ▄▄▄▄▄  ▄  ▄▀██",
	"██▄▄▄▄▄▄▄█▄▄▄▄███▄█▄█▄██▄▄▄█▄████",
	"█████████████████████████████████",
	};

	size_t terminalWidth = static_cast<size_t>(getTerminalWidth());

    const size_t artHeight = 17;
    size_t artWidth = 33;

    for (size_t i = 0; i < artHeight; ++i)
	{
        size_t padding = (terminalWidth - artWidth) / 2;
        if (padding < 0)
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
