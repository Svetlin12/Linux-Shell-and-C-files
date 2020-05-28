#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>

void errorHandler(int code, int errnum, int fd)
{
	if (fd != -1)
		close(fd);
	errno = errnum;
	err(code, "error code: %d", code);
}

int main(int argc, char** argv)
{
	if (argc != 4)
		errx(1, "usage: ./main [command1] [command2] [filename]");

	int fileFd = open(argv[3], O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
	if (fileFd == -1)
		err(2, "error in opening file %s", argv[3]);

	pid_t pid = fork();
	if (pid == -1)
		errorHandler(3, errno, fileFd);		

	if (pid == 0)
	{
		if (execlp(argv[1], argv[1], 0) == -1)
			err(4, "error in executing command %s", argv[1]);
	}

	int status;
	if (wait(&status) == -1)
		errorHandler(5, errno, fileFd);	
	
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
	{
		ssize_t cmndSize = strlen(argv[1]);
		if (write(fileFd, argv[1], cmndSize) != cmndSize)
			errorHandler(6, errno, fileFd);	
	}
	else
		warnx("%s did not finish well", argv[1]);

	close(fileFd);
	if (execlp(argv[2], argv[2], 0) == -1)
		err(7, "error in executing command %s", argv[2]);
}
