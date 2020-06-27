#include "minishell.h"

static int	export_new_var(char *token, t_list *env)
{
	t_list	*new;
	
	if (!(new = (t_list *)malloc(sizeof(t_list))))
		return (0);
	if (!(new->content = ft_strdup(token)))
	{
		free(new);
		return (0);
	}
	new->next = 0;
	ft_lstadd_back(&env, new);
	return (1);
}

static int	is_var_already(char *token, t_list *env)
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
			free(tmp->content);
			if (!(tmp->content = ft_strdup(token)))
				return (0);
			return (1);
		}
		tmp = tmp->next;
	}
	return (0);
}

static int	sh_export_body(char **tokens, t_list *env, int i)
{

	if (ft_strlen(tokens[i]) == 1)
	{
		ft_putendl_fd("bash: export: `=': not a valid identifier", 2);
		return (-1);
	}
	else
	{
		if (!is_var_already(tokens[i], env))
		{
			if (!export_new_var(tokens[i], env))
				return (0);
		}
	}
	return (1);
}

static void print_env_w_declare(t_list *env)
{
	env = env->next;
	while (env != 0)
	{
		
		ft_putstr_fd("declare -x ", 1);
		ft_putendl_fd(env->content, 1);
		env = env->next;
	}
}

int			sh_export(char **tokens, t_list *env, int *p_status)
{
	int		i;
	int		token_count;
	
	token_count = count_tokens(tokens);
	if (token_count == 1)
		print_env_w_declare(env);
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