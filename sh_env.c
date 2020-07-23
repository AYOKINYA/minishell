/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sh_env.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkang <jkang@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/30 15:08:44 by jkang             #+#    #+#             */
/*   Updated: 2020/06/30 15:24:16 by jkang            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	sh_env(char **tokens, t_list *env, int *p_status)
{
	int token_count;

	token_count = count_tokens(tokens);
	if (token_count > 1)
	{
		ft_putstr_fd("env: '", 2);
		ft_putstr_fd(tokens[1], 2);
		ft_putstr_fd("'", 2);
		ft_putendl_fd(": No such file or directory", 2);
		*p_status = 127;
		return (0);
	}
	env = env->next;
	while (env != 0)
	{
		ft_putstr_fd(env->content, 1);
		write(1, "\n", 1);
		env = env->next;
	}
	*p_status = 0;
	return (1);
}
