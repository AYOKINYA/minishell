#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>


int ft_strlen(char *s)
{
	int i;

	i = 0;
	while (*s)
	{
		++i;
		++s;
	}
	return (i);
}
int main(int arg, char **argv)
{
	char buf[100 + 1];
	int ret;

	while ((ret = read(0, buf, 100)) > 0)
	{
		buf[ret] = '\0';
		if (ret == 0 && ft_strlen(buf))
			ret = 1;
		printf("%s\n" , buf);
		printf("ret %d\n" , ret);
	}
	return (0);
}