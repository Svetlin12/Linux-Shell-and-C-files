#include <err.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char** argv)
{
	if (argc != 3)
		errx(1, "Invalid argument count. Usage: %s (command 1) (command 2)", argv[0]);

	pid_t pid1 = fork();

	if (pid1 == -1)
		err(2, "Could not fork");
	
	if (pid1 == 0)
	{
		if (execlp(argv[1], argv[1], 0) == -1)
			err(3, "Could not execute execlp");
	}
	
	int status;
	if (wait(&status) == -1)
		err(4, "Error while waiting for child to end");
	
	if (!WIFEXITED(status) || WEXITSTATUS(status))
		errx(-1, "Child did not end successfully");
	
	pid_t pid2 = fork();
	
	if (pid2 == -1)
		err(5, "Could not fork");

	if (pid2 == 0)
	{
		if (execlp(argv[2], argv[2], 0) == -1)
			err(6, "Could not execute execlp");
	}

	if (wait(&status) == -1)
		err(7, "Error while waiting for child to end");

	if (!WIFEXITED(status) || WEXITSTATUS(status))
		errx(-1, "Child did not end successfully");

	exit(0);
}
