#include "minishell.h"

void		ft_free(char **map, int index)
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