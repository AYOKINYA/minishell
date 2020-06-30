/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   patch_token.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkang <jkang@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/30 15:17:37 by jkang             #+#    #+#             */
/*   Updated: 2020/06/30 15:17:37 by jkang            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void		quote_mark(int *quote, char c)
{
	if (*quote == 0 && (c == '\'' || c == '\"'))
		*quote = c;
	else if (*quote != 0 && *quote == c)
		*quote = 0;
}

static int	is_exception(char c, char *except_str)
{
	int i;

	i = 0;
	while (except_str[i] != '\0')
	{
		if (c == except_str[i])
			return (1);
		++i;
	}
	return (0);
}

char		*exception_substr(char *from, int len, char *except_str)
{
	char	*res;
	int		i;

	i = 0;
	if (!(res = (char *)malloc(sizeof(char) * (len + 1))))
		return (0);
	while (i < len)
	{
		if (is_exception(*from, except_str))
			++from;
		res[i] = *from;
		++from;
		++i;
	}
	res[i] = '\0';
	return (res);
}

static int	redirection_with_chars(char *res)
{
	int len;

	len = ft_strlen(res);
	if (len < 2)
		return (0);
	else if (len == 2 && *res == '>' * -1 && *(res + 1) == '>' * -1)
		return (0);
	while (*res != '\0')
	{
		if (*res == '|' * -1 || *res == '<' * -1 || *res == '>' * -1)
			return (1);
		++res;
	}
	return (0);
}

int			patch_token(char *res, t_list *tokens)
{
	int		mix;
	t_list	*token;

	mix = redirection_with_chars(res);
	if (mix == 1)
	{
		if (!redirection_split(res, tokens))
		{
			free(res);
			return (0);
		}
	}
	else
	{
		if (!(token = ft_lstnew(res)))
		{
			free(res);
			return (0);
		}
		ft_lstadd_back(&tokens, token);
	}
	return (1);
}
