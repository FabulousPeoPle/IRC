# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ohachim <ohachim@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/10/05 12:47:22 by ohachim           #+#    #+#              #
#    Updated: 2022/03/03 13:22:54 by azouiten         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


CC = clang++


SRC_PATH_BOT = irc_bot/sources
SRC_NAME_BOT = main.cpp Bot.cpp currentTime.cpp utils.cpp wikiFetcher.cpp
OBJ_PATH_BOT = irc_bot/obj_bot
OBJ_NAME_BOT = $(SRC_NAME_BOT:.cpp=.o)
OBJ_BOT = $(addprefix $(OBJ_PATH_BOT)/, $(OBJ_NAME_BOT))
HEADER_NAME_BOT = Bot.hpp cmds.hpp utils.hpp
INC_BOT = irc_bot/includes
HEADER_BOT = $(addprefix $(INC_BOT)/, $(HEADER_NAME_BOT))

SRC_PATH_FTP = irc_bot/ftp_client/sources
SRC_NAME_FTP = send_ftp.cpp ssplit.cpp
OBJ_PATH_FTP = irc_bot/ftp_client/obj
OBJ_NAME_FTP = $(SRC_NAME_FTP:.cpp=.o)
OBJ_FTP = $(addprefix $(OBJ_PATH_FTP)/, $(OBJ_NAME_FTP))
HEADER_NAME_FTP = ssplit.hpp
INC_FTP = irc_bot/ftp_client/includes
HEADER_FTP = $(addprefix $(INC_FTP)/, $(HEADER_NAME_FTP))

SRC_PATH = sources
SRC_NAME = main.cpp Server.cpp Client.cpp Message.cpp ServerGetters.cpp strToken.cpp Channel.cpp ServerReply.cpp intToString.cpp
OBJ_PATH = obj
OBJ_NAME = $(SRC_NAME:.cpp=.o)
OBJ = $(addprefix $(OBJ_PATH)/, $(OBJ_NAME))
HEADER_NAME = Client.hpp Message.hpp Server.hpp Channel.hpp
INC = includes
HEADER = $(addprefix $(INC)/, $(HEADER_NAME))

# CFLAGS =  -Wall -Wextra -Werror

TARGET = irc_server
TARGET_BOT = bota
TARGET_FTP = ftp_client

all: $(TARGET) $(TARGET_BOT) $(TARGET_FTP)

$(TARGET): $(OBJ)
	$(CC)  $^ -o $(TARGET)
	
$(OBJ_PATH)/%.o: $(SRC_PATH)/%.cpp $(HEADER)
	@mkdir $(OBJ_PATH) 2> /dev/null || true
	$(CC) $(CFLAGS) -I $(INC) -o $@ -c $<

$(TARGET_BOT): $(OBJ_BOT)
	$(CC)  $^ -o $(TARGET_BOT)

$(OBJ_PATH_BOT)/%.o: $(SRC_PATH_BOT)/%.cpp $(HEADER_BOT)
	@mkdir $(OBJ_PATH_BOT) 2> /dev/null || true
	$(CC) $(CFLAGS) -I $(INC_BOT) -o $@ -c $<

$(TARGET_FTP): $(OBJ_FTP)
	$(CC)  $^ -o $(TARGET_FTP)

$(OBJ_PATH_FTP)/%.o: $(SRC_PATH_FTP)/%.cpp $(HEADER_FTP)
	@mkdir $(OBJ_PATH_FTP) 2> /dev/null || true
	$(CC) $(CFLAGS) -I $(INC_FTP) -o $@ -c $<

clean:
	@rm -vf $(OBJ)
	@rmdir $(OBJ_PATH) 2> /dev/null || true
	@rm -vf $(OBJ_BOT)
	@rmdir $(OBJ_PATH_BOT) 2> /dev/null || true
	@rm -vf $(OBJ_FTP)
	@rmdir $(OBJ_PATH_FTP) 2> /dev/null || true

fclean: clean
	@rm -vf $(TARGET)
	@rm -vf $(TARGET_BOT)
	@rm -vf $(TARGET_FTP)

re: fclean all