/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_cmds_with_fork.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkang <jkang@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/30 15:21:30 by jkang             #+#    #+#             */
/*   Updated: 2020/07/12 19:05:52 by jkang            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	parent_process(int *pipe_fd, int *p_status)
{
	int		status;
	pid_t	wait_pid;

	close(pipe_fd[1]);
	dup2(pipe_fd[0], 0);
	close(pipe_fd[0]);
	wait_pid = waitpid(g_pid, &status, 0);
	if (wait_pid == -1)
		ft_putstr_fd("wait_pid returns error. no child process", 2);
	g_pid = 0;
	*p_status = status / 256;
	return (1);
}

static int	main_cmd_with_fork(char **cmd, t_list *env,\
												int *p_status)
{
	int ret;

	if ((ret = exec_builtin_cmds(cmd, env, p_status)) == -1)
	{
		free_2d_array(cmd);
		return (0);
	}
	else if (ret == 0)
	{
		if (!(cmd_not_builtins(cmd, env)))
		{
			free_2d_array(cmd);
			return (0);
		}
	}
	free_2d_array(cmd);
	return (1);
}

int			process_cmds_with_fork(char ***cmds, t_list *env,\
													int *p_status)
{
	int		pipe_fd[2];
	char	**cmd;

	if (pipe(pipe_fd) == -1)
		return (0);
	if ((g_pid = fork()) == -1)
	{
		ft_putstr_fd("Fork is failed.", 2);
		return (0);
	}
	else if (g_pid == 0)
	{
		if (!(cmd = refine_cmd(cmds, pipe_fd)))
			exit(1);
		if (!main_cmd_with_fork(cmd, env, p_status))
			exit(1);
		exit(0);
	}
	else
	{
		if (!parent_process(pipe_fd, p_status))
			return (0);
	}
	return (1);
}
