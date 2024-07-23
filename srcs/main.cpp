/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mruggier <mruggier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 10:31:32 by lmicheli          #+#    #+#             */
/*   Updated: 2024/07/23 15:35:13 by mruggier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <server.hpp>
#include <sys/ioctl.h>

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

void print_logo() {
    std::string art[] = {
	"┌──┬───────────────────────────┬──┐ ",
	"│  │                           │, │",
	"│[]│                           │[]│",
	"│  │                           │  │",
	"│  │    ██╗██████╗  ██████╗    │  │",
	"│  │    ██║██╔══██╗██╔════╝    │  │",
	"│  │    ██║██████╔╝██║         │  │",
	"│  │    ██║██╔══██╗██║         │  │",
	"│  │    ██║██║  ██║╚██████╗    │  │",
	"│  │    ╚═╝╚═╝  ╚═╝ ╚═════╝    │  │",
	"│  │                           │  │",
	"│  │                           │  │",
	"│  \\                           /  │",
	"│   '─────────────────────────'   │",
	"│       ,─────────────────.────.  │",
	"│       │  ,───,          │    │  │",
	"│       │  │   │          │    │  │",
	"│ ╔═╓ ╖ │  │   │          │    │  │",
	"│ ║ ╠═╣ │  │   │          │    ││││",
	"│ ╚═╙ ╜ │  │   │          │    │◥◤│",
	"\\       │  │___│          │____│  │",
	" ',____/───────────────────_______/",
    };
	//sources:
	//Font Name: ANSI Shadow
	//https://patorjk.com/software/taag/#p=testall&f=Graffiti&t=IRC

    int terminalWidth = getTerminalWidth();

    const int artHeight = 22;
    int artWidth = 35;

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

	print_logo();
	
	Server server(av[1], av[2]);
	try
	{
		server.accept_connection();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
}