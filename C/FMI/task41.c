#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <sys/types.h>       
#include <sys/stat.h>       
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv)
{
	if (argc != 3)
		errx(1, "usage: ./main.c input.bin output.bin");

	if (access(argv[1], F_OK | R_OK) == -1)
		errx(2, "%s is either not readable or isn't a file", argv[1]);

	if (access(argv[2], F_OK) != -1 && access(argv[2], W_OK)== -1)
		errx(3, "file %s is not writable", argv[2]);

	struct stat st;

	if (stat(argv[1], &st) == -1)
		err(4, "error: could not stat file %s", argv[1]);

	if (st.st_size % sizeof(uint16_t) != 0)
		errx(5, "error: file %s does not consist of only uint16_t numbers", argv[1]);

	int input = open(argv[1], O_RDONLY);
	if (input == -1)
		err(6, "error in opening file %s", argv[1]);

	int output = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);	
	if (output == -1)
	{
		int olderrno = errno;
		close(input);
		errno = olderrno;
		err(7, "error in opening file %s", argv[2]);
	}

	uint32_t maxSize = 0x10001;
	uint16_t container[maxSize];

	for (uint32_t i = 0; i < maxSize; i++)
		container[i] = 0;

	uint16_t num;
	ssize_t readSize;
	while ((readSize=(read(input, &num, sizeof(num)))) == sizeof(num))
		container[num]++;

	for (uint32_t i = 0; i < maxSize; i++)
	{
		while (container[i])
		{
			num = i;
			if (write(output, &num, sizeof(num)) != sizeof(num))
			{
				int olderrno = errno;
				close(input);
				close(output);
				errno = olderrno;
				err(8, "error: could not write to file %s", argv[2]);
			}
			container[i]--;
		}
	}

	if (readSize == -1)
	{
		int olderrno = errno;
		close(input);
		close(output);
		errno = olderrno;
		err(9, "error in reading from file %s", argv[1]);
	}

	close(input);
	close(output);
	exit(0);
}
