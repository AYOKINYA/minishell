#include "minishell.h"

static int	exec_not_builtin_cmds(char **cmd, char **envp, int *p_status)
{
	pid_t	pid;
	int		status;
	
	if ((pid = fork()) == -1)
	{
		ft_putendl_fd("FORK FAILED", 2);
		return (0);
	}
	else if (pid == 0)
	{
		if (!cmd_not_builtins(cmd, envp))
			exit(1);
		exit(0);
	}
	else
	{		
		if (waitpid(pid, &status, 0) == -1)
		{
			ft_putendl_fd("waitpid returns error. no child process", 2);
			return (0);
		}
		*p_status = status / 256;
	}
	return (1);
}

int			process_cmd_without_fork(char ***cmds, t_list *env, char **envp, int *p_status)
{
	int ret;
	
	if ((*cmds)[0] == 0)
		return (1);
	if (!check_fd_aggregation(*cmds, p_status))
		return (1);
	if (!(ret = exec_builtin_cmds(*cmds, env, p_status)))
	{
		if (!exec_not_builtin_cmds(*cmds, envp, p_status))
			return (0);
	}
	else if (ret == -1)
		return (0);
	return (1);
}
