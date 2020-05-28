#include <err.h>
#include <unistd.h>
#include <sys/types.h>

int main() {

	int a[2];
	if (pipe(a) == -1)
		err(1, "could not pipe");

	pid_t pid = fork();	
	if (pid == -1)
		err(2, "could not fork");

	if (pid == 0)
	{
		close(a[0]);
		if (dup2(a[1], 1) == -1)
			err(3, "could not execute dup2");
		if (execlp("cut", "cut", "-d:", "-f7", "/etc/passwd", 0) == -1)
			err(4, "could not execute execlp");
	}

	close(a[1]);
	int b[2];
	if (pipe(b) == -1)
		err(1, "could not pipe");
	
	pid_t pid2 = fork();
	if (pid2 == -1)
		err(2, "could not fork");	

	if (pid2 == 0)
	{
		if (dup2(a[0], 0) == -1)
			err(3, "could not execute dup2");
		close(b[0]);
		if (dup2(b[1], 1) == -1)
			err(3, "could not execute dup2");
		if (execlp("sort", "sort", 0) == -1)
			err(4, "could not execute execlp");
	}

	close(b[1]);
	int c[2];
	if (pipe(c) == -1)
		err(1, "could not pipe");

	pid_t pid3 = fork();
	if (pid3 == -1)
		err(2, "could not fork");

	if (pid3 == 0)
	{
		if (dup2(b[0], 0) == -1)
			err(3, "could not execute dup2");
		close(c[0]);
		if (dup2(c[1], 1) == -1)
			err(3, "could not execute dup2");
		if (execlp("uniq", "uniq", "-c", 0) == -1)
			err(4, "could not execute dup2"); 
	}

	close(c[1]);
	if (dup2(c[0], 0) == -1)
		err(3, "could not execute dup2");

	if (execlp("sort", "sort", "-n", 0) == -1)
		err(4, "could not execute dup2");
}
