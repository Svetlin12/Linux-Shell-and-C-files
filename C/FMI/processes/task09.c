#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

void errorHandler(int code, int errnum, int fd)
{
	if (fd != -1)
		close(fd);
	errno = errnum;
	err(code, "error code:%d", code); 
}

int main()
{
	// create file "foobar.txt"
	int foo = open("foobar.txt", O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
	if (foo == -1)
		errorHandler(1, errno, foo);

	// assure that child processes can open an existing file
	close(foo);

	pid_t pid1 = fork();
	if (pid1 == -1)
		errorHandler(2, errno, foo);

	if (pid1 == 0)
	{
		int fd = open("foobar.txt", O_APPEND | O_WRONLY);
		if (fd == -1)
			errorHandler(3, errno, fd);
		if (write(fd, "foo", 3) != 3)
			errorHandler(4, errno, fd);
		exit(0);
	}

	// add this line if first child process is to complete it's work before the second one
	// if (wait(NULL) == -1)
	// 	errorHandler(5, errno, -1);

	pid_t pid2 = fork();
	if (pid2 == -1)
		errorHandler(5, errno, foo);

	if (pid2 == 0)
	{
		int fd = open("foobar.txt", O_APPEND | O_WRONLY);
		if (fd == -1)
			errorHandler(6, errno, fd);
		if (write(fd, "bar", 3) != 3)
			errorHandler(7, errno, fd);
		exit(0);
	}
	
	if (wait(NULL) == -1)
		errorHandler(8, errno, -1);
	exit(0);
}
