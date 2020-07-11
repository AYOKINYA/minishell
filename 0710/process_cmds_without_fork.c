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

static int	exec_not_builtin_cmds(char **cmd, int *p_status, t_list *env)
{
	int		status;

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
		if (waitpid(g_pid, &status, 0) == -1)
		{
			ft_putendl_fd("waitpid returns error. no child process", 2);
			return (0);
		}
		g_pid = 0;
		*p_status = status / 256;
	}
	return (1);
}

int			process_cmd_without_fork(char ***cmds, t_list *env,\
										int *p_status)
{
	int ret;

	if ((*cmds)[0] == 0)
		return (1);
	if (!check_fd_aggregation(*cmds, p_status))
		return (1);
	if (!(ret = exec_builtin_cmds(*cmds, env, p_status)))
	{
		if (!exec_not_builtin_cmds(*cmds, p_status, env))
			return (0);
	}
	else if (ret == -1)
		return (0);
	return (1);
}
