/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtin_cmds.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkang <jkang@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/30 15:13:13 by jkang             #+#    #+#             */
/*   Updated: 2020/07/13 14:02:46 by jkang            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int			count_tokens(char **tokens)
{
	int token_count;

	token_count = 0;
	while (tokens[token_count] != 0)
		++token_count;
	return (token_count);
}

static void	slash_error(char **tokens)
{
	int	i;

	i = 0;
	while (tokens[0][i] != '\0')
	{
		if (tokens[0][i] != '/')
			break ;
		++i;
	}
	if (tokens[0][i] == '\0')
		i = -1;
	ft_putstr_fd("bash: ", 2);
	ft_putstr_fd(tokens[0], 2);
	if (i == -1)
		ft_putendl_fd(": is a directory", 2);
	else
		ft_putendl_fd(": No such file or directory", 2);
	exit(127);
}

static void	cmd_not_found_error(char **tokens)
{
	ft_putstr_fd("bash: ", 2);
	ft_putstr_fd(tokens[0], 2);
	ft_putendl_fd(": command not found", 2);
	exit(127);
}

int			cmd_not_builtins(char **tokens, t_list *env)
{
	int		ret;
	char	**envp;

	if (!(envp = make_env_to_envp(env)))
		exit(1);
	if (ft_strcmp(tokens[0], "\0") == 0)
		return (1);
	if (ft_strncmp(tokens[0], "/", 1) == 0 || !path_is_in_envp(envp))
	{
		if ((ret = execve(tokens[0], tokens, envp)) == -1)
			slash_error(tokens);
	}
	else
	{
		if (!(ret = search_with_path(tokens, envp)))
		{
			free(envp);
			return (0);
		}
		else if (ret == 1)
			cmd_not_found_error(tokens);
	}
	free_2d_array(envp);
	return (1);
}

int			exec_builtin_cmds(char **tokens, t_list *env, int *p_status)
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
