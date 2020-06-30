#include <stdint.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>       
#include <sys/stat.h>       
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char** argv)
{
	if (argc != 2)
		errx(1, "expected name of file as argument");

	if (access(argv[1], F_OK) == -1)
		errx(2, "%s is not a file", argv[1]);

	if (access(argv[1], R_OK) == -1)
		errx(3, "file %s is not readable", argv[1]);

	if (access(argv[1], W_OK) == -1)
		errx(4, "file %s is not writable", argv[1]);

	int fd = open(argv[1], O_RDWR);
	if (fd == -1)
		err(5, "could not open file %s", argv[1]);

	uint32_t counting[256];

	for (int i = 0; i < 256; i++)
		counting[i] = 0;

	uint8_t num;
	ssize_t readSize;
	while ((readSize=read(fd, &num, sizeof(num))) == sizeof(num))
		counting[num]++;

	if (readSize == -1)
	{
		int olderrno = errno;
		close(fd);
		errno = olderrno;
		err(6, "failed at reading file %s", argv[1]);
	}

	lseek(fd, 0, SEEK_SET);

	for (int i = 0; i < 256; i++)
	{
		uint8_t num = i;
		while (counting[i])
		{
			if (write(fd, &num, sizeof(num)) != sizeof(num))
			{
				int olderrno = errno;
				close(fd);
				errno = olderrno;
				err(7, "failed writing to file %s", argv[1]);
			}
			counting[i]--;
		}
	}

	close(fd);
	exit(0);
}
