#include <err.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>

int main(int argc, char** argv)
{
	if (argc > 2)
		errx(1, "usage: %s [command name]/no command name", argv[0]);

	char* command;
	if (argc == 2 && strlen(argv[1]) <= 4)
		command = argv[1];
	else if (argc == 1)
		command = "echo";
	else
		errx(2, "the length of the command is to be less than or equal to 4 chars");

	char command1[5];
	char command2[5];
	int toPlace = 1;
	int counter = 0;
	char c;
	while (read(0, &c, 1) > 0)
	{
		if (c == '\n' || c == ' ')
		{
			if (toPlace == 1)
				command1[counter] = '\0';
			else
				command2[counter] = '\0';
			toPlace++;
			counter = 0;
		}
		else if (counter < 4) 
		{
			if (toPlace == 1)
				command1[counter++] = c;
			else
				command2[counter++] = c;
		}
		else if (counter == 4)
			errx(3, "all args must have <= 4 chars");
	
		if (toPlace == 3)
		{
			toPlace = 1;
			pid_t pid = fork();
			if (pid == -1)
				err(4, "could not fork");

			if (pid == 0)
			{
				if (execlp(command, command, command1, command2, NULL) == -1)
					err(5, "could not execute execlp");
			}

			if (wait(NULL) == -1)
				err(6, "could not execute wait");
		}
	}
	
	if (toPlace == 2)
	{
		pid_t pid = fork();
		if (pid == -1)
			err(4, "could not fork");

		if (pid == 0)
		{
			if (execlp(command, command, command1, NULL) == -1)
				err(5, "could not execute execlp");
		}

		if (wait(NULL) == -1)
			err(6, "could not execute wait");
	}

	exit(0);
}
