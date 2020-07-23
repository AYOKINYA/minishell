/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sh_export.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkang <jkang@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/30 16:41:10 by jkang             #+#    #+#             */
/*   Updated: 2020/07/07 19:38:50 by jkang            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	has_alpha(char *s, char until)
{
	while (*s != until)
	{
		if (ft_isalpha(*s))
			return (1);
		++s;
	}
	return (0);
}

static void	export_err_msg(char *token)
{
	ft_putstr_fd("bash: export: `", 2);
	ft_putstr_fd(token, 2);
	ft_putendl_fd("': not a valid identifier", 2);
}

static int	sh_export_body(char **tokens, t_list *env, int i)
{
	if (ft_strlen(tokens[i]) == 1 && ft_strchr(tokens[i], '_'))
		return (1);
	else if ((ft_strchr(tokens[i], '=') == 0 && !has_alpha(tokens[i], '\0'))\
								|| is_not_valid(tokens[i], '='))
	{
		export_err_msg(tokens[i]);
		return (-1);
	}
	else if (ft_strchr(tokens[i], '=') != 0)
	{
		if (!has_alpha(tokens[i], '='))
		{
			export_err_msg(tokens[i]);
			return (-1);
		}
		if (!is_var_already(tokens[i], env))
		{
			if (!export_new_var(tokens[i], env))
				return (0);
		}
	}
	return (1);
}

static void	print_env_w_declare(t_list *env)
{
	env = env->next;
	while (env != 0)
	{
		ft_putstr_fd("declare -x ", 1);
		ft_putendl_fd(env->content, 1);
		env = env->next;
	}
}

int			sh_export(char **tokens, t_list *env)
{
	int		i;
	int		token_count;
	int		ret;

	token_count = count_tokens(tokens);
	if (token_count == 1)
		print_env_w_declare(env);
	else
	{
		i = 0;
		while (++i < token_count)
		{
			if (!(ret = sh_export_body(tokens, env, i)))
			{
				g_status = 1;
				return (0);
			}
			else if (ret == -1)
				g_status = 1;
		}
	}
	g_status = 0;
	return (1);
}
