#include <time.h>
#include <err.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>

void writeToLogFile(char* filename, time_t startTime, time_t endTime, int exitCode)
{
	int fd = open(filename, O_CREAT | O_APPEND | O_WRONLY, S_IRUSR | S_IWUSR);
	if (fd == -1)
		err(3, "could not open file %s", filename);
	char buffer[sizeof(startTime) + sizeof(endTime) + sizeof(exitCode) + 4];
 	sprintf(buffer, "%ld %ld %d\n", startTime, endTime, exitCode);
	if (write(fd, buffer, sizeof(buffer)) != sizeof(buffer))
	{
		int olderrno = errno;
		close(fd);
		errno = olderrno;
		err(4, "could not write to file run.log");
	}
	close(fd);
}

int main (int argc, char** argv) 
{
	if (argc < 3) 
		errx(1, "usage: ./main [1-9] [program Q] (parameters for Q if needed)"); 

	char* secs = argv[1];
	if (*secs < '0' || *secs > '9')
		errx(2, "expected number in the range of [1-9]");	

	int unsuccessful = 0;
	while (unsuccessful != 2)
	{
		pid_t pid = fork();
		if (pid == -1)
			err(5, "could not fork");
	
		time_t startTime;
		if ((startTime=time(NULL)) == -1)
			err(6, "could not execute time");

		if (pid == 0)
		{
			if (execvp(argv[2], argv+2) == -1)
				err(7, "could not execute execvp");
		}

		int status;
		if (wait(&status) == -1)
			err(8, "error in wait");

		time_t exitTime;
		if ((exitTime=time(NULL)) == -1)
			err(6, "could not execute time");

		int exitCode = 129;
		if (WIFEXITED(status))
			exitCode = WEXITSTATUS(status);
		
		writeToLogFile("run.log", startTime, exitTime, exitCode);

		if (exitCode != 0 && exitTime - startTime < *secs - '0')
			unsuccessful++;
		else 
			unsuccessful--;
	}	

	exit(0);
}
