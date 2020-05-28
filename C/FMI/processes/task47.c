#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char** argv)
{
	if (argc != 2)
		errx(42, "usage: %s [dirname]", argv[0]);

	struct stat st;

	if (stat(argv[1], &st) == -1) 
		err(1, "could not stat");

	if (!S_ISDIR(st.st_mode))
		errx(2, "%s is not a directory", argv[1]);

	int a[2];
	if (pipe(a) == -1)
		err(3, "could not pipe");

	pid_t pid1 = fork();
	if (pid1 == -1)
		err(4, "could not fork");

	if (pid1 == 0)
	{
		close(a[0]);
		if (dup2(a[1], 1) == -1)
			err(5, "could not execute dup2");
		if (execlp("find", "find", argv[1], "-type", "f", "-printf", "%T@ %P\n", 0) == -1)
			err(6, "could not execute execlp");
	}

	close(a[1]);

	int b[2];
	if (pipe(b) == -1)
		err(3, "could not pipe");

	pid_t pid2 = fork();
	if (pid2 == -1)
		err(4, "could not fork");

	if (pid2 == 0)
	{
		if (dup2(a[0], 0) == -1)
			err(5, "could not execute dup2");
		close(b[0]);
		if (dup2(b[1], 1) == -1)
			err(5, "could not execute dup2");
		if (execlp("sort", "sort", "-rn", 0) == -1)
			err(6, "could not execute execlp");
	}
	
	close(b[1]);

	int c[2];
	if (pipe(c) == -1)
		err(3, "could not pipe");

	pid_t pid3 = fork();
	if (pid3 == -1)
		err(4, "could not fork");

	if (pid3 == 0)
	{
		if (dup2(b[0], 0) == -1)
			err(5, "could not execute dup2");
		close(c[0]);
		if (dup2(c[1], 1) == -1)
			err(5, "could not execute dup2");
		if (execlp("head", "head", "-1", 0) == -1)
			err(6, "could not execute execlp");
	}

	close(c[1]);

	if (dup2(c[0], 0) == -1)
		err(5, "could not execute dup2");
	if (execlp("cut", "cut", "-d", " ", "-f2", 0) == -1)
		err(6, "could not execute execlp");
}
