#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <errno.h>
#include <err.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

void errorHandler(int code, int fd1, int fd2, int fd3, int errnum)
{
	if (fd1 != -1)
		close(fd1);

	if (fd2 != -1)
		close(fd2);

	if (fd3 != -1)
		close(fd3);

	errno = errnum;
	err(code, "error");
}

int main()
{
	struct pair {
	     uint32_t start;
	     uint32_t len;
	};

	struct stat st;
	if (stat("f1", &st) == -1)
		errorHandler(1, -1, -1, -1, errno);

	// check if f1 has even count of numbers - otherwise we would not be able to cycle through it correctly
	if (st.st_size % sizeof(struct pair) != 0)
		errx(2, "file f1 does not have even count of numbers");

	if (stat("f2", &st) == -1)
		errorHandler(3, -1, -1, -1, errno);

	// check if f2 has only uint32_t numbers
	if (st.st_size % sizeof(uint32_t) != 0)
		errx(3, "file f2 does not consist of only uint32_t numbers");

	int fd1 = open("f1", O_RDONLY);
	if (fd1 == -1)
		errorHandler(4, fd1, -1, -1, errno);

	int fd2 = open("f2", O_RDONLY);
	if (fd2 == -1)
		errorHandler(5, fd1, fd2, -1, errno);

	int fd3 = open("result", O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
	if (fd3 == -1)
		errorHandler(6, fd1, fd2, fd3, errno);

	struct pair p;

	while (read(fd1, &p, sizeof(p)) > 0)
	{
		off_t pos = lseek(fd2, (p.start - 1)*sizeof(uint32_t), SEEK_SET);
		if (pos == -1)
			errorHandler(7, fd1, fd2, fd3, errno);

		uint32_t num;
		uint32_t cnt = 0;
		while(read(fd2, &num, sizeof(num)) > 0)
		{
			if (write(fd3, &num, sizeof(num)) != sizeof(num))
				errorHandler(8, fd1, fd2, fd3, errno);

			cnt++;
			if (cnt == p.len)
				break;
					       	
		}
	}

	close(fd1);
	close(fd2);
	close(fd3);

	exit(0);
}
