# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/06/25 10:29:36 by lmicheli          #+#    #+#              #
#    Updated: 2024/09/19 13:44:52 by lmicheli         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv
BOT = bot

SRCS =	srcs/main.cpp \
		srcs/parse.cpp \
		srcs/Client.cpp  \
		srcs/Channel.cpp  \
		srcs/server_login.cpp \
		srcs/server_commands.cpp \
		#srcs/Hashing.cpp


UTILS = utils/string_utils.cpp

SRC = $(SRCS) $(UTILS)

BOTSRCS =	bonus/coucou.cpp \
		bonus/bot.cpp \
		srcs/Client.cpp  \
		utils/string_utils.cpp \
		srcs/parse.cpp

INC = -I Includes

CC = clang++ -Wall -Wextra -Werror \
			-pedantic -Wshadow -Wfloat-equal \
			 -Wundef -Wredundant-decls -Wold-style-cast -Wnon-virtual-dtor \
			-Woverloaded-virtual -Wformat=2 \
			-std=c++98 -g -Ofast

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME):
	$(CC) $(INC) $(SRC) -o $(NAME)

bot:
	$(CC) $(INC) $(BOTSRCS) -o $(BOT) 

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

rebot:
	rm -f $(BOT)
	$(CC) $(INC) $(BOTSRCS) -o $(BOT)

.PHONY: all clean fclean re