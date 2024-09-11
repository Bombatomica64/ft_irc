# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/06/25 10:29:36 by lmicheli          #+#    #+#              #
#    Updated: 2024/09/11 15:35:42 by lmicheli         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

SRCS =	srcs/main.cpp \
		srcs/parse.cpp \
		srcs/Client.cpp  \
		srcs/Channel.cpp  \
		srcs/Hashing.cpp   \
		bonus/coucou.cpp    \
		srcs/server_login.cpp \
		srcs/server_commands.cpp \


UTILS = utils/string_utils.cpp

SRC = $(SRCS) $(UTILS)

INC = -I Includes

CC = clang++ -Wall -Wextra -Werror \
			-pedantic -Wshadow -Wfloat-equal \
			 -Wundef -Wredundant-decls -Wold-style-cast -Wnon-virtual-dtor \
			-Woverloaded-virtual -Wformat=2 \
			-std=c++98 -g -Ofast

CRYPTO = -lssl -lcrypto

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME):
	$(CC) $(INC) $(SRC) -o $(NAME) $(CRYPTO)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)
	rm -f test

re: fclean all

.PHONY: all clean fclean re