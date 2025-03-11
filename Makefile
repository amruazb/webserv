# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: shmuhamm <shmuhamm@student.42abudhabi.a    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/02/27 11:57:01 by shmuhamm          #+#    #+#              #
#    Updated: 2025/03/06 13:05:53 by shmuhamm         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	=	webserv

SRCS_FOLDER = ./src

INCLUDE_FOLDER = -I./include -I./lib/string

FILES_NAMES = main.cpp Server.cpp ServerManager.cpp Request.cpp ConfigParser.cpp

FILES = lib/string/ft_string.cpp $(addprefix $(SRCS_FOLDER)/,$(FILES_NAMES))

CXX		=	c++

CXXFLAGS=	-Wall -Wextra -Werror -std=c++98

OBJS	=	$(FILES:.cpp=.o)

all: 		$(NAME)

.cpp.o:
			$(CXX) $(CXXFLAGS) $(INCLUDE_FOLDER) -c $< -o $@

$(NAME):	$(OBJS)
			$(CXX) $(OBJS) $(CXXFLAGS) $(INCLUDE_FOLDER) -o $(NAME)

clean:
			/bin/rm -f $(OBJS)

fclean:		clean
			/bin/rm -f $(NAME)

re:			fclean all

.PHONY:		all clean fclean re