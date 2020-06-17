#include "minishell.h"

static int	parent_process(pid_t pid, int *pipe_fd, int *p_status)
{
	int		status;
	pid_t	wait_pid;

	close(pipe_fd[1]);
	dup2(pipe_fd[0], 0);
	close(pipe_fd[0]);
	wait_pid = waitpid(pid, &status, 0);
	if (wait_pid == -1)
		ft_putstr_fd("wait_pid returns error. no child process", 2);
	*p_status = status / 256;
	return (1);
}

static int	main_cmd_with_fork(char **cmd, t_list *env, char **envp, int *p_status)
{
	int ret;

	if ((ret = exec_builtin_cmds(cmd, env, p_status)) == -1)
	{
		free_2d_array(cmd);
		return(0);
	}
	else if (ret == 0)
	{
		if (!(cmd_not_builtins(cmd, envp)))
		{
			free_2d_array(cmd);
			return(0);
		}
	}
	free_2d_array(cmd);
	return (1);
}


int			process_cmds_with_fork(char ***cmds, t_list *env, char **envp, int *p_status)
{
	pid_t	pid;
	int		pipe_fd[2];
	char	**cmd;
	
	pipe(pipe_fd);
	if ((pid = fork()) == -1)
	{
		ft_putstr_fd("Fork is failed.", 2);
		return (0);
	}
	else if (pid == 0)
	{
		if (!(cmd = refine_cmd(cmds, pipe_fd)))
			exit(1);
		if (!main_cmd_with_fork(cmd, env, envp, p_status)) //2d array cmd free처리 안에서 다 한다.
			exit(1);
		exit(0);
	}
	else
	{	
		if (!parent_process(pid, pipe_fd, p_status))
			return (0);
	}
	return (1);
}