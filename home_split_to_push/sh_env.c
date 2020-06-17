#include "minishell.h"

int sh_env(char **tokens, t_list *env, int *p_status)
{
	int token_count;

	token_count = count_tokens(tokens);
	if (token_count > 1)
	{
		ft_putstr_fd("env: '", 2);
		ft_putstr_fd(tokens[1], 2);
		ft_putstr_fd("'", 2);
		ft_putendl_fd(": No such file or directory", 2);
		*p_status = 1;
		return (0);
	}
	env = env->next;
	while (env != 0)
	{
		ft_putstr_fd(env->content, 1);
		write(1, "\n", 1);
		env = env->next;
	}
	*p_status = 0;
	return (1);
}