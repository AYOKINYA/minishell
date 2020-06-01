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

// void sh_loop(void)
// {
// 	int status;
// 	int ret;

// 	status = 1;

// 	while (status)
// 	{
// 		ft_putchar(" >");
// 	}
	
// 	if (ft_strcmp("exit", cmd, 4) == 0)
//     {
// 		ft_putchar("Bye!");
// 		exit (0);
// 	}
// }

int echo_dollar_question(void)
{
	return (0);
}

int echo_dollar(char *token, char **env)
{
	int		i;
	int		j;
	char	*cmp;

	i = 0;
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
		while (env[i] != 0)
		{
			if (ft_strncmp(env[i], cmp, ft_strlen(cmp)) == 0)
				ft_putstr_fd((env[i] + ft_strlen(cmp) + 1), 1);
			++i;
		}
		free(cmp);
	}
	return (1);
}

int sh_echo(char **tokens, char **env)
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

int sh_env(char **tokens, char **env)
{
	int i;
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
	i = 0;
	while (env[i] != 0)
	{
		ft_putstr_fd(env[i], 1);
		write(1, "\n", 1);
		++i;
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

int sh_cd(char **tokens, char **env)
{
	int token_count;
	int cd_result;
	int i;

	token_count = 0;
	while (tokens[token_count] != 0)
		++token_count;
	if (token_count == 1 || (token_count == 2 && ft_strlen(tokens[1]) == 1 && tokens[1][0] == '~'))
	{
		i = 0;
		while (env[i] != 0)
		{
			if (ft_strncmp(env[i], "HOME=", 5) == 0)
			{
				ft_putendl_fd(env[i] + 5, 1);
				chdir((env[i] + 5));
				sh_pwd();
			}
			++i;
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

int cmd_not_exists(char **tokens)
{
	ft_putstr_fd("zsh: command not found: ", 1);
	ft_putendl_fd(tokens[0], 1);
	return (0);
}

int exec_cmds(char **tokens, char **env)
{
	int		status;
	pid_t	pid;
	pid_t	wait_pid;
	
	if (ft_strcmp(tokens[0], "exit") == 0)
		sh_exit();
	else if (ft_strcmp(tokens[0], "cd") == 0)
		sh_cd(tokens, env);
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
			// else if (ft_strcmp(tokens[0], "export") == 0)
			// 	sh_export(tokens);
			// else if (ft_strcmp(tokens[0], "unset") == 0)
			// 	sh_unset(tokens);
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

int main(int argc, char **argv, char **env)
{
	char	*line;
	char	**cmd;
	char	**tokens;
	int		i; 
	int		j;
	int		status;

	if (!argc || !argv || !env)
		return (0);
	line = 0;

	hello_sh();
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
			tokens = ft_split(cmd[i], " \\"); //malloc guard
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

	return (0);
}