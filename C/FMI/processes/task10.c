#include <stdlib.h>
#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char** argv)
{
	if (argc != 3)
		errx(1, "usage: ./main [command1] [command2]");

	pid_t pid1 = fork();
	if (pid1 == -1)
		err(2, "could not fork");

	if (pid1 == 0)
	{
		if (execlp(argv[1], argv[1], 0) == -1)
			err(3, "could not execute command %s", argv[1]);
	}

	int status;
	if (wait(&status) == -1)
		err(4, "error occurred in wait");

	if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
	{
		warnx("child process did not end normally");
		exit(-1);	
	}

	pid_t pid2 = fork();
	if (pid2 == -1)
		err(5, "could not fork again");

	if (pid2 == 0)
	{
		if (execlp(argv[2], argv[2], 0) == -1)
			err(6, "could not execute command %s", argv[2]);
	}

	if (wait(NULL) == -1)
		err(7, "error occurred in wait");

	exit(0);
}
