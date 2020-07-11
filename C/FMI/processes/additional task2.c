#include <err.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

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
	if (argc != 2)
		errx(1, "Invalid argument count. Usage: %s cmd", argv[0]);

	char* cmd = argv[1];

	ssize_t readSize;
	char buff[256];
	while ((readSize=read(0, &buff, sizeof(buff))) > 0)
	{
		int optionsCnt = 0;
		for (int i = 0; i < readSize; i++)
		{
			if (buff[i] == ' ' || buff[i] == '\n')
				optionsCnt++;
		}

		char* args[++optionsCnt];
		int currArg = 0, lastReadEnd = 0, cnt = 0;
		args[currArg++] = cmd;
		char opt[256];
		for (int i = 0; i < readSize; i++)
		{
			if (buff[i] == ' ' || buff[i] == '\n')
			{
				opt[cnt] = 0;
				cnt = 0;
				args[currArg] = malloc(i - lastReadEnd);
				lastReadEnd = i;
				if (args[currArg] == NULL)
					err(2, "No memory left to allocate");
				strcpy(args[currArg++], opt);
				continue;
			}
			opt[cnt++] = buff[i];
		}

		args[currArg] = NULL;
		
		executeCommand(args);

		if (wait(NULL) == -1)
			err(3, "Could not wait for child");

		for (int i = 1; i < currArg; i++)
			free(args[i]);

		write(1, "\n", 1);
	}	

	if (readSize == -1)
		err(7, "Error occurred while reading from stdin");

	exit(0);
}
