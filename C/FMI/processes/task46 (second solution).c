#include <err.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void executeCommand(char** args)
{
	pid_t pid = fork();
	if (pid == -1)
		err(2, "Could not fork");

	if (pid == 0)
	{
		if (execvp(args[0], args) == -1)
			err(3, "Could not execute command %s", args[0]);
	}	
}

int main(int argc, char** argv)
{
	if (argc != 1 && argc != 2)
		errx(1, "Invalid argument count. Usage: %s cmd", argv[0]);

	char* cmd;
	if (argc == 2)
		cmd = argv[1];
	else
		cmd = "echo";

	const int size = 256;
	char buff[size], option[5];
	char* args[4];
	args[0] = cmd;
	ssize_t readSize;
	int currArg = 1; 
	while ((readSize=read(0, &buff, sizeof(buff))) > 0)
	{
		int wordSize = 0;
		for (int i = 0; i < readSize; i++)
		{
			if (buff[i] == ' ' || buff[i] == '\n')
			{
				option[wordSize++] = 0;
				args[currArg] = malloc(wordSize);
				if (args[currArg] == NULL)
					err(4, "No memory left to allocate");
				strcpy(args[currArg++], option);
				wordSize = 0;
			}
			else
			{
				option[wordSize] = buff[i];
				wordSize++;
				if (wordSize > 4)
					errx(5, "Command argument too big. It has to be less than 4 characters");
			}

			if (currArg == 3)
			{
				args[currArg] = 0;
				executeCommand(args);
				currArg = 1;
				int status;
				if (wait(&status) == -1)
					err(6, "Could not wait for child");

				if (!WIFEXITED(status))
					err(7, "Child did not terminate well");

				if (WEXITSTATUS(status))
					warn("Child did not end with exit code 0");

				for (int i = 1; i < 4; i++)
					free(args[i]);
			}
		}	
	}
	if (currArg == 2)
	{
		args[currArg] = 0;
		executeCommand(args);
			
		int status;
		if (wait(&status) == -1)
			err(6, "Could not wait for child");

		if (!WIFEXITED(status))
			err(7, "Child did not terminate well");

		if (WEXITSTATUS(status))
			warn("Child did not end with exit code 0");
		
		for (int i = 1; i < 3; i++)
			free(args[i]);
	}
	
	exit(0);
}
