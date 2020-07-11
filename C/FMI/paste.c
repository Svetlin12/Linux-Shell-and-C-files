#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

void errHandler(int code, int errnum, int fd1, int fd2, const char* msg)
{
	if (fd1 != -1) close(fd1);
	if (fd2 != -1) close(fd2);
	errno = errnum;
	err(code, "%s", msg);
}

int main(int argc, char** argv)
{
	if (argc != 5 && argc != 3)
		errx(1, "Expected arugments. Usage: %s (-d delimiter) f1 f2", argv[0]);

	char* delimiter;
	int start = 1;
	if (strcmp(argv[1], "-d") == 0)
	{
		start = 3;
		delimiter = argv[2];
	}
	else
		delimiter = "\t"; 
	
	struct stat st;
	if (stat(argv[start], &st) == -1)
		err(2, "Could not stat %s", argv[start]);

	if (!(S_ISREG(st.st_mode)))
		errx(3, "%s is not a regular file", argv[start]);

	if (!(st.st_mode & S_IRUSR))
		errx(4, "%s is not readable", argv[start]);

	if (stat(argv[start + 1], &st) == -1)
		err(5, "Could not stat %s", argv[start + 1]);

	if (!(S_ISREG(st.st_mode)))
		errx(6, "%s is not a regular file", argv[start + 1]);

	if (!(st.st_mode & S_IRUSR))
		errx(7, "%s is not redable", argv[start + 1]);

	int fd1 = open(argv[start], O_RDONLY);
	if (fd1 == -1)
		errHandler(8, errno, fd1, -1, "Could not open first file");

	int fd2 = open(argv[start+1], O_RDONLY);
	if (fd2 == -1)
		errHandler(9, errno, fd1, fd2, "Could not open first file");

	while (1)
	{
		int idx = 0;
		char buff[1024], first[256], second[256], c;
		ssize_t readSize;
		while ((readSize=read(fd1, &c, 1)) > 0 && c != '\n')
			first[idx++] = c;
	
		if (readSize == -1)
			errHandler(10, errno, fd1, fd2, "Error occurred while reading from first file");

		first[idx] = 0;
		idx = 0;

		while ((readSize=read(fd2, &c, 1)) > 0 && c != '\n')
			second[idx++] = c;

		if (readSize == -1)
			errHandler(11, errno, fd1, fd2, "Error ocurred while reading from second file");

		second[idx] = 0;
		idx = 0;
		
		if (!strlen(first) && !strlen(second))
			break;

		sprintf(buff, "%s%s%s\n", first, delimiter, second); 	
		write(1, &buff, strlen(buff));
	}	

	close(fd1);
	close(fd2);
	exit(0);
}
