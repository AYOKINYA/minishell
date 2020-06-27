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

int get_line()
{
	int before_eof;
	char buf[100 + 1];
	int ret;

	before_eof = 0;
	if (before_eof)
	{
		while ((ret = read(0, buf, 100)) > 0)
		{
			buf[ret] = '\0';
			printf("%s\n" , buf);
			printf("ret %d\n" , ret);
		}
		return (0);
	}
	else
	{
		while ((ret = read(0, buf, 100)) > 0)
		{
			buf[ret] = '\0';
			printf("%s\n" , buf);
			printf("ret %d\n" , ret);
		}
		if (ret == 0 && ft_strlen(buf))
		{
			before_eof = 1;
			return 1;
		}
	}
	return (ret);
}
int main(int arg, char **argv)
{
	
	while (1)
	{
		if (get_line() == 0)
			return (0);
		printf("hi\n");
	}
	return (0);
}