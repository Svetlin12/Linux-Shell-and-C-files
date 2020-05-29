#include <err.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>

void errorHandler(int code, int errnum, char* toFree)
{
	free(toFree);
	errno = errnum;
	err(code, "error code: %d", code);
}

int main()
{	
	char prompt[15] = "user@main.c:~$ ";
	write(1, &prompt, sizeof(prompt));
	char buffer[256];
	char* command;
	ssize_t readSize;
	while ((readSize=read(0, &buffer, sizeof(buffer))) > 0)
	{
		int commandLocation = 0;
		for (int i = 0; i < readSize; i++)
		{
			if (buffer[i] == ' ' || buffer[i] == '\t' || buffer[i] == '\n')
				break;
			commandLocation++;
		}
	
		command = malloc(commandLocation + 1);
		if (command == NULL)
			err(1, "could not malloc");

		for (int i = 0; i < commandLocation; i++)
			command[i] = buffer[i];

		command[commandLocation] = '\0';

		if (strcmp(command, "exit") == 0)
		{
			free(command);
			break;
		}
		
		pid_t pid = fork();
		if (pid < 0)
			errorHandler(2, errno, command);

		if (pid == 0)
		{
			if (execlp(command, command, 0) == -1)
				errorHandler(3, errno, command);
		}
		
		if (wait(NULL) == -1)
			errorHandler(4, errno, command);

		free(command);
		write(1, &prompt, sizeof(prompt));
	}

	exit(0);
}
