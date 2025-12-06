# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile_RV                                        :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rvarela <rvarela@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/07/30 21:29:00 by rvarela           #+#    #+#              #
#    Updated: 2025/10/10 15:43:47 by rvarela          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# _______________________________COLORS_________________________________________#

RESET	= \033[0m
BLACK 	= \033[1;30m
RED 	= \033[1;31m
GREEN 	= \033[1;32m
YELLOW 	= \033[1;33m
BLUE	= \033[1;34m
PURPLE 	= \033[1;35m
CYAN 	= \033[1;36m
WHITE 	= \033[1;37m

#_______________________________COMPILATION____________________________________#

NAME 		= webserv

CXX			= c++
CXXFLAGS	= -Wall -Werror -Wextra -Wshadow -std=c++98 -pedantic -g -Wunused -fsanitize=address

# valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes

RM			= rm -rf

OBJS_DIR 	= objs
SRC_DIR     = src
INC_DIR     = includes

FILES 		= main utils HttpParser Socket Client Logger

SRC 		= 	$(addprefix $(SRC_DIR)/, $(addsuffix .cpp, $(FILES)))
OBJ 		= $(addprefix $(OBJS_DIR)/, $(addsuffix .o, $(FILES)))

#__________________________________RULES_______________________________________#

all: $(NAME)

$(NAME): $(OBJS_DIR) $(OBJ)
		@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME) -I $(INC_DIR)
		@echo "[$(GREEN)$(NAME) created$(RESET)]"

$(OBJS_DIR)/%.o : $(SRC_DIR)/%.cpp
		@$(CXX) $(CXXFLAGS) -c $< -o $@ -I $(INC_DIR)
		@echo "Created object files"

$(OBJS_DIR):
		@mkdir -p $(OBJS_DIR)
		@echo "[$(GREEN)objs dir created$(RESET)]"

clean:
		@$(RM) $(OBJS_DIR)
		@echo "[$(RED)objs removed$(RESET)]"

fclean: clean
		@$(RM) $(NAME)
		@echo "[$(RED)executables removed$(RESET)]"

re: fclean all

.PHONY: all clean fclean re
