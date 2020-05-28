#include <unistd.h>
#include <err.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>

int main(int argc, char** argv)
{
	if (argc != 4)
		errx(1, "usage: ./main [command1] [command2] [command3]");

	pid_t pid = fork();
	int status;
	
	if (pid == -1)
		err(2, "could not fork");

	if (pid == 0)
	{
		if (execlp(argv[1], argv[1], 0) == -1)
			err(3, "could not execute command %s", argv[1]);
	}	
	
	if (wait(&status) == -1)
		err(4, "error in wait");
	
	if (WIFEXITED(status))
		printf("pid of process: %d, exit code: %d\n", pid, WEXITSTATUS(status));	
	
	pid = fork();

	if (pid == -1)
		err(2, "could not fork");

	if (pid == 0)
	{
		if (execlp(argv[2], argv[2], 0) == -1)
			err(3, "could not execute command %s" ,argv[2]);
	}

	if (wait(&status) == -1)
		err(4, "error in wait");

	if (WIFEXITED(status))
		printf("pid of process: %d, exit code: %d\n", pid, WEXITSTATUS(status));	
	
	pid = fork();

	if (pid == -1)
		err(2, "could not fork");

	if (pid == 0)
	{
		if (execlp(argv[3], argv[3], 0) == -1)
			err(3, "could not execute command %s" ,argv[3]);
	}

	if (wait(&status) == -1)
		err(4, "error in wait");

	if (WIFEXITED(status))
		printf("pid of process: %d, exit code: %d\n", pid, WEXITSTATUS(status));	
	
	exit(0);
}
