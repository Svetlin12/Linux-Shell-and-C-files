#include <err.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

pid_t executeCommand(const char* cmd, int stdIn, int toClose, int toClose2)
{
	pid_t pid = fork();
	if (pid == -1)
		err(2, "Could not fork");
	
	if (pid == 0)
	{
		close(toClose);
		if (toClose2 != -1) close(toClose2);
		if (dup2(stdIn, 0) == -1)
			err(3, "Could not duplicate");
		
		if (execlp(cmd, cmd, (char*)0) == -1)
			err(4, "Could not execute command %s", cmd);
	}
	
	return pid;
}

int main(int argc, char** argv)
{
	if (argc != 3)
		errx(1, "Invalid arg count. Usage: %s cmd1 cmd2", argv[0]);

	const char* const cmd1 = argv[1];
	const char* const cmd2 = argv[2];

	int a[2];
	if (pipe(a) == -1)
		err(2, "Could not pipe");

	pid_t pid1 = executeCommand(cmd1, a[0], a[1], -1);
	close(a[0]);
	
	int b[2];
	if (pipe(b) == -1)
		err(5, "Could not pipe");	

	pid_t pid2 = executeCommand(cmd2, b[0], b[1], -1);
	close(b[0]);

	char buff[256];
	ssize_t readSize;
	int isFirstCommand = 1;
	while ((readSize=read(0, &buff, sizeof(buff))) > 0)
	{
		if (isFirstCommand)
		{
			if (write(a[1], &buff, readSize) != readSize)
				err(6, "Error occurred while writing to pipe");
		}
		else
		{
			if (write(b[1], &buff, readSize) != readSize)
				err(7, "Error occurred while writing to pipe");
		}
		
		if (isFirstCommand)
			isFirstCommand = 0;
		else
			isFirstCommand = 1;
	}
	
	if (readSize == -1)
		err(8, "Error occurred while reading from stdin");
	
	close(a[1]);
	close(b[1]);
	int status;
	if (waitpid(pid1, &status, 0) == -1)
		err(9, "Could not wait for child");

	if (!WIFEXITED(status))
		err(10, "Child did not terminate well");

	if (WEXITSTATUS(status) != 0)
		err(11, "Child did not exit with code 0");

	if (waitpid(pid2, &status, 0) == -1)
		err(12, "Could not wait for child");

	if (!WIFEXITED(status))
		err(13, "Child did not terminate well");

	if (WEXITSTATUS(status) != 0)
		err(14, "Child did not exit with code 0");

	exit(0);
}
