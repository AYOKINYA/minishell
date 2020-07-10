/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   make_env_to_envp.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkang <jkang@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/07/10 00:40:57 by yopark            #+#    #+#             */
/*   Updated: 2020/07/10 11:05:23 by jkang            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char			**make_env_to_envp(t_list *env)
{
	size_t		len;
	size_t		i;
	char		**envp;

	len = ft_lstsize(env);
	if (!(envp = (char **)malloc(sizeof(char *) * (len - 1 + 1))))
		return (NULL);
	i = 0;
	while (i < len - 1)
	{
		if (ft_strncmp(env->content, "?=", ft_strlen("?=")) == 0)
		{
			env = env->next;
			continue ;
		}
		envp[i] = ft_strdup(env->content);
		env = env->next;
		i++;
	}
	envp[i] = NULL;
	return (envp);
}

int			path_is_in_envp(char **envp)
{	
	while (*envp != 0)
	{
		if (ft_strncmp(*envp, "PATH=", 5) == 0)
			return (1);
		++envp;
	}
	return (0);
}
