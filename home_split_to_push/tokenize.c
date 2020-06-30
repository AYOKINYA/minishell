/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkang <jkang@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/30 16:41:47 by jkang             #+#    #+#             */
/*   Updated: 2020/06/30 16:41:47 by jkang            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*tokens_into_args_body(t_list *tokens, t_list *env)
{
	char *arg;

	if (is_env_var(tokens->content))
	{
		if (!(arg = convert_var_to_value(tokens->content, env)))
			return (0);
	}
	else
	{
		if (!(arg = ft_strdup(tokens->content)))
			return (0);
	}
	return (arg);
}

static char	**tokens_into_args(t_list *tokens, t_list *env)
{
	int		row_size;
	char	**args;
	int		i;

	row_size = ft_lstsize(tokens);
	if (!(args = (char **)malloc(sizeof(char *) * row_size)))
		return (0);
	i = 0;
	tokens = tokens->next;
	while (tokens != 0)
	{
		if (!(args[i] = tokens_into_args_body(tokens, env)))
		{
			ft_free(args, i);
			return (0);
		}
		++i;
		tokens = tokens->next;
	}
	args[i] = 0;
	return (args);
}

static int	cmds_into_tokens(char *line, t_list *tokens)
{
	int		quote;
	char	*res;

	quote = 0;
	while (*line != '\0')
	{
		quote_mark(&quote, *line);
		while (*line == ' ' || *line == '\t')
			++line;
		if (quote == 0)
			res = read_unquoted_token(&quote, &line);
		else
			res = read_quoted_token(&quote, &line);
		if (res == 0)
			return (0);
		else
		{
			if (!patch_token(res, tokens))
				return (0);
		}
		if (*line == '\0')
			break ;
		++line;
	}
	return (1);
}

char		**tokenize(char *line, t_list *env)
{
	t_list	*tokens;
	char	**args;

	if (!(tokens = (t_list *)malloc(sizeof(t_list))))
		return (0);
	if (!(tokens->content = ft_strdup("")))
	{
		free(tokens);
		return (0);
	}
	tokens->next = 0;
	if (!cmds_into_tokens(line, tokens))
	{
		ft_lstclear(&tokens, free);
		return (0);
	}
	if (!(args = tokens_into_args(tokens, env)))
	{
		ft_lstclear(&tokens, free);
		return (0);
	}
	ft_lstclear(&tokens, free);
	return (args);
}
