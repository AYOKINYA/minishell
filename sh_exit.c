/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sh_exit.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkang <jkang@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/30 15:24:38 by jkang             #+#    #+#             */
/*   Updated: 2020/07/09 19:56:00 by jkang            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_numeric(char *s)
{
	while (*s != 0)
	{
		if (!ft_isdigit(*s))
			return (0);
		++s;
	}
	return (1);
}

int			sh_exit(char **tokens)
{
	int token_count;

	token_count = count_tokens(tokens);
	ft_putendl_fd("exit", 1);
	if (token_count == 2 && is_numeric(tokens[1]))
		exit(ft_atoi(tokens[1]));
	else if (token_count > 2 && is_numeric(tokens[1]))
	{
		ft_putendl_fd("bash: exit: too many arguments", 2);
		g_status = 1;
	}
	else
		exit(2);
	return (1);
}
