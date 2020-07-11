/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_split.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkang <jkang@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/30 15:14:10 by jkang             #+#    #+#             */
/*   Updated: 2020/07/11 14:49:41 by jkang            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	count_cmds(char **args)
{
	int count;

	count = 1;
	while (*args != 0)
	{
		if (**args == '|' * -1)
			++count;
		++args;
	}
	return (count);
}

static int	count_args(char **args)
{
	int count;

	count = 0;
	while (*args != 0)
	{
		if (**args == '|' * -1)
			break ;
		++count;
		++args;
	}
	return (count);
}

static char	**pipe_split_body(char ***args)
{
	int		j;
	int		args_count;
	char	**res;

	args_count = count_args(*args);
	if (!(res = (char **)malloc(sizeof(char *) * (args_count + 1))))
		return (0);
	res[args_count] = 0;
	j = -1;
	while (++j < args_count)
	{
		if (!(res[j] = ft_strdup(**args)))
		{
			free_2d_array(res);
			return (0);
		}
		++(*args);
	}
	return (res);
}

char		***pipe_split(char **args)
{
	int		cmds_count;
	int		i;
	char	***res;

	cmds_count = count_cmds(args);
	if (!(res = (char ***)malloc(sizeof(char **) * (cmds_count + 1))))
		return (0);
	res[cmds_count] = 0;
	i = 0;
	while (i < cmds_count)
	{
		if (!(res[i] = pipe_split_body(&args)))
		{
			free_3d_array(res);
			return (0);
		}
		++i;
		++args;
	}
	return (res);
}
