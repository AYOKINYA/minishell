#include "minishell.h"

int	count_tokens(char **tokens)
{
	int token_count;

	token_count = 0;
	while (tokens[token_count] != 0)
		++token_count;
	return (token_count);
}

int	cmd_not_builtins(char **tokens, char **envp)
{
	int ret;

	if (ft_strncmp(tokens[0], "/", 1) == 0 || ft_strncmp(tokens[0], "/", 1) == 0)
	{
		if ((ret = execve(tokens[0], tokens, envp)) == -1)
		{
			ft_putstr_fd(tokens[0], 2);
			ft_putendl_fd(": command not found", 2);
			exit(1);
		}
	}
	else
	{
		if (!(ret = search_with_path(tokens, envp)))
			return (0);
		else if (ret == 1)
		{
			ft_putstr_fd(tokens[0], 2);
			ft_putendl_fd(": command not found", 2);
			exit(1);
		}
	}
	return (1);
}

int	exec_builtin_cmds(char **tokens, t_list *env, int *p_status)
{
	if (ft_strcmp(tokens[0], "exit") == 0)
		sh_exit(tokens, p_status);
	else if (ft_strcmp(tokens[0], "cd") == 0)
		sh_cd(tokens, env, p_status);
	else if (ft_strcmp(tokens[0], "export") == 0)
	{
		if (!sh_export(tokens, env, p_status))
			return (-1);
	}
	else if (ft_strcmp(tokens[0], "unset") == 0)
		sh_unset(tokens, env, p_status);
	else if (ft_strcmp(tokens[0], "echo") == 0)
		sh_echo(tokens, p_status);
	else if (ft_strcmp(tokens[0], "pwd") == 0)
	{
		if (!sh_pwd(p_status))
			return (-1);
	}
	else if (ft_strcmp(tokens[0], "env") == 0)
		sh_env(tokens, env, p_status);
	else
		return (0);
	return (1);
}