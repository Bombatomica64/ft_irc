# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/06/25 10:29:36 by lmicheli          #+#    #+#              #
#    Updated: 2024/08/08 10:37:48 by lmicheli         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

SRCS =	srcs/main.cpp \
		srcs/parse.cpp \
		srcs/server.cpp \
		srcs/Client.cpp  \
		srcs/Channel.cpp  \
		srcs/Hashing.cpp   \
		bonus/coucou.cpp    \


UTILS = utils/string_utils.cpp

SRC = $(SRCS) $(UTILS)

INC = -I Includes

#CC = clang++ -Wall -Wextra -Werror -std=c++98 -g -Ofast
CC = clang++ -Wall -Wextra -Werror -pedantic -Wshadow -Wfloat-equal -Wundef -Wredundant-decls -Wold-style-cast -Wnon-virtual-dtor -Woverloaded-virtual -Wformat=2 -std=c++98 -g -Ofast

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

play: re
	./$(NAME) 8000 2

val : re
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
	./$(NAME) 8000 2

#TESTS

GTEST_DIR = $(HOME)/gtest
GTEST_INC = -I $(GTEST_DIR)/include
GMOCK_INC = -I $(GTEST_DIR)/include
GTEST_LIB = $(GTEST_DIR)/lib/libgtest.a
GMOCK_LIB = $(GTEST_DIR)/lib/libgmock.a
GTEST_MAIN_LIB = $(GTEST_DIR)/lib/libgtest_main.a
GMOCK_MAIN_LIB = $(GTEST_DIR)/lib/libgmock_main.a
GTEST = $(GTEST_LIB) $(GTEST_MAIN_LIB)
GMOCK = $(GMOCK_LIB) $(GMOCK_MAIN_LIB)

TESTS = tests/gtest.cpp \
		srcs/parse.cpp   \
		srcs/server.cpp   \
		srcs/Client.cpp    \
		srcs/Channel.cpp    \
		srcs/Hashing.cpp     \
		bonus/coucou.cpp      \
		utils/string_utils.cpp \

CXX = clang++ --std=c++17 -g

test: $(TESTS) $(GTEST) $(GMOCK)
	$(CXX) $(INC) $(GTEST_INC) $(GMOCK_INC) $(TESTS) $(GTEST) $(GMOCK) -o test $(CRYPTO) -lpthread
	./test

.PHONY: all clean fclean re