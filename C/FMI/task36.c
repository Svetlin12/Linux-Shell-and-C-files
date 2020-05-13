#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

int cnt = 1;

void readFromFd(bool isOptionActivated, int fd) 
{
	char c;
	bool isStart = true;
	while (read(fd, &c, 1) > 0)
	{
		if (isOptionActivated && isStart)
		{
			setbuf(stdout, NULL);
			fprintf(stdout, "%d ", cnt);
			isStart = false;
		}
		
		write(1, &c, sizeof(c));

		if (c == '\n')
		{
			cnt++;
			isStart = true;
		}	
	}
}

int main(int argc, char** argv)
{

	int i = 1;
	bool isOptionActivated = false;
	
	if (argc > 1 && strcmp(argv[1],"-n") == 0)
	{
		isOptionActivated = true;
		i = 2;
	}
	else if (argc == 1) 
	{
		readFromFd(isOptionActivated, 0);
	}

	for (; i < argc; i++)
	{
		if (strcmp(argv[i], "-") == 0)
		{
			readFromFd(isOptionActivated, 0);
			continue;
		}	

		if (access(argv[i], F_OK) == -1)
		{
			printf("%s is not a file\n", argv[i]);
			continue;
		}

		if (access(argv[i], R_OK) == -1)
		{
			printf("file %s is not readable\n", argv[i]);
			continue;
		}

		int fd = open(argv[i], O_RDONLY);
		if (fd == -1)
		{
			int olderrno = errno;
			close(fd);
			errno = olderrno;
			err(1, "could not open file %s", argv[i]);
		}

		readFromFd(isOptionActivated, fd);

		close(fd);
	}

	exit(0);
}
