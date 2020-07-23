/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_input.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkang <jkang@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/30 16:33:41 by jkang             #+#    #+#             */
/*   Updated: 2020/07/11 17:10:01 by jkang            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	***get_cmds_with_pipe_split(char *line, t_list *env)
{
	char	**args;
	char	***cmds;

	if (!(args = tokenize(line, env)))
		return (0);
	if (!(cmds = pipe_split(args)))
	{
		free_2d_array(args);
		return (0);
	}
	free_2d_array(args);
	return (cmds);
}

static int	split_pipe_and_process_cmd(char *input, t_list *env)
{
	int		save[2];
	char	***cmds;

	if (!(cmds = get_cmds_with_pipe_split(input, env)))
		return (0);
	save[0] = dup(0);
	save[1] = dup(1);
	if (!process_cmd(cmds, env))
	{
		free_3d_array(cmds);
		return (0);
	}
	dup2(save[0], 0);
	dup2(save[1], 1);
	free_3d_array(cmds);
	return (1);
}

static int	dollar_question(t_list *env)
{
	char *new;
	char *var;
	char *status;

	if (!(status = ft_itoa(g_status)))
		return (0);
	if (!(var = ft_strdup("?=")))
	{
		free(status);
		return (0);
	}
	if (!(new = ft_strjoin(var, status)))
	{
		free(var);
		free(status);
		return (0);
	}
	free(env->content);
	if (!(env->content = new))
	{
		free(status);
		return (0);
	}
	free(status);
	return (1);
}

int			exec_input(char *line, t_list *env)
{
	int		i;
	char	**inputs;

	if (only_semicolon(line))
		return (1);
	if (!(inputs = semicolon_split(line)))
		return (0);
	i = -1;
	while (inputs[++i] != 0)
	{
		if (g_sig_status == 1)
			g_status = 130;
		if (!dollar_question(env))
			return (0);
		if (!split_pipe_and_process_cmd(inputs[i], env))
		{
			free_2d_array(inputs);
			return (0);
		}
		if (g_sig_status == 1)
			g_sig_status = 0;
	}
	free_2d_array(inputs);
	return (1);
}
