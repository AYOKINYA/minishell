#include "tmp.h"

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>

#define BUFFER_SIZE 100

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

void quote_mark(int *quote, char c)
{
	if (*quote == 0 && (c == '\'' || c == '\"'))
		*quote = c;	
	else if (*quote != 0 && *quote == c)
		*quote = 0;			

}

int is_special(char c)
{
	if (c == ';' || c == '>' || c == '<' || c == '|' || c == '$')
		return (1);
	return (0);
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
	if (!(res = (char *)malloc(sizeof(char) * (len + 1))))
		return (0);
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

int unquoted_token_len(int *quote, char **line, int *escape_exception)
{
	int		len;

	len = 0;
	while (**line != '\0' && **line != ' ')
	{
		quote_mark(quote, **line);
		printf("*quote during unquoted: %d\n", *quote);
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

char	*read_unquoted_token(int *quote, char **line)
{
	char	*from;
	char	*res;
	int		len;
	int		escape_exception;

	from = *line;
	escape_exception = 0;
	len = unquoted_token_len(quote, line, &escape_exception);
	printf("len : %d\n", len);
	if (escape_exception == 0)
		res = ft_substr(from, 0, len);
	else
		res = exception_substr(from, len, "\\");
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

int		ft_num_cmds(char *line)
{
	int num_cmds;
	int quote;

	quote = 0;
	num_cmds = 1;
	while (*line != '\0')
	{
		quote_mark(&quote, *line);
		if (quote == 0 && *line == ';')
			++num_cmds;
		++line;
	}
	return (num_cmds);
}

int		ft_cmd_len(char *line)
{
	int cmd_len;
	int quote;
	
	cmd_len = 0;
	quote = 0;
	while (*line != '\0')
	{
		quote_mark(&quote, *line);
		if (quote == 0 && *line == ';')
			break ;
		++line;
		++cmd_len;
	}
	return (cmd_len);
}

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

int		only_semicolon(char *line)
{
	int semicolon_count;
	int i;
	char *start;

	semicolon_count = 0;
	i = 0;
	start = line;
	while (*line != '\0')
	{
		if (*line == ' ' || *line == ';')
			++i;
		if (*line == ';')
			++semicolon_count;
		++line;
	}
	if (semicolon_count == 1 && start[i] == '\0')
		return (1);
	else if (semicolon_count > 1)
	{
		ft_putendl_fd("zsh: parse error near ';;'", 1);
		return (1);
	}
	return (0);
}

char	**semicolon_split(char *line)
{
	int		i;
	int		num_cmds;
	int		cmd_len;
	char	**cmds;

	num_cmds = ft_num_cmds(line);
	if (!(cmds = (char **)malloc(sizeof(char *) * (num_cmds + 1))))
		return (0);
	i = 0;
	while (i < num_cmds)
	{
		cmd_len = ft_cmd_len(line);
		if (!(cmds[i] = ft_substr(line, 0, cmd_len)))
		{
			ft_free(cmds, i);
			return (0);
		}
		line += cmd_len + 1;
		++i;
	}
	cmds[num_cmds] = 0;
	return (cmds);
}

int redirection_with_chars(char *res)
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

int is_redirection(char c)
{
	if (c == '<' * -1 || c == '>' * -1 || c == '|' * -1 )
		return (1);
	return (0);
}

int redirection_left_split(char *res, t_list *tokens)
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

int redirection_middle_split(char *res, t_list *tokens)
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

int redirection_right_split(char *res, t_list *tokens)
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

int redirection_split(char *res, t_list *tokens)
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
	{
		ft_putendl_fd("zsh: parse error near '\\n'", 1);
		return (0);
	}
	if (redirection_right_split(res, tokens) == -1)
		return (0);
	free(start);
	return (1);
}

int patch_token(char *res, t_list *tokens)
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

int cmds_into_tokens(char *line, t_list *tokens)
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
			break;
		++line;
	}
	return (1);
}

int		is_var(char *content)
{
	while (*content != '\0')
	{
		if (*content == '$' * -1)
			return (1);
		++content;
	}
	return (0);
}

int		get_value_len(char *env_content)
{
	int value_len;

	value_len = 0;
	while (*env_content != '=')
		++env_content;
	++env_content; // = 다음 문자로 넘어가기
	while (*env_content != '\0')
	{	
		printf("**s : %c\n", *env_content);
		++value_len;
		++env_content;
	}
	return (value_len);
}

int		var_len(char **s, t_list *env)
{
	int		i;
	int		value_len;
	char	*copy;

	++(*s); // $ 다음 문자로 넘어간다.
	if (**s == '\0')
		return (0);
	copy = *s;
	i = 0;
	value_len = 0;
	while (**s != '\0' && **s != '$' * -1)
	{
		++i;
		++(*s);
	}
	while (env != 0)
	{
		if (ft_strncmp(env->content, copy, i) == 0 && ((char *)env->content)[i] == '=')
			value_len = get_value_len(env->content);
		env = env->next;
	}
	printf("value_len : %d\n", value_len);
	return (value_len);
}

int		converted_len(char *s, t_list *env)
{
	int len;

	len = 0;
	while (*s != '\0')
	{
		if (*s == '$' * -1 && *(s + 1) != '\0' && *(s + 1) != '$' * -1)
			len += var_len(&s, env);
		else
		{
			if (*s == '$' * -1)
				*s *= -1;
			printf("*s is %c\n", *s);
			++len;
			++s;
		}
	}
	printf("final converted len : %d\n", len);
	return (len);
}

void		get_value(char *env_content, char **res)
{
	while (*env_content != '=')
		++env_content;
	++env_content; // = 다음 문자로 넘어가기
	while (*env_content != '\0')
	{	
		**res = *env_content;
		printf("**res : %c\n", **res);
		++(*res);
		++env_content;
	}
}

int		add_var(char **s, t_list *env, char **res)
{
	int		i;
	char	*copy;

	printf("*s ADD VAR is %c\n", **s);
	++(*s); // $ 다음 문자로 넘어간다.
	if (**s == '\0')
		return (1);
	copy = *s;
	i = 0;
	while (**s != '\0' && **s != '$' * -1 && **s != '$')
	{
		printf("*s ADD VAR is %c\n", **s);
		++i;
		++(*s);
	}
	while (env != 0)
	{
		if (ft_strncmp(env->content, copy, i) == 0 && ((char *)env->content)[i] == '=')
			get_value(env->content, res);
		env = env->next;
	}
	return (1);
}

char	*convert_var_to_value(char *content, t_list *env)
{
	int		len;
	char	*s;
	char	*res;
	char	*ret;
	
	s = content;
	len = converted_len(s, env);
	printf("len is : %d\n", len);
	ret = (char *)malloc(sizeof(char) * (len + 1));
	ret[len] = 0;
	res = ret;
	printf("START %s\n", s);
	while (*s != '\0')
	{
		if (*s == '$' * -1 && *(s + 1) != '\0' && *(s + 1) != '$' * -1)
			add_var(&s, env, &res);
		else
		{
			printf("*s in is %c\n", *s);
			if (*s == '$' * -1)
				*s *= -1;
			*res = *s;
			printf("*res is %c\n", *res);
			++res;
			++s;
		}
	}
	return (ret);
}

char	**tokens_into_args(t_list *tokens, t_list *env)
{
	int		row_size;
	char	**args;
	int		i;

	row_size = ft_lstsize(tokens);
	args = (char **)malloc(sizeof(char *) * row_size);
	i = 0;
	tokens = tokens->next;
	while (tokens != 0)
	{
		if (is_var(tokens->content))
		{
			if (!(args[i] = convert_var_to_value(tokens->content, env)))
			{
				ft_free(args, i);
				return (0);
			}
		}
		else
		{
			if (!(args[i] = ft_strdup(tokens->content)))
			{
				ft_free(args, i);
				return (0);
			}
		}
		++i;
		tokens = tokens->next;
	}
	args[i] = 0;
	ft_lstclear(&tokens, free);
	i = 0;
	while (args[i] != 0)
	{
		printf("args : %s\n", args[i]);
		++i;
	}
	return (args);
}

int tokenize(char *line, t_list *env)
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
	if (!(args = tokens_into_args(tokens, env))) // args 성공적이면 함수 안에서 바로 ft_lstclear한다.
	{
		ft_lstclear(&tokens, free);
		return (0);
	}
	return (1);
}

int exec_cmds(char *line, t_list *env)
{
	int		i;	
	char	**cmds;

	if (only_semicolon(line))
		return (1);
	if (!(cmds = semicolon_split(line)))
		return (0);
	i = 0;
	while (cmds[i] != 0)
	{
		if (!tokenize(cmds[i], env))
		{
			ft_free(cmds, i);
			return (0);
		}
		++i;
	}
	ft_free(cmds, i);
	return (1);
}

t_list *copy_env(char **envp)
{
	int		i;
	t_list	*res;
	t_list	*new;

	if (!(res = ft_lstnew(envp[0])))
		return (0);
	i = 1;
	while(envp[i] != 0)
	{
		if (!(new = (t_list *)malloc(sizeof(t_list))))
			return (0);
		new->content = ft_strdup(envp[i]);
		new->next = 0;
		ft_lstadd_back(&res, new);
		++i;
	}

	return (res);
}

int main(int argc, char **argv, char **envp)
{
	char	*line;
	// int status;
	t_list	*env;

	if (!argc || !argv || !envp)
		return (0);

	if (!(env = copy_env(envp)))
		return (-1);
	line = 0;
	while (1)
	{
		get_next_line(0, &line);
		if (!exec_cmds(line, env))
		{
			free(line);
			break;
		}
		free(line);
	}
	free(env);
	return (0);
}
