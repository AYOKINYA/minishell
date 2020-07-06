/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   convert_to_env_var.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkang <jkang@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/30 15:12:35 by jkang             #+#    #+#             */
/*   Updated: 2020/07/05 20:49:25 by jkang            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	add_var(char **s, t_list *env, char **res)
{
	int		i;
	char	*copy;

	++(*s);
	if (**s == '\0')
		return (1);
	copy = *s;
	i = 0;
	while (**s != '\0' && **s != '$' * -1 && **s != '\'' && **s != ' ')
	{
		++i;
		++(*s);
	}
	while (env != 0)
	{
		if (ft_strncmp(env->content, copy, i) == 0 &&\
											((char *)env->content)[i] == '=')
			get_value(env->content, res);
		env = env->next;
	}
	return (1);
}

static int	var_len(char **s, t_list *env)
{
	int		i;
	int		value_len;
	char	*copy;

	++(*s);
	if (**s == '\0')
		return (0);
	copy = *s;
	i = 0;
	value_len = 0;
	while (**s != '\0' && **s != '$' * -1)
	{
		++i;
		++(*s);
	}
	while (env != 0)
	{
		if (ft_strncmp(env->content, copy, i) == 0 &&\
											((char *)env->content)[i] == '=')
			value_len = get_value_len(env->content);
		env = env->next;
	}
	return (value_len);
}

static int	converted_len(char *s, t_list *env)
{
	int len;

	len = 0;
	while (*s != '\0')
	{
		if (*s == '$' * -1 && *(s + 1) != '\0' && *(s + 1) != '$' * -1)
			len += var_len(&s, env);
		else
		{
			++len;
			++s;
		}
	}
	return (len);
}

char		*convert_var_to_value(char *content, t_list *env)
{
	int		len;
	char	*s;
	char	*res;
	char	*ret;

	s = content;
	len = converted_len(s, env);
	if (!(ret = (char *)malloc(sizeof(char) * (len + 1))))
		return (0);
	ret[len] = 0;
	res = ret;
	while (*s != '\0')
	{
		if (*s == '$' * -1 && *(s + 1) != '\0' && *(s + 1) != '$' * -1)
			add_var(&s, env, &res);
		else
		{
			if (*s == '$' * -1)
				*s *= -1;
			*(res++) = *(s++);
		}
	}
	return (ret);
}
