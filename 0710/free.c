/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkang <jkang@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/30 15:10:16 by jkang             #+#    #+#             */
/*   Updated: 2020/06/30 16:32:35 by jkang            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_free(char **map, int index)
{
	int i;

	i = index;
	while (i > 0)
	{
		--i;
		free(map[i]);
	}
	free(map);
}

void	free_2d_array(char **arr)
{
	int count;

	count = 0;
	while (arr[count] != 0)
		++count;
	ft_free(arr, count);
}

void	free_3d_array(char ***arr)
{
	int count;

	count = 0;
	while (arr[count] != 0)
		++count;
	while (count--)
	{
		free_2d_array(arr[count]);
	}
	free(arr);
}
