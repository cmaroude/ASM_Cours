SRCS	=	ft_strlen.s \
				ft_strcpy.s \
				ft_strcmp.s \
				ft_write.s \
				ft_read.s \
				ft_strdup.s 

OBJS	=	${SRCS:.s=.o}

NASM	= nasm

NASMFLAGS	= -f elf64

CC = gcc

CFLAGS = -Wall -Werror -Wextra -no-pie

TEST = test

NAME	=	libasm.a

all:	${NAME}

${NAME}:	${OBJS}
					ar rcs ${NAME} ${OBJS}

%.o: %.s
	$(NASM) $(NASMFLAGS) $< -o $@

clean:
				rm -f ${OBJS}

fclean:	clean
				rm -f ${NAME}
				rm -f ${TEST}

re:	fclean all

$(TEST): $(NAME) main.c
	$(CC) $(CFLAGS) main.c -L. -lasm -o $(TEST)
	./$(TEST) < Makefile
