# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ohachim <ohachim@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/10/05 12:47:22 by ohachim           #+#    #+#              #
#    Updated: 2022/03/03 11:47:49 by ohachim          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


CC = clang++

SRC_PATH = sources

SRC_NAME = main.cpp Server.cpp Client.cpp Message.cpp ServerGetters.cpp strToken.cpp Channel.cpp ServerReply.cpp intToString.cpp

OBJ_PATH = obj

OBJ_NAME = $(SRC_NAME:.cpp=.o)

OBJ = $(addprefix $(OBJ_PATH)/, $(OBJ_NAME))

INC = includes

HEADER_NAME = Client.hpp Message.hpp Server.hpp Channel.hpp

HEADER = $(addprefix $(INC)/, $(HEADER_NAME))

# CFLAGS =  -Wall -Wextra -Werror

TARGET = irc_server

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC)  $^ -o $(TARGET)
	
$(OBJ_PATH)/%.o: $(SRC_PATH)/%.cpp $(HEADER)
	@mkdir $(OBJ_PATH) 2> /dev/null || true
	$(CC) $(CFLAGS) -I $(INC) -o $@ -c $<

bot:
	g++ sources/bot.cpp -o bot
clean:
	@rm -vf $(OBJ)
	@rm -vf bot
	@rmdir $(OBJ_PATH) 2> /dev/null || true

fclean: clean
	@rm -vf $(TARGET)

re: fclean all