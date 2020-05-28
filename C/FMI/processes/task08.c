#include <stdlib.h>
#include <err.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

void errorHandler(int code, int errnum, int fd)
{	
	if (fd != -1)
		close(fd);
	errno = errnum;
	err(code, "error code: %d", code);
}

int main(int argc, char** argv) 
{
	if (argc != 2)
		errx(1, "usage: %s [filename]", argv[0]);

	int fileFd = open(argv[1], O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
	if (fileFd == -1)
		err(2, "could not open file %s", argv[1]);

	pid_t pid = fork();
	if (pid == -1)
		errorHandler(3, errno, fileFd);

	if (pid == 0)
	{
		char foo[7] = "foobar\n";
		if (write(fileFd, &foo, 7) != 7)
			errorHandler(4, errno, fileFd);
		exit(0);
	}		

	if (wait(NULL) == -1)
		errorHandler(5, errno, fileFd);

	fileFd = open(argv[1], O_RDONLY);
	if (fileFd == -1)
		errorHandler(6, errno, fileFd);

	char c[2];
	ssize_t readSize;
	while ((readSize=read(fileFd, &c, 2)) > 0)
	{
		write(1, &c, readSize);
		write(1, " ", 1);	
	} 

	if (readSize == -1)
		errorHandler(7, errno, fileFd);	

	exit(0);
}
