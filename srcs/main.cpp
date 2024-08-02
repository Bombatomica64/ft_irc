/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 10:31:32 by lmicheli          #+#    #+#             */
/*   Updated: 2024/08/02 22:14:11 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <server.hpp>

void	handle_signal(int sig)
{
	(void)sig;
	throw std::runtime_error("\nClosing server...");
}

int getTerminalWidth() {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) {
        // If ioctl fails, return a default width
        return 80;
    }
    return ws.ws_col;
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

	int terminalWidth = getTerminalWidth();

    const int artHeight = 17;
    int artWidth = 33;

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
}

int main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cerr << "Usage: " << av[0] << " <port> <password>" << std::endl;
		return 1;
	}
	//CTRL + C
	signal(SIGINT,	handle_signal);
	srand(time(NULL));

	
	try {
		Server server(av[1], av[2]);
		server.accept_connection();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		printQrCode();
	}
	
}
