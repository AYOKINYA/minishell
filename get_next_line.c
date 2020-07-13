/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkang <jkang@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/30 15:12:52 by jkang             #+#    #+#             */
/*   Updated: 2020/06/30 16:33:01 by jkang            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ft_has_new_line(char *buf, char **line)
{
	int		i;
	char	tmp[1 + 1];

	if (ft_strchr(buf, '\n') != 0)
	{
		i = 0;
		while (buf[i] != '\n')
		{
			tmp[i] = buf[i];
			++i;
		}
		tmp[i] = '\0';
		if (!(*line = ft_strjoin(*line, tmp)))
			return (-1);
		return (1);
	}
	return (0);
}

int			get_next_line(int fd, char **line)
{
	char			buf[1 + 1];
	size_t			ret;

	if (fd < 0 || line == 0 || (read(fd, buf, 0) < 0))
		return (-1);
	if (!(*line = ft_strdup("")))
		return (-1);
	buf[0] = '\0';
	while (*buf != '\n')
	{
		while ((ret = read(fd, buf, 1)) > 0)
		{
			buf[ret] = '\0';
			if (ft_has_new_line(buf, line) == 1)
				return (1);
			if (!(*line = ft_strjoin(*line, buf)))
				return (-1);
		}
		if (ft_strlen(*line) == 0 && *buf != '\n')
			return (0);
	}
	return (1);
}
