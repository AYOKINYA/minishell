#include "minishell.h"

int			is_redirection(char c)
{
	if (c == '<' * -1 || c == '>' * -1 || c == '|' * -1 )
		return (1);
	return (0);
}

int			has_redirection(char *arg)
{
	while (*arg != '\0')
	{
		if (is_redirection(*arg))
			return (1);
		++arg;
	}
	return (0);
}

static int	investigate_redirection(char **args, int i)
{
	if (args[i + 1] == 0 || ft_strlen(args[i]) > 2) //redirection 다음 arg 없을 때!
	{
		ft_putendl_fd("syntax error!", 2);	
		return (0);
	}
	else if (ft_strlen(args[i]) == 2)
	{
		if (args[i][0] != '>' * -1 || args[i][1] != '>' * -1)
		{
			ft_putendl_fd("syntax error!", 2);	
			return (0);
		}
	}
	return (1);
}

int			validate_redirection(char **args)
{
	int i;

	i = 0;
	while (args[i] != 0)
	{
		if (has_redirection(args[i]))
		{
			if (!investigate_redirection(args, i))
				return (0);
		}
		++i;
	}
	return (1);
}