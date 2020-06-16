#include "tmp.h"

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>
#include <signal.h>

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
		free(*s_res);
		*s_res = 0;
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

int count_tokens(char **tokens)
{
	int token_count;

	token_count = 0;
	while (tokens[token_count] != 0)
		++token_count;
	return (token_count);
}

int sh_echo(char **tokens, int *p_status)
{
	int		token_count;
	int		i;

	token_count = count_tokens(tokens);
	i = 1;
	if (token_count == 1)
	{
		write(1, "\n", 1);
		*p_status = 0;
		return (1);
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
	*p_status = 0;
	return (1);
}

int is_numeric(char *s)
{
	while (*s != 0)
	{
		if (*s < '0' || *s > '9')
			return (0);
		++s;
	}
	return (1);
}

int sh_exit(char **tokens, int *p_status)
{
	int token_count;

	token_count = count_tokens(tokens);
	ft_putendl_fd("exit", 1);
	if (token_count == 2 && is_numeric(tokens[1]))
		exit(ft_atoi(tokens[1]));
	else if (token_count > 2 && is_numeric(tokens[1]))
	{
		ft_putendl_fd("bash: exit: too many arguments", 2);
		*p_status = 1;
	}
	else
		exit(2);
	return (1);
}

int sh_env(char **tokens, t_list *env, int *p_status)
{
	int token_count;

	token_count = count_tokens(tokens);
	if (token_count > 1)
	{
		ft_putstr_fd("env: '", 2);
		ft_putstr_fd(tokens[1], 2);
		ft_putstr_fd("'", 2);
		ft_putendl_fd(": No such file or directory", 2);
		*p_status = 1;
		return (0);
	}
	env = env->next;
	while (env != 0)
	{
		ft_putstr_fd(env->content, 1);
		write(1, "\n", 1);
		env = env->next;
	}
	*p_status = 0;
	return (0);
}

int sh_pwd(int *p_status)
{
	char buf[1024];

	if (!getcwd(buf, 1024))
	{
		*p_status = 1;
		return (-1);
	}
	ft_putendl_fd(buf, 1);
	*p_status = 0;
	return (0);
}

int sh_ch_homedir(t_list *env, int *p_status)
{
	while (env != 0)
	{
		if (ft_strncmp(env->content, "HOME=", 5) == 0)
		{
			if (chdir(env->content + 5) == -1)
			{
				ft_putstr_fd("bash: cd: ", 2);
				ft_putstr_fd(env->content + 5, 2);
				ft_putendl_fd(": no such file or directory", 2);
				*p_status = 1;
				return (0);
			}
		}
		env = env->next;
	}
	return (1);
}

int sh_chdir(char **tokens, int *p_status)
{
	if (chdir(tokens[1]) == -1)
	{
		ft_putstr_fd("bash: cd: ", 2);
		ft_putstr_fd(tokens[1], 2);
		ft_putendl_fd(": no such file or directory", 2);
		*p_status = 1;
		return (0);
	}
	return (1);
}

int sh_cd(char **tokens, t_list *env, int *p_status)
{
	int token_count;

	token_count = count_tokens(tokens);
	if (token_count == 1 || (token_count == 2 && ft_strlen(tokens[1]) == 1 && tokens[1][0] == '~'))
	{
		if (!sh_ch_homedir(env, p_status))
			return (0);
	}
	else if (token_count > 2)
	{
		ft_putendl_fd("bash: cd: too many arguments", 2);
		*p_status = 1;
	}
	else
	{
		if (!(sh_chdir(tokens, p_status)))
			return (0);
	}
	*p_status = 0;
	return (1);
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

int sh_export_body(char **tokens, t_list *env, int i)
{
	t_list	*new;

	if (ft_strlen(tokens[i]) == 1)
	{
		ft_putendl_fd("bash: export: `=': not a valid identifier", 2);
		return (0);
	}
	else
	{
		if (!is_var_already(tokens[i], env))
		{
			if (!(new = (t_list *)malloc(sizeof(t_list))))
				return (0);
			if (!(new->content = ft_strdup(tokens[i])))
			{
				free(new);
				return (0);
			}
			new->next = 0;
			ft_lstadd_back(&env, new);
		}
	}
	return (1);
}

int sh_export(char **tokens, t_list *env, int *p_status)
{
	int		i;
	int		token_count;
	
	token_count = count_tokens(tokens);
	if (token_count == 1)
		sh_env(tokens, env, p_status);
	else
	{
		i = 0;
		while (++i < token_count)
		{
			if (ft_strchr(tokens[i], '=') != 0)
			{
				if (!sh_export_body(tokens, env, i))
				{
					*p_status = 1;
					return (0);
				}
			}
		}
	}
	*p_status = 0;
	return (1);
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

int sh_unset_body(char **tokens, int token_count, t_list *env)
{
	int i;

	i = 1;
	while (i < token_count)
	{
		if (!is_alpha_num(tokens[i]))
		{
			ft_putstr_fd("unset: ", 2);
			ft_putstr_fd(tokens[i], 2);
			ft_putendl_fd(": invalid parameter name", 2);
			return (0);
		}
		else if (is_var(tokens[i], env))
			unset_var(tokens[i], env);
		++i;
	}
	return (1);
}

int sh_unset(char **tokens, t_list *env, int *p_status)
{
	int token_count;
	
	token_count = 0;
	while (tokens[token_count] != 0)
		++token_count;
	if (token_count == 1)
	{
		ft_putendl_fd("unset: not enough arguments", 2);
		*p_status = 1;
		return (0);
	}
	else
	{
		if (!(sh_unset_body(tokens, token_count, env)))
		{
			*p_status = 1;
			return (0);
		}
	}
	*p_status = 0;
	return (1);
}

int count_path_len(char *env_path)
{
	int count;

	count = 0;
	while (env_path[count] != '\0' && env_path[count] != ':')
		++count;
	return (count);

}

char *get_env_path(char **envp)
{
	char *env_path;
	
	while (*envp != 0)
	{
		if (ft_strncmp(*envp, "PATH=", 5) == 0)
		{
			*envp += 5;
			if (!(env_path = ft_strdup(*envp)))
				return (0);
			break ;
		}
		++envp;
	}
	return (env_path);
}

char	*get_path(char *env_path, int path_len)
{
	char	*tmp;
	char	*path;
	
	if (*env_path == '/')
	{
		if (!(path = ft_substr(env_path, 0, path_len)))
			return (0);
	}
	else
	{
		if (!(tmp = ft_strdup("/")))
			return (0);
		if (!(path = ft_substr(env_path, 0, path_len)))
			return (0);
		if (!(path = ft_strjoin(tmp, path)))
			return (0);
	}

	return (path);
}

int exec_with_path(char **tokens, char **envp, char *env_path, int path_len)
{
	char	*path;
	int		ret;
	
	if (!(path = get_path(env_path, path_len)))
		return (0);
	if (!(path = ft_strjoin(path, "/")))
		return (0);
	if (!(path = ft_strjoin(path, tokens[0])))
		return (0);
	ret = execve(path, tokens, envp);
	free(path);
	if (ret >= 0)
		return (2);
	return (1);
}

int search_with_path(char **tokens, char **envp)
{
	char	*env_path;
	char	*origin;
	int		path_len;
	int		ret;

	if (!(env_path = get_env_path(envp)))
		return (0);
	origin = env_path;
	while (*env_path != 0)
	{
		path_len = count_path_len(env_path);
		if (!(ret = exec_with_path(tokens, envp, env_path, path_len)))
		{
			free(origin);
			return (0);
		}
		env_path += path_len + 1;
		if (*env_path != '\0')
			++env_path;
	}
	free(origin);
	return (ret);
}

int cmd_not_builtins(char **tokens, char **envp)
{
	int ret;

	if (ft_strncmp(tokens[0], "/", 1) == 0 || ft_strncmp(tokens[0], "/", 1) == 0)
	{
		if ((ret = execve(tokens[0], tokens, envp)) == -1)
		{
			ft_putstr_fd("zsh: command not found: ", 2);
			ft_putendl_fd(tokens[0], 2);
			exit(1);
		}
	}
	else
	{
		if (!(ret = search_with_path(tokens, envp)))
			return (0);
		else if (ret == 1)
		{
			ft_putstr_fd("zsh: command not found: ", 2);
			ft_putendl_fd(tokens[0], 2);
			exit(1);
		}
	}
	return (1);
}

int check_fd_aggregation(char **args)
{
	int count;
	int i;
	int j;

	count = 0;
	i = -1;
	while (args[++i] != 0)
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
	}
	if (count > 1)
	{
		ft_putendl_fd("fd aggregation is not supported.", 2);
		return (0);
	}
	return (1);
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
char	*ret_str(int escape_exception, char *from, int len)
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

char	*read_quoted_token(int *quote, char **line)
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
	else if (semicolon_count > 1 && start[i] == '\0')
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
		if (*s == '$' * -1 && *(s + 1) != '\0' && *(s + 1) != '$' * -1)// && *(s + 1) != '?')
			add_var(&s, env, &res);
		else
		{
			if (*s == '$' * -1)
				*s *= -1;
			*(res++) = *(s++);
		}
	}
	return (ret);
}

char	*tokens_into_args_body(t_list *tokens, t_list *env)
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

char	**tokens_into_args(t_list *tokens, t_list *env)
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
			// ft_free(args, i);
			free_2d_array(args);
			return (0);
		}
		++i;
		tokens = tokens->next;
	}
	args[i] = 0;
	// i = 0;
	// while (args[i] != 0)
	// {
	// 	printf("arg : %s\n", args[i]);
	// 	++i;
	// }
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
		ft_putendl_fd("syntax error!", 2);	
		return (0);
	}
	else if (ft_strlen(args[i]) == 2)
	{
		if (args[i][0] != '>' * -1 || args[i][1] != '>' * -1)
		{
			ft_putendl_fd("syntax error!", 2);	
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

char	**pipe_split_body(char ***args)
{
	int		j;
	int		args_count;
	char	**res;
	
	args_count = count_args(*args);
	if (!(res = (char **)malloc(sizeof(char *) *(args_count + 1))))
		return (0);
	res[args_count] = 0;
	j = -1;
	while (++j < args_count)
	{
		if (!(res[j] = ft_strdup(**args)))
		{
			free_2d_array(res);
			return (0);
		}
		++(*args);
	}
	return (res);
}

char	***pipe_split(char **args)
{
	int		cmds_count;
	int		i;
	char	***res;

	cmds_count = count_cmds(args);
	if (!(res = (char ***)malloc(sizeof(char **) * (cmds_count + 1))))
		return (0);
	res[cmds_count] = 0;
	i = 0;
	while (i < cmds_count)
	{
		if (!(res[i] = pipe_split_body(&args)))
		{
			free_3d_array(res);
			return (0);
		}
		++i;
		++args;
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
	if (!(args = tokens_into_args(tokens, env)))
	{
		ft_lstclear(&tokens, free);
		return (0);
	}
	ft_lstclear(&tokens, free);
	return (args);
}

int exec_builtin_cmds(char **tokens, t_list *env, int *p_status)
{
	if (ft_strcmp(tokens[0], "exit") == 0)
		sh_exit(tokens, p_status);
	else if (ft_strcmp(tokens[0], "cd") == 0)
		sh_cd(tokens, env, p_status);
	else if (ft_strcmp(tokens[0], "export") == 0)
		sh_export(tokens, env, p_status);
	else if (ft_strcmp(tokens[0], "unset") == 0)
		sh_unset(tokens, env, p_status);
	else if (ft_strcmp(tokens[0], "echo") == 0)
		sh_echo(tokens, p_status);
	else if (ft_strcmp(tokens[0], "pwd") == 0)
		sh_pwd(p_status);
	else if (ft_strcmp(tokens[0], "env") == 0)
		sh_env(tokens, env, p_status);
	else
		return (0);
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
	i = -1;
	while (++i < count)
	{
		if (is_redirection(**cmd))
			cmd += 2;
		if (!(res[i] = ft_strdup(*cmd)))
		{
			ft_free(res, i);
			return (0);
		}
		++cmd;
	}
	return (res);
}

char ***get_cmds_with_pipe_split(char *line, t_list *env)
{
	char	**args;
	char	***cmds;
	
	if (!(args = tokenize(line, env)))
		return (0);
	if (!(cmds = pipe_split(args)))
	{
		free_2d_array(args);
		return (0);
	}
	free_2d_array(args);
	return (cmds);
}

int		process_pipe_and_redirection(char ***cmds, int *pipe_fd, int *redirection)
{
	int file_fd;
	
	close(pipe_fd[0]);
	if (*(cmds + 1) != 0)
		dup2(pipe_fd[1], 1);
	close(pipe_fd[1]);
	*redirection = check_redirection(*cmds, &file_fd);
	if (file_fd < 0)
	{
		ft_putendl_fd("file fd error!", 2);
		return (0);
	}
	else if (*redirection == 1 || *redirection == 2)
		dup2(file_fd, 1);
	else if (*redirection == 3)
		dup2(file_fd, 0);
	return (1);
}

int	parent_process(pid_t pid, int *pipe_fd, int *p_status)
{
	int		status;
	pid_t	wait_pid;

	close(pipe_fd[1]);
	dup2(pipe_fd[0], 0);
	close(pipe_fd[0]);
	wait_pid = waitpid(pid, &status, 0);
	if (wait_pid == -1)
		ft_putstr_fd("wait_pid returns error. no child process", 2);
	*p_status = status / 256;
	return (1);
}

int main_cmd_with_fork(char **cmd, t_list *env, char **envp, int *p_status)
{
	int ret;

	if ((ret = exec_builtin_cmds(cmd, env, p_status)) == -1)
	{
		free_2d_array(cmd);
		return(0);
	}
	else if (ret == 0)
	{
		cmd_not_builtins(cmd, envp);
	}
	free_2d_array(cmd);
	return (1);
}

char	**refine_cmd(char ***cmds, int pipe_fd[])
{
	char	**cmd;
	int		redirection;

	if (!process_pipe_and_redirection(cmds, pipe_fd, &redirection))
		return (0);
	if (!(cmd = get_cmd(*cmds, redirection))) // redirection과 child process pipe 처리 거친 명령어 ex) echo hi, ls, ...
		return (0);
	return (cmd);
}

int process_cmds_with_fork(char ***cmds, t_list *env, char **envp, int *p_status)
{
	pid_t	pid;
	int		pipe_fd[2];
	char	**cmd;
	
	pipe(pipe_fd);
	if ((pid = fork()) == -1)
	{
		ft_putstr_fd("Fork is failed.", 2);
		return (0);
	}
	else if (pid == 0)
	{
		if (!(cmd = refine_cmd(cmds, pipe_fd)))
			exit(1);
		if (!main_cmd_with_fork(cmd, env, envp, p_status)) //2d array cmd free처리 안에서 다 한다.
			exit(1);
		exit(0);
	}
	else
	{	
		if (!parent_process(pid, pipe_fd, p_status))
			return (0);
	}
	return (1);
}

int exec_not_builtin_cmds(char **cmd, char **envp, int *p_status)
{
	pid_t	pid;
	int		status;
	
	if ((pid = fork()) == -1)
	{
		ft_putendl_fd("FORK FAILED", 2);
		return (0);
	}
	else if (pid == 0)
	{
		if (!cmd_not_builtins(cmd, envp))
			exit(1);
		exit(0);
	}
	else
	{		
		if (waitpid(pid, &status, 0) == -1)
		{
			ft_putendl_fd("waitpid returns error. no child process", 2);
			return (0);
		}
		*p_status = status / 256;
	}
	return (1);
}

int process_cmd_without_fork(char ***cmds, t_list *env, char **envp, int *p_status)
{
	if ((*cmds)[0] == 0)
		return (1);
	if (!check_fd_aggregation(*cmds))
		return (0);
	if (!exec_builtin_cmds(*cmds, env, p_status))
	{
		if (!exec_not_builtin_cmds(*cmds, envp, p_status))
			return (0);
	}
	return (1);
}

int if_no_need_pipe(char ***cmds)
{
	int len;
	int redirection_flag;
	int i;

	len = 0;
	while (cmds[len] != 0)
		++len;
	redirection_flag = 0;
	i = 0;
	while (cmds[0][i] != 0)
	{
		if (has_redirection(cmds[0][i]))
		{
			redirection_flag = 1;
			break ;
		}
		++i;
	}
	if (len == 1 && !redirection_flag)
		return (1);
	return (0);
}

int process_cmd(char ***cmds, t_list *env, char **envp, int *p_status)
{
	if (if_no_need_pipe(cmds))
	{
		process_cmd_without_fork(cmds, env, envp, p_status);
		return (1);
	}
	else
	{
		while (*cmds)
		{
			if ((*(cmds))[0] == 0)
				return (1);
			if (!validate_redirection(*cmds))
				return (1);
			if (!check_fd_aggregation(*cmds))
				return (0);
			if (!process_cmds_with_fork(cmds, env, envp, p_status))
					return (0);
			++cmds;
		}
	}
	return (1);
}

int exec_cmd_with_pipe(char *input, t_list *env, char **envp, int *p_status)
{
	int		save[2];
	char	***cmds;

	if (!(cmds = get_cmds_with_pipe_split(input, env)))
		return (0);
	save[0] = dup(0);
	save[1] = dup(1);
	if (!process_cmd(cmds, env, envp, p_status))
	{
		free_3d_array(cmds);
		return (0);
	}
	dup2(save[0], 0);
	dup2(save[1], 1);
	free_3d_array(cmds);
	return (1);
}

int dollar_question(t_list *env, int *p_status)
{
	char *new;
	char *var;
	char *status;

	if (!(status = ft_itoa(*p_status)))
		return (0);
	if (!(var = ft_strdup("?=")))
	{
		free(status);
		return (0);
	}
	if (!(new = ft_strjoin(var, status)))
	{
		free(var);
		free(status);
		return (0);
	}
	free(env->content);
	if (!(env->content = new))
	{
		free(status);
		return (0);
	}
	free(status);
	return (1);
}

int exec_input(char *line, t_list *env, char **envp, int *p_status)
{
	int		i;
	char	**inputs;

	if (only_semicolon(line))
		return (1);
	if (!(inputs = semicolon_split(line)))
		return (0);
	i = -1;
	while (inputs[++i] != 0)
	{
		if (!dollar_question(env, p_status))
			return (0);
		if (!exec_cmd_with_pipe(inputs[i], env, envp, p_status))
		{
			free_2d_array(inputs);
			return (0);
		}
	}
	free_2d_array(inputs);
	return (1);
}

t_list *copy_env(char **envp)
{
	int		i;
	t_list	*res;
	t_list	*new;
	char	*zero;

	if (!(zero = ft_strdup("?=0")))
		return (0);
	if (!(res = ft_lstnew(zero)))
		return (0);
	i = -1;
	while (envp[++i] != 0)
	{
		if (!(new = (t_list *)malloc(sizeof(t_list))))
			return (0);
		if (!(new->content = ft_strdup(envp[i])))
		{
			free(new);
			ft_lstclear(&res, free);
		}
		new->next = 0;
		ft_lstadd_back(&res, new);
	}
	return (res);
}

void	sig_handler(int signo)
{
	if (signo == SIGINT)
	{
		ft_putendl_fd(" SIGINT!", 1);
		ft_putstr_fd("> ", 1);
	}
	if (signo == SIGQUIT)
	{
		ft_putendl_fd("SIGQUIT!", 1);
	}
}

int main(int argc, char **argv, char **envp)
{
	char	*line;
	int		read_status;
	int		p_status;
	t_list	*env;

	if (!argc || !argv || !envp)
		return (0);
	if (!(env = copy_env(envp)))
		return (0);
	signal(SIGINT, sig_handler);
	signal(SIGQUIT, sig_handler);
	line = 0;
	hello_sh();
	read_status = 1;
	p_status = 0;
	while (read_status > 0)
	{
		read_status = get_next_line(0, &line);
		if (!exec_input(line, env, envp, &p_status))
		{
			free(line);
			line = 0;
			break;
		}
		free(line);
		if (read_status > 0)
			ft_putstr_fd("> ", 1);
	}
	if (!line)
		free(line);
	ft_lstclear(&env, free);
	return (0);
}