# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/06/25 10:29:36 by lmicheli          #+#    #+#              #
#    Updated: 2024/07/17 13:06:32 by lmicheli         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

SRCS =	srcs/main.cpp \
		srcs/parse.cpp \
		srcs/server.cpp \
		srcs/Client.cpp  \
		srcs/Channel.cpp \
		bonus/coucou.cpp \

UTILS = utils/string_utils.cpp

SRC = $(SRCS) $(UTILS)

INC = -I Includes

CC = clang++ -Wall -Wextra -Werror -std=c++98 -g

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME):
	$(CC) $(INC) $(SRC) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

play: re
	./$(NAME) 8000 123

val : re
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
	./$(NAME) 8000 123

.PHONY: all clean fclean re