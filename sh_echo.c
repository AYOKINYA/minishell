#include <unistd.h>
#include <stdio.h>

int ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t			i;
	unsigned char	*u_s1;
	unsigned char	*u_s2;

	i = 0;
	u_s1 = (unsigned char*)s1;
	u_s2 = (unsigned char*)s2;
	if (n == 0)
		return (0);
	while (u_s1[i] != '\0' && u_s2[i] != '\0' && i < n)
	{
		if (u_s1[i] != u_s2[i])
			return ((u_s1[i] - u_s2[i]));
		else
			++i;
	}
	if (i == n)
		return (0);
	return (u_s1[i] - u_s2[i]);
}

int ft_strlen(const char *s)
{
	int len;

	len = 0;
	while (s[len] != 0)
		++len;
	return (len);
}

int main(int argc, char *argv[])
{
	int i;
	int len;
	
	if (argc < 2)
		return 0;
	if (argc == 2)
	{	
		if (ft_strncmp(argv[1], "echo", 4) == 0)
			write(1, "\n", 2);
		return 0;
	}
	if (ft_strncmp(argv[2], "-n", 2) == 0)
		i = 3;
	else
		i = 2;
	while (i < argc)
	{
		len = ft_strlen(argv[i]);
		write(1, argv[i], len);
		++i;
	}
	if (ft_strncmp(argv[2], "-n", 2) != 0)
		write(1, "\n", 1);
}