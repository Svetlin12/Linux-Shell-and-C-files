#include <unistd.h>
#include <stdlib.h>
#include <err.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char** argv)
{
	if (argc != 2)
		errx(1, "usage: ./main [command]");
	
	pid_t res = fork();
	if (res == -1)
		err(2, "could not fork");

	if (res == 0)	
		if (execlp(argv[1], argv[1], (char*)NULL) == -1)
			err(3, "could not execute %s", argv[1]);
	
	int status;
	if (wait(&status) == -1)
     		err(4, "could not execute wait");
	
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
		printf("the executed command is: %s\n", argv[1]);
	else
		printf("the command could not be executed\n");
	
	exit(0);
}
