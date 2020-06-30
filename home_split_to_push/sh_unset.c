/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sh_unset.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkang <jkang@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/30 16:42:11 by jkang             #+#    #+#             */
/*   Updated: 2020/06/30 16:42:12 by jkang            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_alpha_num(char *s)
{
	while (*s != '\0')
	{
		if (!ft_isalnum(*s))
			return (0);
		++s;
	}
	return (1);
}

static int	is_var(char *token, t_list *env)
{
	int		i;
	t_list	*tmp;
	char	*var;

	tmp = env;
	while (tmp != 0)
	{
		i = 0;
		var = (char *)(tmp->content);
		while (var[i] != '=')
		{
			if (var[i] != token[i])
				break ;
			++i;
		}
		tmp = tmp->next;
		if (var[i] == '=')
			return (1);
	}
	return (0);
}

static int	unset_var(char *token, t_list *env)
{
	t_list *tmp;

	while (env != 0)
	{
		if (ft_strncmp((env->next)->content, token, ft_strlen(token)) == 0)
		{
			tmp = (env->next)->next;
			free((env->next)->content);
			free(env->next);
			env->next = tmp;
			return (1);
		}
		env = env->next;
	}
	return (0);
}

static int	sh_unset_body(char **tokens, int token_count, t_list *env)
{
	int i;

	i = 1;
	while (i < token_count)
	{
		if (!is_alpha_num(tokens[i]))
		{
			ft_putstr_fd("unset: ", 2);
			ft_putstr_fd(tokens[i], 2);
			ft_putendl_fd(": invalid parameter name", 2);
			return (0);
		}
		else if (is_var(tokens[i], env))
			unset_var(tokens[i], env);
		++i;
	}
	return (1);
}

int			sh_unset(char **tokens, t_list *env, int *p_status)
{
	int token_count;

	token_count = 0;
	while (tokens[token_count] != 0)
		++token_count;
	if (token_count == 1)
	{
		ft_putendl_fd("unset: not enough arguments", 2);
		*p_status = 1;
		return (0);
	}
	else
	{
		if (!(sh_unset_body(tokens, token_count, env)))
		{
			*p_status = 1;
			return (0);
		}
	}
	*p_status = 0;
	return (1);
}
