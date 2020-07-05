/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sh_export_2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkang <jkang@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/07/05 13:12:15 by jkang             #+#    #+#             */
/*   Updated: 2020/07/05 13:55:32 by jkang            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int is_not_valid(char *s, char until)
{
	while (*s != until)
	{
		if (!ft_isalnum(*s) && *s != '=' && *s != '-')
			return (1);
		++s;
	}
	return (0);
}

int	export_new_var(char *token, t_list *env)
{
	t_list	*new;

	if (!(new = (t_list *)malloc(sizeof(t_list))))
		return (0);
	if (!(new->content = ft_strdup(token)))
	{
		free(new);
		return (0);
	}
	new->next = 0;
	ft_lstadd_back(&env, new);
	return (1);
}

int	is_var_already(char *token, t_list *env)
{
	t_list	*tmp;
	int		len;

	tmp = env;
	while (tmp != 0)
	{
		len = 0;
		while (token[len] != '=')
			++len;
		if (ft_strncmp(tmp->content, token, len) == 0)
		{
			free(tmp->content);
			if (!(tmp->content = ft_strdup(token)))
				return (0);
			return (1);
		}
		tmp = tmp->next;
	}
	return (0);
}