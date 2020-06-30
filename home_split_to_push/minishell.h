/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkang <jkang@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/30 16:45:08 by jkang             #+#    #+#             */
/*   Updated: 2020/06/30 16:45:09 by jkang            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "./libft/libft.h"
# include <unistd.h>
# include <stdlib.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <stdio.h>
# include <signal.h>

int			get_next_line(int fd, char **line);

int			count_tokens(char **tokens);
int			exec_builtin_cmds(char **tokens, t_list *env, int *p_status);
int			cmd_not_builtins(char **tokens, char **envp);

int			sh_echo(char **tokens, int *p_status);
int			sh_exit(char **tokens, int *p_status);
int			sh_env(char **tokens, t_list *env, int *p_status);
int			sh_pwd(int *p_status);
int			sh_cd(char **tokens, t_list *env, int *p_status);
int			sh_export(char **tokens, t_list *env, int *p_status);
int			sh_unset(char **tokens, t_list *env, int *p_status);
int			search_with_path(char **tokens, char **envp);

void		ft_free(char **map, int index);
void		free_2d_array(char **arr);
void		free_3d_array(char ***arr);

char		***pipe_split(char **args);

int			only_semicolon(char *line);
char		**semicolon_split(char *line);

int			redirection_split(char *res, t_list *tokens);

char		*read_quoted_token(int *quote, char **line);

char		*read_unquoted_token(int *quote, char **line);

void		quote_mark(int *quote, char c);
char		*exception_substr(char *from, int len, char *except_str);
int			patch_token(char *res, t_list *tokens);

int			is_redirection(char c);
int			has_redirection(char *arg);
int			validate_redirection(char **args);

char		**tokenize(char *line, t_list *env);

void		get_value(char *env_content, char **res);
int			get_value_len(char *env_content);

int			is_env_var(char *content);
char		*convert_var_to_value(char *content, t_list *env);

char		**refine_cmd(char ***cmds, int pipe_fd[]);

int			process_cmds_with_fork(char ***cmds, t_list *env, char **envp,\
															int *p_status);
int			process_cmd_without_fork(char ***cmds, t_list *env, char **envp,\
															int *p_status);

int			check_fd_aggregation(char **args, int *p_status);
int			process_cmd(char ***cmds, t_list *env, char **envp, int *p_status);

int			exec_input(char *line, t_list *env, char **envp, int *p_status);

int			g_sig_status;
#endif
