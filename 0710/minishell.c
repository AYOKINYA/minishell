/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkang <jkang@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/30 15:16:41 by jkang             #+#    #+#             */
/*   Updated: 2020/07/09 22:08:45 by jkang            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_list	*copy_env(char **envp)
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

static void		sig_handler(int signo)
{
	if (signo == SIGINT)
	{
		g_sig_status = 1;
		ft_putendl_fd(" SIGINT!", 2);
		printf("g_sig_status %d\n", g_sig_status);
		ft_putstr_fd("sig> ", 2);
	}
	if (signo == SIGQUIT)
	{
		ft_putendl_fd("SIGQUIT!", 2);
		exit(0);
	}
}

static void		welcome(void)
{
	ft_putendl_fd("Hi, it's jkang's minishell.", 1);
	ft_putstr_fd("> ", 1);
}

static int		go_minishell_go(t_list *env)
{
	char	*line;
	int		p_status;
	int		read_status;

	line = 0;
	read_status = 1;
	p_status = 0;
	while (read_status > 0)
	{
		read_status = get_next_line(0, &line);
		if (!exec_input(line, env, &p_status))
		{
			free(line);
			p_status = 1;
			return (0);
		}
		if (read_status > 0)
			ft_putstr_fd("> ", 1);
		else if (read_status == -1)
			return (0);
		if (p_status == 130)
			g_sig_status = 0;
		free(line);
	}
	return (1);
}
int				main(int argc, char **argv, char **envp)
{
	t_list	*env;

	if (!argc || !argv || !envp)
		return (0);
	if (!(env = copy_env(envp)))
		return (0);
	signal(SIGINT, sig_handler);
	signal(SIGQUIT, sig_handler);
	welcome();
	if (!go_minishell_go(env))
		return (-1);
	ft_lstclear(&env, free);
	return (0);
}
