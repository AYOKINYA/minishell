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

int echo_dollar(char *token, t_list *env)
{
	int		j;
	char	*cmp;

	j = 1;
	if (ft_strlen(token) == 1)
		write(1, token, 1);
	else if (ft_strcmp(token, "$?") == 0)
		echo_dollar_question();
	else
	{
		cmp = (char *)malloc(sizeof(char) * (ft_strlen(token) + 1));
		while (token[j] != '\0')
		{
			cmp[j - 1] = token[j];
			++j;
		}
		cmp[j] = '=';
		cmp[j + 1] = '\0';
		while (env != 0)
		{
			if (ft_strncmp(env->content, cmp, ft_strlen(cmp)) == 0)
				ft_putstr_fd((env->content + ft_strlen(cmp) + 1), 1);
			env = env->next;
		}
		free(cmp);
	}
	return (1);
}

int sh_echo(char **tokens, t_list *env)
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
		if (tokens[i][0] == '$')
			echo_dollar(tokens[i], env);
		else
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
		ft_putendl_fd(": No such file or directory", 1);
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
		return (-1);
	return (0);
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

int exec_cmds(char **tokens, t_list *env)
{
	int		status;
	pid_t	pid;
	pid_t	wait_pid;
	
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
				sh_echo(tokens, env);
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
	char	**cmd;
	char	**tokens;
	int		i; 
	int		j;
	int		status;
	t_list	*env;

	if (!argc || !argv || !envp)
		return (0);
	line = 0;

	hello_sh();
	
	if (!(env = copy_env(envp)))
		return (-1);

	status = 1;
	while (status)
	{
		if ((status = get_next_line(0, &line)) == -1)
			return (-1);
		line = ft_strtrim(line, " "); //malloc guard
		cmd = ft_split(line, ";\n"); //malloc guard
		i = 0;
		while (cmd[i] != 0)
		{
			cmd[i] = ft_strtrim(cmd[i], " "); //malloc guard
			tokens = ft_split(cmd[i], " \t"); //malloc guard
			if (tokens[0] != 0)
				exec_cmds(tokens, env);
			j = 0;
			while (tokens[j] != 0)
			{
				free(tokens[j]);
				++j;
			}
			free(tokens);
			free(cmd[i]);
			++i;
		}
		free(cmd);
		free(line);
		line = 0;
		ft_putstr_fd("> ", 1);
	}
	ft_lstclear(&env, free);

	return (0);
}