#include "tmp.h"

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>

#define BUFFER_SIZE 100

// void quote_mark(int *quote, char buf)
// {
// 	if (*quote == 0 && (buf == '\'' || buf == '\"'))
// 		*quote = buf;	
// 	else if (*quote != 0 && (buf == '\'' || buf == '\"'))
// 	{
// 		if (*quote == buf)
// 			*quote = 0;			
// 	}
// }

// void escape_letter(char **line, int quote)
// {
// 	char buf[3];
	
// 	ft_bzero(buf, 3);
// 	buf[0] = '\\';
// 	read(0, buf + 1, 1);
// 	if (quote == '\'')
// 	{
// 		if (buf[1] == '\\')
// 			*line = ft_strjoin(*line, buf + 1);
// 		else
// 			*line = ft_strjoin(*line, buf);
// 	}
// 	else if (quote == '\"')
// 	{
// 		if (buf[1] == '$' || buf[1] =='\\' || buf[1] == '\"')
// 			*line = ft_strjoin(*line, buf + 1);
// 		else
// 			*line = ft_strjoin(*line, buf);
// 	}
// 	else
// 		*line = ft_strjoin(*line, buf + 1);
// }

// int get_line(char **line)
// {
// 	char	buf[2];
// 	int		quote;

// 	ft_bzero(buf, 2);
// 	quote = 0;
// 	*line = ft_strdup("");
// 	while (read(0, buf, 1) && *buf != '\n')
// 	{
// 		if (*buf == '\'' || *buf == '\"')
// 			quote_mark(&quote, *buf);
// 		if (*buf == '\\')
// 			escape_letter(line, quote);
// 		else
// 			*line = ft_strjoin(*line, buf);
// 	}
// 	if (*buf == '\0' || *buf == '\n')
// 		return (0);
// 	return (1);
// }



int				ft_free_and_ret(char **s_res, int ret)
{
	if (*s_res)
	{
		free(*s_res);
		*s_res = 0;
	}
	return (ret);
}

int				ft_has_new_line(char *buf, char **s_res, char **line)
{
	int		i;
	char	tmp[BUFFER_SIZE + 1];

	if (ft_strchr(buf, '\n') != 0)
	{
		i = 0;
		while (buf[i] != '\n')
		{
			tmp[i] = buf[i];
			++i;
		}
		tmp[i] = '\0';
		if (!(*line = ft_strjoin(*line, tmp)))
			return (ft_free_and_ret(s_res, -1));
		//free(*s_res);
		//*s_res = 0;
		if (!(*s_res = ft_strdup(&buf[i + 1])))
			return (ft_free_and_ret(s_res, -1));
		return (1);
	}
	return (0);
}

int				ft_read_s_res(char **s_res, char **line)
{
	int		i;
	char	tmp[BUFFER_SIZE + 1];

	if ((ft_strchr(*s_res, '\n')) == 0)
	{
		if (!(*line = ft_strjoin(*line, *s_res)))
			return (ft_free_and_ret(s_res, -1));
		free(*s_res);
		*s_res = 0;
	}
	else
	{
		i = 0;
		while ((*s_res)[i] != '\n' && (*s_res)[i] != '\0')
		{
			tmp[i] = (*s_res)[i];
			++i;
		}
		tmp[i] = '\0';
		if (!(*line = ft_strjoin(*line, tmp)))
			return (ft_free_and_ret(s_res, -1));
		ft_memmove(*s_res, *s_res + i + 1, ft_strlen(*s_res) - i);
		return (1);
	}
	return (0);
}

int				get_next_line(int fd, char **line)
{
	char			buf[BUFFER_SIZE + 1];
	static char		*s_res;
	size_t			ret;

	if (fd < 0 || line == 0 || (read(fd, buf, 0) < 0) || BUFFER_SIZE < 1)
		return (-1);
	if (!(*line = ft_strdup("")))
		return (ft_free_and_ret(&s_res, -1));
	if (s_res)
	{
		if (ft_read_s_res(&s_res, line) == 1)
			return (1);
	}
	while ((ret = read(fd, buf, BUFFER_SIZE)) > 0)
	{
		buf[ret] = '\0';
		if (ft_has_new_line(buf, &s_res, line) == 1)
			return (1);
		if (!(*line = ft_strjoin(*line, buf)))
			return (ft_free_and_ret(&s_res, -1));
	}
	return (ft_free_and_ret(&s_res, ret));
}

void quote_mark(int *quote, char buf)
{
	if (*quote == 0 && (buf == '\'' || buf == '\"'))
		*quote = buf;	
	else if (*quote != 0 && *quote == buf)
		*quote = 0;			

}


int is_delim(char c)
{
	if (c == ' ' || c == '>' || c == '<' || c == '|')
		return (1);
	return (0);
}


int unquoted_token_len(int *quote, char **line)
{
	int		len;

	len = 0;
	while (**line != '\0' && !is_delim(**line))
	{
		quote_mark(quote, **line);
		printf("*quote during unquoted: %d\n", *quote);
		if (*quote != 0)
			break ;
		if (**line == '\\')
			++(*line);
		if (**line != '\0')
			++(*line);
		++len;
	}
	return (len);
}

char	*read_unquoted_token(int *quote, char **line)
{
	char	*from;
	char	*res;
	int		len;

	from = *line;
	len = unquoted_token_len(quote, line);
	printf("len : %d\n", len);
	res = ft_substr(from, 0, len);
	printf("res : %s\n", res);
	return (res);
}

void escape_letter(int *quote, char **line, int *escape_exception)
{
	printf("escape!\n");
	printf("escape starting line %s\n", *line);
	printf("escape quote %d\n", *quote);
	printf("escape next char %c\n", (*line)[1]);
	printf("t or f : %d\n", (*quote == '\"'));
	printf("t or f : %d\n", ((*line)[1] == '\\'));
	if (*quote == '\'' && (*line)[1] == '\\')
	{
		printf("escape 1 \n");
		++(*line); //backslash로 이동
		*escape_exception = 1;
	}
	else if (*quote == '\"' && ((*line)[1] == '$' || (*line)[1] == '\\' || (*line)[1] == '\"'))
	{
		printf("escape 2\n");
		++(*line); //backslash로 이동
		*escape_exception = 2;
	}
}

int quoted_token_len(int *quote, char **line, int *escape_exception)
{
	int		len;

	len = 0;
	while (**line != '\0' && *quote != 0)
	{
		quote_mark(quote, **line);
		printf("**line during len : %d\n", **line);
		printf("*quote during len : %d\n", *quote);
		if (*quote == 0)
			break ;
		if (**line == '\\')
			escape_letter(quote, line, escape_exception);
		if (**line != '\0')
			++(*line);
		++len;
	}
	return (len);
}

int		is_exception(char c, char *except_str)
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

char	*exception_substr(char *from, int len, char *except_str)
{
	char	*res;
	int		i;

	i = 0;
	res = (char *)malloc(sizeof(char) * (len + 1));
	printf("from is : %s\n", from);
	while (i < len)
	{
		printf("*from with exception : %c\n", *from);
		if (is_exception(*from, except_str))
			++from;
		res[i] = *from;
		++from;
		++i;
	}
	res[i] = '\0';
	return (res);
}

char *read_quoted_token(int *quote, char **line)
{
	char	*from;
	char	*res;
	int		len;
	int		escape_exception;

	if (**line == '\"' || **line == '\'')
		++(*line); // quote 바로 다음 letter로 넘어간다!
	printf("**line : %c\n", **line);
	printf("*quote : %d\n", *quote);
	
	from = *line;
	escape_exception = 0;
	len = quoted_token_len(quote, line, &escape_exception);
	printf("len %d\n", len);
	printf("flag %d\n", escape_exception);
	if (escape_exception == 0)
		res = ft_substr(from, 0, len);
	else if (escape_exception == 1)
		res = exception_substr(from, len, "\\");
	else
		res = exception_substr(from, len, "$\\\"");
	printf("res : %s\n", res);
	return (res);
}

int tokenize(char *line)
{
	int		quote;
	//t_list	*tokens;

	quote = 0;
	// tokens = (t_list *)malloc(sizeof(t_list));
	
	while (*line != '\0')
	{
		quote_mark(&quote, *line);
		printf("check *line : %c\n", *line);
		while (*line == ' ' || *line == '\t')
			++line;
		printf("quote is : %d\n", quote);
		if (quote == 0)
			read_unquoted_token(&quote, &line);
		else
			read_quoted_token(&quote, &line);
		if (*line == '\0')
			break;
		++line;
	}
	return (1);
}


int main(void)//int argc, char **argv, char **envp)
{
	char	*line;
	// int status;

	line = 0;
	while (1)
	{
		get_next_line(0, &line);
		tokenize(line);
		//printf("%s\n", line);
		free(line);
	}
	return (0);
}
