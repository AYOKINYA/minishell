#include "minishell.h"

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

void	get_value(char *env_content, char **res)
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