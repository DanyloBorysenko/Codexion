NAME=codexion
CC = cc
FLAGS = -Wall -Wextra -Werror -pthread
RM = rm -f
HEADER = codexion.h
MY_SRC = arg_parser.c    \
         main.c          \
		 time_counter.c  \
		 work.c        \
		 locks.c         \
		 logger.c        \
		 initialisation.c \
		 dongles.c
OBJ = $(MY_SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(FLAGS) $(OBJ) -g -o $(NAME)

%.o: %.c $(HEADER)
	$(CC) $(FLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
