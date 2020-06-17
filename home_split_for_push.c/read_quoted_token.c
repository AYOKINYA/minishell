#include "minishell.h"

static void	escape_letter(int *quote, char **line, int *escape_exception)
{
	if (*quote == '\'' && (*line)[1] == '\\')
	{
		++(*line); //backslash로 이동
		*escape_exception = 1;
	}
	else if (*quote == '\"' && ((*line)[1] == '$' || (*line)[1] == '\\' || (*line)[1] == '\"'))
	{
		++(*line); //backslash로 이동
		*escape_exception = 2;
	}
}

static int	quoted_token_len(int *quote, char **line, int *escape_exception)
{
	int		len;

	len = 0;
	while (**line != '\0' && *quote != 0)
	{
		quote_mark(quote, **line);
		if (*quote == 0)
			break ;
		if (*quote == '\"' && **line == '$')
			**line *= -1;
		if (**line == '\\')
			escape_letter(quote, line, escape_exception);
		if (**line != '\0')
			++(*line);
		++len;
	}
	return (len);
}
static char	*ret_str(int escape_exception, char *from, int len)
{
	char	*res;

	if (escape_exception == 0)
	{
		if (!(res = ft_substr(from, 0, len)))
			return (0);
	}
	else if (escape_exception == 1)
	{	
		if (!(res = exception_substr(from, len, "\\")))
			return (0);
	}
	else
	{
		if (!(res = exception_substr(from, len, "$\\\"")))
			return (0);
	}
	return (res);
}

char		*read_quoted_token(int *quote, char **line)
{
	char	*from;
	char	*res;
	int		len;
	int		escape_exception;

	if (**line == '\"' || **line == '\'')
		++(*line); // quote 바로 다음 letter로 넘어간다!
	from = *line;
	escape_exception = 0;
	len = quoted_token_len(quote, line, &escape_exception);
	if (!(res = ret_str(escape_exception, from ,len)))
		return (0);
	return (res);
}