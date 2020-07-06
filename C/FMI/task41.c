#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

int main(int argc, char** argv)
{
	if (argc != 3)
		errx(1, "Invalid argument count. Usage: %s (input.bin) (output.bin)", argv[0]);

	struct stat st;
	if (stat(argv[1], &st) == -1)
		err(2, "Could not stat %s", argv[1]);

	if (!(S_ISREG(st.st_mode)))
		errx(3, "%s is not a regular file", argv[1]);
	
	if (!(st.st_mode & S_IRUSR))
		errx(4, "%s is not readable", argv[1]);

	if (st.st_size % sizeof(uint16_t) != 0)
		errx(5, "%s does not contain only uint16_t numbers", argv[1]);

	uint32_t buffer[65536];
	for (int i = 0; i < 65536; i++)
		buffer[i] = 0;

	int source = open(argv[1], O_RDONLY);
	if (source == -1)
		err(6, "Could not open file %s", argv[1]);

	uint16_t num;
	ssize_t readSize;
	while ((readSize=read(source, &num, sizeof(num))) == sizeof(num))
		buffer[num]++;

	if (readSize == -1)
	{
		int olderrno = errno;
		close(source);
		errno = olderrno;
		err(7, "Error occurred while reading from file %s", argv[1]);
	}

	close(source);

	int dest = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
	if (dest == -1)
		err(8, "Could not open file %s", argv[2]);

	for (int i = 0; i < 65536; i++)
	{
		num = i;
		while (buffer[i]--)
		{
			if (write(dest, &num, sizeof(num)) != sizeof(num))
			{
				int olderrno = errno;
				close(source);
				errno = olderrno;
				err(9, "Error occurred while writing to file %s", argv[2]);
			}
		}
	}
	
	close(dest);
	exit(0);
}
