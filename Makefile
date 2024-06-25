# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/06/25 10:29:36 by lmicheli          #+#    #+#              #
#    Updated: 2024/06/25 12:00:04 by lmicheli         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

SRCS =	main.cpp \
		parse.cpp \
		server.cpp

INC = -I Includes

CC = clang++ -std=c++98 -Wall -Wextra -Werror -g

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME):
	$(CC) $(INC) $(SRCS) -o $(NAME)

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