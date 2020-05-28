#include <stdlib.h>
#include <err.h>
#include <sys/types.h>   
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char** argv)
{
	int successful = 0;
	int unsuccessful = 0;
	for (int i = 1; i < argc; i++)
	{
		pid_t pid = fork();
		
		if (pid < 0)	
			err(1, "could not fork");

		if (pid == 0)
		{
			if (execlp(argv[i], argv[i], 0) == -1)
				err(2, "error in executing command %s", argv[i]);
		}
		
		int status;
		if (wait(&status) == -1)
			err(3, "error in wait");

		if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
			successful++;
		else
			unsuccessful++;		
	}

	printf("successful commands: %d\nunsuccessful commands: %d\n", successful, unsuccessful);

	exit(0);
}
