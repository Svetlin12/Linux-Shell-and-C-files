#include <err.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

int main()
{
	char prompt[] = "pesho's prompt: ";
	write(1, &prompt, strlen(prompt));
	ssize_t readSize;
	char buff[256];
	while ((readSize=read(0, &buff, sizeof(buff))) > 0)
	{
		for (int i = 0; i < readSize; i++)
		{
			if (buff[i] == ' ' || buff[i] == '\n')
			{
				buff[i] = 0;
				break;
			}
		}
		
		if (strcmp(buff, "exit") == 0)
			break;

		pid_t pid = fork();
		if (pid == -1)
			err(1, "Could not fork");

		if (pid == 0)
		{
			if (execlp(buff, buff, (char*)0) == -1)
				err(2, "Could not execute command %s", buff);
		}
		
		int status;
		if (wait(&status) == -1)
			err(3, "Could not wait for child");

		if (!WIFEXITED(status))
			err(4, "Child did not terminate well");

		if (WEXITSTATUS(status))
			warnx("Child did not end with exit code 0");

		write(1, &prompt, strlen(prompt));
	}

	if (readSize == -1)
		err(5, "Error occurred while reading from stdin");

	exit(0);
}
