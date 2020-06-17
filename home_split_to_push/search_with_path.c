#include "minishell.h"

static int	count_path_len(char *env_path)
{
	int count;

	count = 0;
	while (env_path[count] != '\0' && env_path[count] != ':')
		++count;
	return (count);

}

static char	*get_env_path(char **envp)
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

static char	*get_path(char *env_path, int path_len)
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

static int	exec_with_path(char **tokens, char **envp, char *env_path, int path_len)
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

int			search_with_path(char **tokens, char **envp)
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