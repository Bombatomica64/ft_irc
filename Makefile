# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/06/25 10:29:36 by lmicheli          #+#    #+#              #
#    Updated: 2024/10/21 00:00:00 by lmicheli         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv
BOT = bot

# Compiler and flags
CXX := clang++
CXXFLAGS := -Wall -Wextra -Werror -pedantic -std=c++98 -O3 -ffast-math
CXXFLAGS += -Wshadow -Wfloat-equal -Wundef -Wredundant-decls
CXXFLAGS += -Wold-style-cast -Wnon-virtual-dtor -Woverloaded-virtual
CXXFLAGS += -Wformat=2 -Wformat-signedness
CXXFLAGS += -Walloca -Wcast-align -Wcast-qual
CXXFLAGS += -Wctor-dtor-privacy -Wdouble-promotion
CXXFLAGS += -Wenum-conversion -Wextra-semi
CXXFLAGS += -Wmismatched-tags -Wmissing-braces -Wmultichar
CXXFLAGS += -Wpointer-arith -Wuninitialized -Wvla
CXXFLAGS += -Wconversion -Wsign-conversion


CPPFLAGS := -I Includes

OBJDIR := .obj

SRCS := srcs/main.cpp \
		srcs/parse.cpp \
		srcs/Client.cpp \
		srcs/Channel.cpp \
		srcs/server_login.cpp \
		srcs/server_commands.cpp
		#srcs/Hashing.cpp

UTILS := utils/string_utils.cpp

SRC := $(SRCS) $(UTILS)
OBJS := $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRC))

BOTSRCS := bonus/coucou.cpp \
		   bonus/bot.cpp \
		   srcs/Client.cpp \
		   utils/string_utils.cpp \
		   srcs/parse.cpp

BOTOBJS := $(patsubst %.cpp,$(OBJDIR)/%.o,$(BOTSRCS))

.PHONY: all clean fclean re bot rebot

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(OBJS) -o $@

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

bot: $(BOTOBJS)
	$(CXX) $(BOTOBJS) -o $(BOT)

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME) $(BOT)

re: fclean all

rebot: 
	rm -f $(BOTOBJS) $(BOT)
	$(MAKE) bot