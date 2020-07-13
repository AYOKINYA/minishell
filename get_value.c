/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_value.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkang <jkang@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/30 15:10:54 by jkang             #+#    #+#             */
/*   Updated: 2020/06/30 15:10:55 by jkang            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int		get_value_len(char *env_content)
{
	int value_len;

	value_len = 0;
	while (*env_content != '=')
		++env_content;
	++env_content;
	while (*env_content != '\0')
	{
		++value_len;
		++env_content;
	}
	return (value_len);
}

void	get_value(char *env_content, char **res)
{
	while (*env_content != '=')
		++env_content;
	++env_content;
	while (*env_content != '\0')
	{
		**res = *env_content;
		++(*res);
		++env_content;
	}
}
