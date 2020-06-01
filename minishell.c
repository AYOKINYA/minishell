#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>

int main(void)
{
	pid_t	pid;
	pid_t	wait_pid;
	int		i;
	int		status;
	
	pid = fork();
	
	if (pid > 0)
	{
		printf("parent process starts\n");
		printf("partent PID : %d\n", getpid());
		i = 0;
		while (i < 3)
		{
			sleep(1);
			++i;
		}
		wait_pid = wait(&status);
		if (wait_pid == -1)
			printf("wait() returns error. no child process");
		else
			printf("wait : child process is done\n");
		printf("parent process ends\n");
		exit(0);
	}
	else if (pid == 0)
	{
		printf("child process starts\n");
		printf("child PID : %d\n", getpid());
		i = 0;
		while (i < 3)
		{
			sleep(1);
			printf("%d\n", i);
			++i;
		}
		printf("child process ends\n");
		exit(0);
	}
	else
	{
		printf("fork failed!");
		return (-1);
	}
	return (0);
}
