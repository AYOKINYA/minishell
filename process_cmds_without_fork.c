/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_cmds_without_fork.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkang <jkang@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/30 15:09:23 by jkang             #+#    #+#             */
/*   Updated: 2020/07/11 17:13:19 by jkang            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	parent_process(void)
{
	int		status;

	if (waitpid(g_pid, &status, 0) == -1)
	{
		ft_putendl_fd("waitpid returns error. no child process", 2);
		return (0);
	}
	g_pid = 0;
	g_status = status / 256;
	if (status == 2)
		g_status = 130;
	return (1);
}

static int	exec_not_builtin_cmds(char **cmd, t_list *env)
{
	if ((g_pid = fork()) == -1)
	{
		ft_putendl_fd("FORK FAILED", 2);
		return (0);
	}
	else if (g_pid == 0)
	{
		if (!cmd_not_builtins(cmd, env))
			exit(1);
		exit(0);
	}
	else
	{
		if (!parent_process())
			return (0);
	}
	return (1);
}

int			process_cmd_without_fork(char ***cmds, t_list *env)
{
	int ret;

	if ((*cmds)[0] == 0)
		return (1);
	if (!check_fd_aggregation(*cmds))
		return (1);
	if (!(ret = exec_builtin_cmds(*cmds, env)))
	{
		if (!exec_not_builtin_cmds(*cmds, env))
			return (0);
	}
	else if (ret == -1)
		return (0);
	return (1);
}
