#include <err.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	if (argc != 2)
		errx(1, "Invalid argument count. Usage: %s (filename)", argv[0]);

	int fileFd = open(argv[1], O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
	if (fileFd == -1)
		err(2, "Could not open file %s", argv[1]);

	pid_t pid = fork();
	if (pid == -1)
	{
		int olderrno = errno;
		close(fileFd);
		errno = olderrno;
		err(3, "Could not fork");
	}

	if (pid == 0)
	{
		if (write(fileFd, "foobar", 6) == -1)
		{
			int olderrno = errno;
			close(fileFd);
			errno = olderrno;
			err(4, "Error occurred while writing to file %s", argv[1]);
		}
		
		close(fileFd);	
		exit(0);
	}
	
	int status;
	if (wait(&status) == -1)
		err(5, "Error occurred while waiting for child process to finish");

	if (!WIFEXITED(status) || WEXITSTATUS(status))
		err(6, "Child did not end successfully");

	close(fileFd);
	fileFd = open(argv[1], O_RDONLY);

	char buff[2];
	ssize_t readSize;
	while ((readSize=read(fileFd, &buff, sizeof(buff))) > 0)
	{
		write(1, &buff, readSize);
		write(1, " ", 1);
	}
	
	if (readSize == -1)
	{
		int olderrno = errno;
		close(fileFd);
		errno = olderrno;
		err(7, "Error occurred while reading from file %s", argv[1]);
	}

	write(1, "\n", 1);
	
	close(fileFd);
	exit(0);
}
