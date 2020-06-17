#include "minishell.h"

int sh_pwd(int *p_status)
{
	char buf[1024];

	if (!getcwd(buf, 1024))
	{
		*p_status = 1;
		return (0);
	}
	ft_putendl_fd(buf, 1);
	*p_status = 0;
	return (1);
}