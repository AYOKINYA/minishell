/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_cmd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkang <jkang@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/30 15:21:34 by jkang             #+#    #+#             */
/*   Updated: 2020/06/30 15:21:35 by jkang            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int			check_fd_aggregation(char **args, int *p_status)
{
	int count;
	int i;
	int j;

	count = 0;
	i = -1;
	while (args[++i] != 0)
	{
		j = 0;
		while (args[i][j] != '\0')
		{
			if (args[i][j] == '<' * -1 || args[i][j] == '>' * -1)
				++count;
			if (args[i][j] == '>' * -1 && args[i][j + 1] == '>' * -1)
				count -= 1;
			++j;
		}
	}
	if (count > 1)
	{
		ft_putendl_fd("fd aggregation is not supported.", 2);
		*p_status = 1;
		return (0);
	}
	return (1);
}

static int	if_no_need_pipe(char ***cmds)
{
	int len;
	int redirection_flag;
	int i;

	len = 0;
	while (cmds[len] != 0)
		++len;
	redirection_flag = 0;
	i = 0;
	while (cmds[0][i] != 0)
	{
		if (has_redirection(cmds[0][i]))
		{
			redirection_flag = 1;
			break ;
		}
		++i;
	}
	if (len == 1 && !redirection_flag)
		return (1);
	return (0);
}

int			process_cmd(char ***cmds, t_list *env, char **envp, int *p_status)
{
	if (if_no_need_pipe(cmds))
	{
		if (!process_cmd_without_fork(cmds, env, envp, p_status))
			return (0);
		return (1);
	}
	else
	{
		while (*cmds)
		{
			if ((*(cmds))[0] == 0)
				return (1);
			if (!validate_redirection(*cmds))
				return (1);
			if (!check_fd_aggregation(*cmds, p_status))
				return (1);
			if (!process_cmds_with_fork(cmds, env, envp, p_status))
				return (0);
			++cmds;
		}
	}
	return (1);
}
