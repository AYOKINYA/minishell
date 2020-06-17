#include "minishell.h"

static int is_numeric(char *s)
{
	while (*s != 0)
	{
		if (*s < '0' || *s > '9')
			return (0);
		++s;
	}
	return (1);
}

int sh_exit(char **tokens, int *p_status)
{
	int token_count;

	token_count = count_tokens(tokens);
	ft_putendl_fd("exit", 1);
	if (token_count == 2 && is_numeric(tokens[1]))
		exit(ft_atoi(tokens[1]));
	else if (token_count > 2 && is_numeric(tokens[1]))
	{
		ft_putendl_fd("bash: exit: too many arguments", 2);
		*p_status = 1;
	}
	else
		exit(2);
	return (1);
}