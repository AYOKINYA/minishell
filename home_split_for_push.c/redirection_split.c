#include "minishell.h"

static int	redirection_left_split(char *res, t_list *tokens)
{
	t_list	*token;
	int		i;
	char	*str;
	int		len;

	i = 0;
	len = 0;
	while (res[i] != '\0' && !is_redirection(res[i]))
	{
		++len;
		++i;
	}
	if (len > 0)
	{
		if (!(str = ft_substr(res, 0, len)))
			return (-1);
		if (!(token = ft_lstnew(str)))
		{
			free(str);
			return (-1);
		}
		ft_lstadd_back(&tokens, token);
	}
	return (len);
}

static int	redirection_middle_split(char *res, t_list *tokens)
{
	t_list	*token;
	int		i;
	char	*str;
	int		len;

	len = 0;
	i = 0;
	while (res[i] != '\0' && is_redirection(res[i]))
	{
		++len;
		++i;
	}
	if (len > 0)
	{
		if (!(str = ft_substr(res, 0, len)))
			return (-1);
		if (!(token = ft_lstnew(str)))
		{
			free(str);
			return (-1);
		}
		ft_lstadd_back(&tokens, token);
	}
	return (len);
}

static int	redirection_right_split(char *res, t_list *tokens)
{
	t_list	*token;
	char	*str;
	int		i;
	int		len;

	len = 0;
	i = -1;
	while (res[++i] != '\0')
	{
		if (is_redirection(res[i]))
			*(res + i) *= -1;
		++len;
	}
	if (len > 0)
	{
		if (!(str = ft_substr(res, 0, len)))
			return (-1);
		if (!(token = ft_lstnew(str)))
		{
			free(str);
			return (-1);
		}
		ft_lstadd_back(&tokens, token);
	}
	return (len);
}

int			redirection_split(char *res, t_list *tokens)
{
	char	*start;
	int		len;

	start = res;
	len = 0;
	if ((len = redirection_left_split(res, tokens)) == -1)
		return (0);
	res += len;
	if ((len = redirection_middle_split(res, tokens)) == -1)
		return (0);
	res += len;
	if (*res == '\0')
		return (1);
	if (redirection_right_split(res, tokens) == -1)
		return (0);
	free(start);
	return (1);
}