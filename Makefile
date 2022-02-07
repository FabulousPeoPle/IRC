# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: azouiten <azouiten@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/10/05 12:47:22 by ohachim           #+#    #+#              #
#    Updated: 2022/02/07 12:58:27 by azouiten         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = clang++

SRC_PATH = sources

SRC_NAME = main.cpp Server.cpp Client.cpp Message.cpp ServerGetters.cpp strToken.cpp Channel.cpp

OBJ_PATH = obj

OBJ_NAME = $(SRC_NAME:.cpp=.o)

OBJ = $(addprefix $(OBJ_PATH)/, $(OBJ_NAME))

INC = includes

HEADER_NAME =

HEADER = $(addprefix $(INC)/, $(HEADER_NAME))

CFLAGS = -pthread -Wall -Wextra -Werror

TARGET = irc_server

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC)  $^ -o $(TARGET)
	
$(OBJ_PATH)/%.o: $(SRC_PATH)/%.cpp $(HEADER)
	@mkdir $(OBJ_PATH) 2> /dev/null || true
	$(CC) $(CFLAGS) -I $(INC) -o $@ -c $<

clean:
	@rm -vf $(OBJ)
	@rmdir $(OBJ_PATH) 2> /dev/null || true

fclean: clean
	@rm -vf $(TARGET)

re: fclean all