#include <sys/types.h>
#include <err.h>
#include <unistd.h>

int main(int argc, char** argv)
{
	if (argc != 2)
		errx(1, "usage: %s [filename]", argv[0]);

	if (access(argv[1], F_OK) == -1)
		errx(2, "%s is not a file", argv[1]);

	if (access(argv[1], R_OK) == -1)
		errx(3, "file %s is not readable", argv[1]);

	int a[2];
	if (pipe(a) == -1)
		err(4, "could not pipe");
		
	pid_t pid = fork();
	if (pid == -1)
		err(5, "could not fork");

	if (pid == 0)
	{
		close(a[0]);
		if (dup2(a[1], 1) == -1)
			err(6, "could not execute dup2");
		if (execlp("cat", "cat", argv[1], 0) == -1)
			err(7, "could not execute execlp");
	}

	close(a[1]);
	if (dup2(a[0], 0) == -1)
		err(6, "could not execute dup2");
	if (execlp("sort", "sort", 0) == -1)
		err(7, "could not execute execlp");
}
