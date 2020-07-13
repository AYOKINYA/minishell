/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sh_echo.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkang <jkang@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/30 15:22:29 by jkang             #+#    #+#             */
/*   Updated: 2020/06/30 16:43:14 by jkang            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	sh_echo(char **tokens, int *p_status)
{
	int		token_count;
	int		i;

	token_count = count_tokens(tokens);
	i = 1;
	if (token_count == 1)
	{
		write(1, "\n", 1);
		*p_status = 0;
		return (1);
	}
	if (ft_strcmp(tokens[1], "-n") == 0)
		i = 2;
	while (tokens[i] != 0)
	{
		ft_putstr_fd(tokens[i], 1);
		if (i != token_count - 1)
			write(1, " ", 1);
		++i;
	}
	if (ft_strcmp(tokens[1], "-n") != 0)
		write(1, "\n", 1);
	*p_status = 0;
	return (1);
}
