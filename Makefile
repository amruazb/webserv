# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hsalah <hsalah@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/02/27 11:57:01 by shmuhamm          #+#    #+#              #
#    Updated: 2025/03/20 09:17:46 by hsalah           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	=	webserv

SRCS_FOLDER = ./src

INCLUDE_FOLDER = ./include ./lib/string

FILES_NAMES = main.cpp ConfigParser.cpp 

FILES = $(addprefix $(SRCS_FOLDER)/,$(FILES_NAMES))

CXX		=	c++

CXXFLAGS=	-Wall -Wextra -Werror -std=c++98

OBJS	=	$(FILES:.cpp=.o)

all: 		$(NAME)

.cpp.o:
			$(CXX) $(CXXFLAGS) -I$(INCLUDE_FOLDER) -c $< -o $@

$(NAME):	$(OBJS)
			$(CXX) $(OBJS) $(CXXFLAGS) -I$(INCLUDE_FOLDER) -o $(NAME)

clean:
			/bin/rm -f $(OBJS)

fclean:		clean
			/bin/rm -f $(NAME)

re:			fclean all

.PHONY:		all clean fclean re