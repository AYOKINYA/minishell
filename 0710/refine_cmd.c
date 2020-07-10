/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   refine_cmd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkang <jkang@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/30 15:26:35 by jkang             #+#    #+#             */
/*   Updated: 2020/06/30 15:26:35 by jkang            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	**get_cmd(char **cmd, int redirection)
{
	int		count;
	int		i;
	char	**res;

	count = 0;
	while (cmd[count] != 0)
		++count;
	if (redirection)
		count -= 2;
	if (!(res = (char **)malloc(sizeof(char *) * (count + 1))))
		return (0);
	res[count] = 0;
	i = -1;
	while (++i < count)
	{
		if (is_redirection(**cmd))
			cmd += 2;
		if (!(res[i] = ft_strdup(*cmd)))
		{
			ft_free(res, i);
			return (0);
		}
		++cmd;
	}
	return (res);
}

static int	check_redirection(char **cmd, int *file_fd)
{
	int redirection;

	redirection = 0;
	while (*cmd != 0)
	{
		if (**cmd == '>' * -1 && *(*cmd + 1) != '>' * -1)
			redirection = 1;
		else if (**cmd == '>' * -1 && *(*cmd + 1) == '>' * -1)
			redirection = 2;
		else if (**cmd == '<' * -1)
			redirection = 3;
		if (redirection > 0)
			break ;
		++cmd;
	}
	if (redirection == 1)
		*file_fd = open(*(cmd + 1), O_RDWR | O_CREAT | O_TRUNC | S_IROTH, 0644);
	else if (redirection == 2)
		*file_fd = open(*(cmd + 1), O_RDWR | O_APPEND | S_IROTH, 0644);
	else if (redirection == 3)
		*file_fd = open(*(cmd + 1), O_RDONLY);
	else
		*file_fd = 1;
	return (redirection);
}

static int	process_pipe_and_redirection(char ***cmds, int *pipe_fd,\
													int *redirection)
{
	int file_fd;

	close(pipe_fd[0]);
	if (*(cmds + 1) != 0)
		dup2(pipe_fd[1], 1);
	close(pipe_fd[1]);
	*redirection = check_redirection(*cmds, &file_fd);
	if (file_fd < 0)
	{
		ft_putendl_fd("file fd error!", 2);
		return (0);
	}
	else if (*redirection == 1 || *redirection == 2)
		dup2(file_fd, 1);
	else if (*redirection == 3)
		dup2(file_fd, 0);
	return (1);
}

char		**refine_cmd(char ***cmds, int pipe_fd[])
{
	char	**cmd;
	int		redirection;

	if (!process_pipe_and_redirection(cmds, pipe_fd, &redirection))
		return (0);
	if (!(cmd = get_cmd(*cmds, redirection)))
		return (0);
	return (cmd);
}
