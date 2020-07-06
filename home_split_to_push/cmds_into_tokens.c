/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmds_into_tokens.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkang <jkang@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/07/06 17:43:27 by jkang             #+#    #+#             */
/*   Updated: 2020/07/06 18:17:32 by jkang            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*read_token(int *quote, char **line)
{
	char	*tmp;

	if (*quote == 0)
		tmp = read_unquoted_token(quote, line);
	else
		tmp = read_quoted_token(quote, line);
	if (tmp == 0)
		return (0);
	return (tmp);
}

static char	*join_and_patch(char *res, char *tmp, char *line, t_list *tokens)
{
	if (!(res = ft_strjoin(res, tmp)))
	{
		free(res);
		free(tmp);
		return (0);
	}
	free(tmp);
	if ((*line == '\0' || *line == ' ' || *line == '\t') ||\
			(*(line + 1) == '\0' || *(line + 1) == ' ' || *(line + 1) == '\t'))
	{
		if (!patch_token(res, tokens))
		{
			free(res);
			return (0);
		}
		if (!(res = ft_strdup("")))
			return (0);
	}
	return (res);
}

int	cmds_into_tokens(char *line, t_list *tokens)
{
	int		quote;
	char	*res;
	char	*tmp;
	
	quote = 0;
	if (!(res = ft_strdup("")))
		return (0);
	while (*line != '\0')
	{
		while (*line == ' ' || *line == '\t')
			++line;
		quote_mark(&quote, *line);
		if (!(tmp = read_token(&quote, &line)))
		{
			free(res);
			return (0);
		}
		if (!(res = join_and_patch(res, tmp, line, tokens)))
			return (0);
		if (*line == '\0')
			break;
		++line;
	}
	free(res);
	return (1);
}
