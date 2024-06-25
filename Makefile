# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/06/25 10:29:36 by lmicheli          #+#    #+#              #
#    Updated: 2024/06/25 16:35:45 by lmicheli         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

SRCS =	srcs/main.cpp \
		srcs/parse.cpp \
		srcs/server.cpp

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