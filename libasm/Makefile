SRCS	=	ft_strlen.s \
				ft_strcpy.s \
				ft_strcmp.s \
				ft_write.s \
				ft_read.s \
				ft_strdup.s 

OBJS	=	${SRCS:.s=.o}

BONUS_SRCS	=	ft_list_push_front.s \
							ft_list_size.s \
							ft_list_remove_if.s \
							ft_list_sort.s \
							ft_atoi_base.s

BONUS_OBJS	=	${BONUS_SRCS:.s=.o}

NASM	= nasm

NASMFLAGS	= -f elf64

CC = gcc

CFLAGS = -Wall -Werror -Wextra

TEST = test

TEST_BONUS = test_bonus

NAME	=	libasm.a

all:	${NAME}

${NAME}:	${OBJS}
					ar rcs ${NAME} ${OBJS}

%.o: %.s
	${NASM} ${NASMFLAGS} $< -o $@

clean:
				rm -f ${OBJS} ${BONUS_OBJS}

fclean:	clean
				rm -f ${NAME} ${TEST} ${TEST_BONUS}

re:	fclean all

test:	${NAME} main.c
					${CC} ${CFLAGS} main.c -L. -lasm -o ${TEST}
					./${TEST} < Makefile

bonus:	${OBJS} ${BONUS_OBJS}
				ar rcs ${NAME} ${OBJS} ${BONUS_OBJS}

test_bonus:	${NAME} main_bonus.c
						${CC} ${CFLAGS} main_bonus.c -L. -lasm -o ${TEST_BONUS}
						./${TEST_BONUS}
