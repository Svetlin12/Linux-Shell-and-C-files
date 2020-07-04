#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int cnt = 1;

void readFrom(int fd, int isOptionActivated)
{
	int buffSize = 256, isNewline = 1;
	if (isOptionActivated == 1)
		buffSize = 1;
	char buff[buffSize];
	ssize_t readSize;
	while ((readSize = read(fd, &buff, sizeof(buff))) > 0)
	{
		if (isOptionActivated && isNewline)
		{
			setbuf(stdout, NULL);
			fprintf(stdout, "%d ", cnt);
			isNewline = 0;
			cnt++;
		}

		if (buff[0] == '\n')
			isNewline = 1;

		write(1, &buff, readSize);
	}

	if (readSize == -1)
	{
		int olderrno = errno;
		close(fd);
		errno = olderrno;
		err(3, "Error occured while reading from file");
	}
}

int main(int argc, char** argv)
{
	if (argc == 1)
		readFrom(0, 0);
	else if (argc == 2 && (strcmp(argv[1], "-n") == 0))
		readFrom(0, 1);
	else
	{
		int isOptionActivated = 0, i = 1;
		if (strcmp(argv[1], "-n") == 0)
		{
			isOptionActivated = 1;
			i++;
		}

		for (; i < argc; i++)
		{
			if (strcmp(argv[i], "-") == 0)
			{
				readFrom(0, isOptionActivated);
				continue;
			}
	
			struct stat st;
		
			if (stat(argv[i], &st) == -1)
				err(1, "Could not stat file %s", argv[i]);

			if (!(S_ISREG(st.st_mode)))
				errx(2, "%s is not a regular file", argv[i]);

			if (!(st.st_mode & S_IRUSR))
				errx(3, "%s is not readable", argv[i]);

			int fd = open(argv[i], O_RDONLY);
			if (fd == -1)
				err(4, "Could not open file %s", argv[i]);

			readFrom(fd, isOptionActivated);
			close(fd);
		}
	}

	exit(0);
}
