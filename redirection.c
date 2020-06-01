#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>

// process를 하나 생성한다. (Parent + Child)
// ls -al > tmp.txt

// 자식프로세스 생성

int main(int argc, char *argv[])
{
	pid_t	pid1;
	int		fd;

	pid1 = fork();
	if (pid1 == 0) {  /* child */

		//tmp.txt에 해당되는 file descriptor (fd) 생성
		//O_CREAT를 없애고, O_APPEND를 해주면 '>>' redirection임
		fd = open(argv[argc], O_RDWR | O_CREAT | S_IROTH, 0644);
		
		//가끔 fd를 생성할 수 없는 상황 (Permission 혹은 fd overflow 등 존재) 예외처리
		if (fd < 0)
		{
			perror ("error");
			exit(-1);
		}

		//Standard output을 위에서 create한 fd로 가도록 설정
		dup2(fd, STDOUT_FILENO);
		//dup2 실행 후, 자식의 모든 출력은 fd로 저장됨
		close(fd);
		
		//exec로 원하는것 실행
		argv[argc] = NULL;
		if (argc != 0){
			execvp(*argv, argv);
			fprintf(stderr, "minish : command not found\n");
			exit(127);
		}
		exit(0);
	}

	/* Parent: Shell */
	waitpid(pid1, NULL, 0);

	//부모는 자식이 끝나면 다시 원래대로..
	//Input redirection 역시 같은 manner로,
	//fd가 Standard output였다면, 이젠 Standard input으로 해주면 된다.


	pid1 = fork();
	if (pid1 == 0) {  /* child */
		fd = open(argv[argc], O_RDONLY);
		if (fd < 0) {
				perror ("error");
				exit(-1);
		}
		dup2(fd, STDIN_FILENO);
		close(fd);

		argv[argc] = NULL;
		if (argv != 0){
			execvp(*argv, argv);
			fprintf(stderr, "minish : command not found\n");
			exit(127);
		}
		exit(0);
	}

	/* Parent: Shell */
	waitpid(pid1, NULL, 0);
	return (0);
}
