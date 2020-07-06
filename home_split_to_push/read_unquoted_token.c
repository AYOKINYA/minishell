/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_unquoted_token.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkang <jkang@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/30 15:18:53 by jkang             #+#    #+#             */
/*   Updated: 2020/07/06 15:57:33 by jkang            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_special(char c)
{
	if (c == ';' || c == '>' || c == '<' || c == '|' || c == '$')
		return (1);
	return (0);
}

static int	unquoted_token_len(int *quote, char **line, int *escape_exception)
{
	int		len;

	len = 0;
	while (**line != '\0' && **line != ' ')
	{
		quote_mark(quote, **line);
		if (*quote != 0)
			break ;
		if (is_special(**line))
			**line *= -1;
		if (**line == '\\')
		{
			++(*line);
			if (is_special(**line))
				*escape_exception = 1;
		}
		if (**line != '\0')
			++(*line);
		++len;
	}
	return (len);
}

char		*read_unquoted_token(int *quote, char **line)
{
	char	*from;
	char	*res;
	int		len;
	int		escape_exception;

	from = *line;
	escape_exception = 0;
	len = unquoted_token_len(quote, line, &escape_exception);
	if (escape_exception == 0)
	{
		if (!(res = ft_substr(from, 0, len)))
			return (0);
	}
	else
	{
		if (!(res = exception_substr(from, len, "\\")))
			return (0);
	}
	return (res);
}
