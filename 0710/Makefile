NAME = minishell
CC = gcc

CFLAGS = -g -Wall -Wextra -Werror

SRCS =  minishell.c \
		get_next_line.c\
		sh_echo.c\
		sh_exit.c\
		sh_env.c\
		sh_pwd.c\
		sh_cd.c\
		sh_export.c\
		sh_export_2.c\
		sh_unset.c\
		search_with_path.c\
		free.c\
		pipe_split.c\
		semicolon_split.c\
		redirection_split.c\
		read_quoted_token.c\
		read_unquoted_token.c\
		patch_token.c\
		validate_redirection.c\
		tokenize.c\
		get_value.c\
		convert_to_env_var.c\
		refine_cmd.c\
		process_cmds_with_fork.c\
		process_cmds_without_fork.c\
		process_cmd.c\
		exec_input.c\
		exec_builtin_cmds.c\
		cmds_into_tokens.c \
		make_env_to_envp.c

OBJS = $(SRCS:.c=.o)

LIB_FT = libft/libft.a #-L./libft -lft 로 해도 같다!

RM = rm -f


$(NAME) : ${OBJS} ${LIB_FT}
		${CC} ${CFLAGS} ${OBJS} ${LIB_FT} -o $(NAME)

%.o : %.c
		${CC} ${CFLAGS} -c $< -o $@

${LIB_FT} :
		  make -C ./libft/

all :	${NAME}

clean:
		${RM} ${OBJS}
		make -C ./libft clean 

fclean:	clean
		${RM} ${NAME}
		make -C ./libft fclean

re : fclean all

.PHONY : all re clean fclean
