#include <err.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char** argv)
{
	if (argc != 4)
		errx(1, "Invalid argument count. Usage: %s (command 1) (command 2) (command 3)", argv[0]);

	for (int i = 1; i < argc; i++)
	{ 	
		pid_t pid = fork();
		if (pid == -1)
			err(2, "Could not fork");

		if (pid == 0)
		{
			if (execlp(argv[i], argv[i], 0) == -1)
				err(3, "Could not complete execlp");
		}

		int status;
		if (wait(&status) != pid)
			err(4, "Error occurred while waiting for child to complete");
		
		if (WIFEXITED(status))
			printf("The child who just finished has PID: %d and exit code: %d\n", pid, WEXITSTATUS(status));
		else
			warn("The process did not finish well");
	}

	exit(0);
}
