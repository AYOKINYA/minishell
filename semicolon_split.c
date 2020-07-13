/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   semicolon_split.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkang <jkang@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/30 15:23:58 by jkang             #+#    #+#             */
/*   Updated: 2020/07/13 11:51:22 by jkang            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ft_num_cmds(char *line)
{
	int num_cmds;
	int quote;

	quote = 0;
	num_cmds = 1;
	while (*line != '\0')
	{
		quote_mark(&quote, *line);
		if (quote == 0 && *line == ';')
			++num_cmds;
		++line;
	}
	return (num_cmds);
}

static int	ft_cmd_len(char *line)
{
	int cmd_len;
	int quote;

	cmd_len = 0;
	quote = 0;
	while (*line != '\0')
	{
		quote_mark(&quote, *line);
		if (quote == 0 && *line == ';')
			break ;
		++line;
		++cmd_len;
	}
	return (cmd_len);
}

static int	only_semicolon_body(char **line, int semicolon_count,\
													int other_count)
{
	while (**line != ';' && **line != '\0')
	{
		if (**line != ' ')
			++other_count;
		++(*line);
	}
	while (**line == ';')
	{
		++semicolon_count;
		++(*line);
	}
	if (other_count == 0 || semicolon_count >= 2)
	{
		if (semicolon_count == 1)
		{
			ft_putendl_fd("bash: syntax error near unexpected token ';'", 2);
			return (0);
		}
		else if (semicolon_count > 1)
		{
			ft_putendl_fd("bash: syntax error near unexpected token ';;'", 2);
			return (0);
		}
	}
	return (1);
}

int			only_semicolon(char *line)
{
	int		semicolon_count;
	int		other_count;

	while (*line != '\0')
	{
		semicolon_count = 0;
		other_count = 0;
		while (*line == ';')
		{
			++semicolon_count;
			++line;
		}
		if (other_count == 0 && semicolon_count == 1)
		{
			ft_putendl_fd("bash: syntax error near unexpected token ';'", 2);
			return (1);
		}
		if (!only_semicolon_body(&line, semicolon_count, other_count))
			return (1);
		if (*line == '\0')
			break ;
		++line;
	}
	return (0);
}

char		**semicolon_split(char *line)
{
	int		i;
	int		num_cmds;
	int		cmd_len;
	char	**cmds;

	num_cmds = ft_num_cmds(line);
	if (!(cmds = (char **)malloc(sizeof(char *) * (num_cmds + 1))))
		return (0);
	i = 0;
	while (i < num_cmds)
	{
		cmd_len = ft_cmd_len(line);
		if (!(cmds[i] = ft_substr(line, 0, cmd_len)))
		{
			ft_free(cmds, i);
			return (0);
		}
		line += cmd_len + 1;
		++i;
	}
	cmds[num_cmds] = 0;
	return (cmds);
}
