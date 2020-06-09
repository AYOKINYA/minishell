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

void hello_sh(void)
{
	ft_putstr_fd("Hi, it's jkang's minishell.\n", 1);
	ft_putstr_fd("> ", 1);
}

int echo_dollar_question(void)
{
	return (0);
}

int sh_echo(char **tokens)
{
	int		token_count;
	int		i;

	token_count = 0;
	while (tokens[token_count] != 0)
		++token_count;
	i = 1;
	if (token_count == 1)
	{
		write(1, "\n", 1);
		exit (0);
	}
	if (ft_strcmp(tokens[1], "-n") == 0)
		i = 2;
	while (tokens[i] != 0)
	{
		ft_putstr_fd(tokens[i], 1);
		if (i != token_count - 1)
			write(1, " ", 1);	
		++i;
	}
	if (ft_strcmp(tokens[1], "-n") != 0)
		write(1, "\n", 1);
	
	return (0);
}

int sh_exit(void)
{
	exit (0);
}

int sh_env(char **tokens, t_list *env)
{
	int token_count;

	token_count = 0;
	while (tokens[token_count] != 0)
		++token_count;
	if (token_count > 1)
	{
		ft_putstr_fd("env: ", 1);
		ft_putstr_fd(tokens[1], 1);
		ft_putendl_fd(": No such file or directory", 2);
		return (0);
	}
	while (env != 0)
	{
		ft_putstr_fd(env->content, 1);
		write(1, "\n", 1);
		env = env->next;
	}
	return (0);
}

int sh_pwd(void)
{
	char buf[1024];

	if (!getcwd(buf, 1024))
		return (-1);
	ft_putendl_fd(buf, 1);

	return (0);
}

int sh_cd(char **tokens, t_list *env)
{
	int token_count;
	int cd_result;

	token_count = 0;
	while (tokens[token_count] != 0)
		++token_count;
	if (token_count == 1 || (token_count == 2 && ft_strlen(tokens[1]) == 1 && tokens[1][0] == '~'))
	{
		while (env != 0)
		{
			if (ft_strncmp(env->content, "HOME=", 5) == 0)
			{
				ft_putendl_fd(env->content + 5, 1);
				chdir((env->content + 5));
				sh_pwd();
			}
			env = env->next;
		}
	}
	else if (token_count > 2)
		ft_putendl_fd("cd: too many arguments", 1);
	else
	{
		cd_result = chdir(tokens[1]);
		if (cd_result == 0)
			sh_pwd();
		if (cd_result != 0)
		{
			ft_putstr_fd("cd: no such file or directory: ", 1);
			ft_putendl_fd(tokens[1], 1);
		}
	}

	return (0);
}

int change_var_value(t_list *tmp, char *token)
{
	free(tmp->content);
	if (!(tmp->content = ft_strdup(token)))
		return (0);
	return (1);
}

int is_var_already(char *token, t_list *env)
{
	t_list	*tmp;
	int len;

	tmp = env;
	while (tmp != 0)
	{
		len = 0;
		while (token[len] != '=')
			++len;
		if (ft_strncmp(tmp->content, token, len) == 0)
		{
			change_var_value(tmp, token);
			return (1);
		}
		tmp = tmp->next;
	}
	return (0);
}

int sh_export(char **tokens, t_list *env)
{
	int		i;
	int		token_count;
	t_list	*new;


	token_count = 0;
	while (tokens[token_count] != 0)
		++token_count;
	if (token_count == 1)
		sh_env(tokens, env);
	else
	{
		i = 1;
		while (i < token_count)
		{
			if (ft_strchr(tokens[i], '=') != 0)
			{
				if (ft_strlen(tokens[i]) == 1)
					ft_putendl_fd("zsh : bad assignment", 1);
				else
				{
					if (!is_var_already(tokens[i], env))
					{
						if (!(new = (t_list *)malloc(sizeof(t_list))))
							return (-1);
						if (!(new->content = ft_strdup(tokens[i])))
							return (-1);
						new->next = 0;
						ft_lstadd_back(&env, new);
					}
				}
			}
			++i;
		}
	}
	return (0);
}

int is_alpha_num(char *s)
{
	while (*s != '\0')
	{
		if (!ft_isalnum(*s))
			return (0);
		++s;
	}
	return (1);
}

int is_var(char *token, t_list *env)
{
	int		i;
	t_list	*tmp;
	char	*var;

	tmp = env;
	while (tmp != 0)
	{
		i = 0;
		var = (char *)(tmp->content);
		while (var[i] != '=')
		{
			if (var[i] != token[i])
				break;
			++i;
		}
		tmp = tmp->next;
		if (var[i] == '=')
			return (1);
	}
	return (0);
}

int unset_var(char *token, t_list *env)
{
	t_list *tmp;

	while (env != 0)
	{
		if (ft_strncmp((env->next)->content, token, ft_strlen(token)) == 0)
		{
			tmp = (env->next)->next;
			free((env->next)->content);
			free(env->next);
			env->next = tmp;
			return (1);
		}
		env = env->next;
	}
	return (0);
}

int sh_unset(char **tokens, t_list *env)
{
	int i;
	int token_count;
	
	token_count = 0;
	while (tokens[token_count] != 0)
		++token_count;
	if (token_count == 1)
		ft_putendl_fd("unset: not enough arguments", 1);
	else
	{
		i = 1;
		while (i < token_count)
		{
			if (!is_alpha_num(tokens[i]))
			{
				ft_putstr_fd("unset: ", 1);
				ft_putstr_fd(tokens[i], 1);
				ft_putendl_fd(": invalid parameter name", 1);
			}
			else if (is_var(tokens[i], env))
				unset_var(tokens[i], env);
			++i;
		}
	}
	return (0);
}

int cmd_not_exists(char **tokens)
{
	ft_putstr_fd("zsh: command not found: ", 1);
	ft_putendl_fd(tokens[0], 1);
	return (0);
}

int check_fd_aggregation(char **args)
{
	int count;
	int i;
	int j;

	count = 0;
	i = 0;
	while (args[i] != 0)
	{
		j = 0;
		while (args[i][j] != '\0')
		{
			if (args[i][j] == '<' * -1 || args[i][j] == '>' * -1)
				++count;
			if (args[i][j] == '>' * -1 && args[i][j + 1] == '>' * -1)
				count -= 1;	
			++j;
		}
		++i;
	}
	if (count > 1)
	{
		ft_putendl_fd("fd aggregation is not supported.", 2);
		return (0);
	}
	return (1);
}

int exec_args(char **tokens, t_list *env)
{
	int		status;
	pid_t	pid;
	pid_t	wait_pid;
	
	if (tokens[0] == 0)
		return (1);
	if (!check_fd_aggregation(tokens))
		return (0);
	if (ft_strcmp(tokens[0], "exit") == 0)
		sh_exit();
	else if (ft_strcmp(tokens[0], "cd") == 0)
		sh_cd(tokens, env);
	else if (ft_strcmp(tokens[0], "export") == 0)
		sh_export(tokens, env);
	else if (ft_strcmp(tokens[0], "unset") == 0)
		sh_unset(tokens, env);
	else
	{
		pid = fork();
		if (pid == 0)
		{
			printf("it is child process.\n");
			printf("====================\n");
			
			//malloc guard
			if (ft_strcmp(tokens[0], "echo") == 0)
				sh_echo(tokens);
			else if (ft_strcmp(tokens[0], "pwd") == 0)
				sh_pwd();
			else if (ft_strcmp(tokens[0], "env") == 0)
				sh_env(tokens, env);
			else
				cmd_not_exists(tokens); //나중에 execve로 검색하는 부분 추가
			sleep(1);
			exit(0);
		}
		else if (pid > 0)
		{
			wait_pid = waitpid(pid, &status, 0);
			if (wait_pid == -1)
				ft_putstr_fd("wait_pid returns error. no child process", 1);
			else
			{
				printf("====================\n");
				ft_putstr_fd("child process is done.\n", 1);
			}
			printf("parent process ends\n");
			return (0);
		}
		else
		{
			ft_putstr_fd("Fork is failed.", 1);
			return (-1);
		}
	}
	return (0);
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

int unquoted_token_len(int *quote, char **line, int *escape_exception)
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

char	*read_unquoted_token(int *quote, char **line)
{
	char	*from;
	char	*res;
	int		len;
	int		escape_exception;

	from = *line;
	escape_exception = 0;
	len = unquoted_token_len(quote, line, &escape_exception);
	if (escape_exception == 0)
		res = ft_substr(from, 0, len);
	else
		res = exception_substr(from, len, "\\");
	return (res);
}

void escape_letter(int *quote, char **line, int *escape_exception)
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

int quoted_token_len(int *quote, char **line, int *escape_exception)
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

char *read_quoted_token(int *quote, char **line)
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
	if (escape_exception == 0)
		res = ft_substr(from, 0, len);
	else if (escape_exception == 1)
		res = exception_substr(from, len, "\\");
	else
		res = exception_substr(from, len, "$\\\"");
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
		return (1);
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

int		is_env_var(char *content)
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
			++len;
			++s;
		}
	}
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
		++(*res);
		++env_content;
	}
}

int		add_var(char **s, t_list *env, char **res)
{
	int		i;
	char	*copy;

	++(*s); // $ 다음 문자로 넘어간다.
	if (**s == '\0')
		return (1);
	copy = *s;
	i = 0;
	while (**s != '\0' && **s != '$' * -1)
	{
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
	if (!(ret = (char *)malloc(sizeof(char) * (len + 1))))
		return (0);
	ret[len] = 0;
	res = ret;
	while (*s != '\0')
	{
		if (*s == '$' * -1 && *(s + 1) != '\0' && *(s + 1) != '$' * -1 && *(s + 1) != '?')
			add_var(&s, env, &res);
		else
		{
			if (*s == '$' * -1)
				*s *= -1;
			*res = *s;
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
		if (is_env_var(tokens->content))
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

int has_redirection(char *arg)
{
	while (*arg != '\0')
	{
		if (is_redirection(*arg))
			return (1);
		++arg;
	}
	return (0);
}

int investigate_redirection(char **args, int i)
{
	if (args[i + 1] == 0 || ft_strlen(args[i]) > 2) //redirection 다음 arg 없을 때!
	{
		ft_putendl_fd("syntax error1!", 2);	
		return (0);
	}
	else if (ft_strlen(args[i]) == 2)
	{
		if (args[i][0] != '>' * -1 || args[i][1] != '>' * -1)
		{
			ft_putendl_fd("syntax error2!", 2);	
			return (0);
		}
	}
	return (1);
}

int validate_redirection(char **args)
{
	int i;

	i = 0;
	while (args[i] != 0)
	{
		if (has_redirection(args[i]))
		{
			if (!investigate_redirection(args, i))
				return (0);
		}
		++i;
	}
	return (1);
}

int		count_cmds(char **args)
{
	int count;

	count = 1;
	while (*args != 0)
	{
		if (**args == '|' * -1)
			++count;
		++args;
	}
	return (count);
}

int		count_args(char **args)
{
	int count;

	count = 0;
	while (*args != 0)
	{
		if (**args == '|' * -1 || **args == '\0')
			break ;
		++count;
		++args;
	}
	return (count);
}

char	***pipe_split(char **args)
{
	int		cmds_count;
	int		args_count;	
	int		i;
	int		j;
	char	***res;

	cmds_count = count_cmds(args);
	if (!(res = (char ***)malloc(sizeof(char **) * (cmds_count + 1))))
		return (0);
	res[cmds_count] = 0;
	i = 0;
	while (i < cmds_count)
	{
		args_count = count_args(args);
		if (!(res[i] = (char **)malloc(sizeof(char *) *(args_count + 1))))
		{
			while (i--)
				free(res[i]);
			free(res);
			return (0);
		}
		res[i][args_count] = 0;
		j = 0;
		while (j < args_count)
		{
			if (!(res[i][j] = ft_strdup(*args)))
			{
				while (j--)
					free(res[i][j]);
				while (i--)
					free(res[i]);
				free(res);
				return (0);
			}
			++args;
			++j;
		}
		++args;
		++i;
	}
	return (res);
}

char	**tokenize(char *line, t_list *env)
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
	if (!(args = tokens_into_args(tokens, env))) // args 성공적이면 함수 안에서 바로 linked list인 tokens를 ft_lstclear한다.
	{
		ft_lstclear(&tokens, free);
		return (0);
	}
	return (args);
}

int exec_cmds_without_fork(char **tokens, t_list *env)
{
	if (ft_strcmp(tokens[0], "exit") == 0)
		sh_exit();
	else if (ft_strcmp(tokens[0], "cd") == 0)
		sh_cd(tokens, env);
	else if (ft_strcmp(tokens[0], "export") == 0)
		sh_export(tokens, env);
	else if (ft_strcmp(tokens[0], "unset") == 0)
		sh_unset(tokens, env);
	else
		return (0);
	return (1);
}

int exec_cmds_with_fork(char **tokens, t_list *env)
{
	if (ft_strcmp(tokens[0], "echo") == 0)
		sh_echo(tokens);
	else if (ft_strcmp(tokens[0], "pwd") == 0)
		sh_pwd();
	else if (ft_strcmp(tokens[0], "env") == 0)
		sh_env(tokens, env);
	else
		cmd_not_exists(tokens);
	return (1);
}

int check_redirection(char **cmd, int *file_fd)
{
	int redirection;

	redirection = 0;
	while (*cmd != 0)
	{
		if (**cmd == '>' * -1 && *(*cmd + 1) != '>' * -1)
			redirection = 1;
		else if (**cmd == '>' * -1 &&  *(*cmd + 1) == '>' * -1)
			redirection = 2;
		else if (**cmd == '<' * -1)
			redirection = 3;
		if (redirection > 0)
			break ;
		++cmd;
	}
	if (redirection == 1)
		*file_fd = open(*(cmd + 1), O_RDWR | O_CREAT | O_TRUNC | S_IROTH, 0644);
	else if (redirection == 2)
		*file_fd = open(*(cmd + 1), O_RDWR | O_APPEND | S_IROTH, 0644);
	else if (redirection == 3)
		*file_fd = open(*(cmd + 1) , O_RDONLY);
	else
		*file_fd = 1;
	return (redirection);
}

char **get_cmd(char **cmd, int redirection)
{
	int count;
	int i;
	char **res;

	count = 0;
	while (cmd[count] != 0)
		++count;
	if (redirection)
		count -= 2;
	if (!(res = (char **)malloc(sizeof(char *) * (count + 1))))
		return (0);
	res[count] = 0;
	i = 0;
	while (i < count)
	{
		if (is_redirection(**cmd))
			cmd += 2;
		if (!(res[i] = ft_strdup(*cmd)))
		{
			ft_free(res, i);
			return (0);
		}
		++i;
		++cmd;
	}
	return (res);
}

char ***get_cmds_with_pipe_split(char *line, t_list *env)
{
	char	**args;
	char	***cmds;
	int		i;
	
	if (!(args = tokenize(line, env)))
		return (0);
	if (!(validate_redirection(args)))
	{
		free_2d_array(args);
		return (0);
	}
	if (!(cmds = pipe_split(args)))
	{
		i = 0;
		while (cmds[i] != 0)
		{
			free_2d_array(cmds[i]);
			++i;
		}
		free_2d_array(args);
	}
	return (cmds);
}

int		process_pipe_and_redirection(char ***cmds, int *pipe_fd, int *file_fd, int *redirection)
{
	int		fd;

	fd = 0;
	dup2(fd, 0);
	if (*(cmds + 1) != 0)
		dup2(pipe_fd[1], 1);
	close(pipe_fd[0]);
	*redirection = check_redirection(*cmds, file_fd);
	if (*file_fd < 0)
	{
		ft_putendl_fd("file fd error!", 2);
		return (0);
	}
	else if (*redirection == 1 || *redirection == 2)
		dup2(*file_fd, 1);
	else if (*redirection == 3)
		dup2(*file_fd, 0);
	return (1);
}

void	parent_process(pid_t pid, int *pipe_fd, int *file_fd)
{
	int		status;
	pid_t	wait_pid;

	wait_pid = waitpid(pid, &status, 0);
	if (wait_pid == -1)
		ft_putstr_fd("wait_pid returns error. no child process", 1);
	else
	{
		printf("====================\n");
		ft_putstr_fd("child process is done.\n", 1);
	}
	printf("parent process ends\n");
	close(pipe_fd[1]);
	*file_fd = pipe_fd[0];
}

int process_cmd(char *line, t_list *env)
{
	char	**cmd;
	char	***cmds;
	int		file_fd;
	int		redirection;
	int		pipe_fd[2];
	pid_t	pid;

	cmds = get_cmds_with_pipe_split(line, env);
	while (*cmds)
	{
		if ((*cmds)[0] == 0)
			return (1);
		if (!check_fd_aggregation(*cmds))
			return (0);
		if (!exec_cmds_without_fork(*cmds, env))
		{
			pipe(pipe_fd);
			pid = fork();
			if (pid == -1)
			{
				ft_putstr_fd("Fork is failed.", 1);
				return (0);
			}
			else if (pid == 0)
			{
				printf("it is child process.\n");
				printf("====================\n");
				file_fd = 0;
				if (!process_pipe_and_redirection(cmds, pipe_fd, &file_fd, &redirection))
					exit(0);
				cmd = get_cmd(*cmds, redirection);
				exec_cmds_with_fork(cmd, env);
				free_2d_array(cmd);
				sleep(1);
				close(file_fd);
				exit(0);
			}
			else
				parent_process(pid, pipe_fd, &file_fd);
		}
		++cmds;
	}
	free_3d_array(cmds);
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
		if (!process_cmd(cmds[i], env))
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
	while (envp[i] != 0)
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
	int status;
	t_list	*env;

	if (!argc || !argv || !envp)
		return (0);
	if (!(env = copy_env(envp)))
		return (-1);
	line = 0;
	ft_putstr_fd("> ", 1);
	status = 1;
	while (status != 0)
	{
		status = get_next_line(0, &line);
		printf("status : %d\n", status);
		if (!exec_cmds(line, env))
		{
			free(line);
			break;
		}
		free(line);
		ft_putstr_fd("> ", 1);
	}
	free(env);
	return (0);
}
