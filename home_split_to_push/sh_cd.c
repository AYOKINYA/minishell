/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sh_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkang <jkang@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/30 15:23:07 by jkang             #+#    #+#             */
/*   Updated: 2020/06/30 15:23:08 by jkang            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	sh_ch_homedir(t_list *env, int *p_status)
{
	while (env != 0)
	{
		if (ft_strncmp(env->content, "HOME=", 5) == 0)
		{
			if (chdir(env->content + 5) == -1)
			{
				ft_putstr_fd("bash: cd: ", 2);
				ft_putstr_fd(env->content + 5, 2);
				ft_putendl_fd(": no such file or directory", 2);
				*p_status = 1;
				return (0);
			}
		}
		env = env->next;
	}
	return (1);
}

static int	sh_chdir(char **tokens, int *p_status)
{
	if (chdir(tokens[1]) == -1)
	{
		ft_putstr_fd("bash: cd: ", 2);
		ft_putstr_fd(tokens[1], 2);
		ft_putendl_fd(": no such file or directory", 2);
		*p_status = 1;
		return (0);
	}
	return (1);
}

int			sh_cd(char **tokens, t_list *env, int *p_status)
{
	int token_count;

	token_count = count_tokens(tokens);
	if (token_count == 1 || (token_count == 2 &&\
		ft_strlen(tokens[1]) == 1 && tokens[1][0] == '~'))
	{
		if (!sh_ch_homedir(env, p_status))
			return (0);
	}
	else if (token_count > 2)
	{
		ft_putendl_fd("bash: cd: too many arguments", 2);
		*p_status = 1;
	}
	else
	{
		if (!(sh_chdir(tokens, p_status)))
			return (0);
	}
	*p_status = 0;
	return (1);
}
