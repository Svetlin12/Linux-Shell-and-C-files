#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <unistd.h>
#include <sys/types.h>    
#include <sys/stat.h>       
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

void errorHandler(int code, int errnum, int fd1, int fd2, int fd3)
{
	if (fd1 != -1)
		close(fd1);

	if (fd2 != -1)
		close(fd2);

	if (fd3 != -1)
		close(fd3);

	errno = errnum;
	err(code, "error code: %d", code);
}

int main(int argc, char** argv)
{
	if (argc != 4)
		errx(1, "usage: ./main.c filename1.bin filename2.bin filename3.bin");

	if (access(argv[1], F_OK) == -1)
		errx(2, "%s is not a file", argv[1]);

	if (access(argv[1], R_OK) == -1)
		errx(3, "file %s is not readable", argv[1]);

	if (access(argv[2], F_OK) == -1)
		errx(4, "%s is not a file", argv[2]);

	if (access(argv[2], R_OK) == -1)
		errx(5, "file %s is not readable", argv[2]);
	
	if (access(argv[3], F_OK) != -1 && access(argv[3], W_OK) == -1)
		errx(6, "file %s is not writable", argv[3]);

	int fd1 = open(argv[1], O_RDONLY);
	if (fd1 == -1)
	{
		int olderrno = errno;
		errorHandler(7, olderrno, fd1, -1, -1);
	}
	
	int fd2 = open(argv[2], O_RDONLY);
	if (fd2 == -1)
	{
		int olderrno = errno;
		errorHandler(8, olderrno, fd1, fd2, -1);
	}
	
	int fd3 = open(argv[3], O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
	if (fd3 == -1)
	{	
		int olderrno = errno;
		errorHandler(9, olderrno, fd1, fd2, fd3);
	}
	
	struct stat stFile1;
	struct stat stFile2;

	if (stat(argv[1], &stFile1) == -1)
	{
		int olderrno = errno;
		errorHandler(10, olderrno, fd1, fd2, fd3);
	}
	if (stat(argv[2], &stFile2) == -1)
	{
		int olderrno = errno;
		errorHandler(11, olderrno, fd1, fd2, fd3);
	}
	
	if (stFile1.st_size % sizeof(uint8_t) != 0)
	{
		int olderrno = errno;
		errorHandler(12, olderrno, fd1, fd2, fd3);
	}
	
	if (stFile2.st_size % sizeof(uint8_t) != 0)
	{
		int olderrno = errno;
		errorHandler(13, olderrno, fd1, fd2, fd3);
	}
	
	if (stFile1.st_size != stFile2.st_size)
		errx(14, "The two files are of different length");

	struct tuple {
		uint16_t offset;
		uint8_t originalByte;
		uint8_t newByte;
	};

	struct tuple t;

	ssize_t readSize1;
	ssize_t readSize2;
	uint8_t byte1;
	uint8_t byte2;
	uint16_t currPos = 0;
	while ((readSize1=read(fd1, &byte1, sizeof(byte1))) == sizeof(byte1) && (readSize2=read(fd2, &byte2, sizeof(byte2))) == sizeof(byte2))
	{
		if (byte1 != byte2)
		{
			t.offset = currPos;
			t.originalByte = byte1;
			t.newByte = byte2;
			if (write(fd3, &t, sizeof(t)) != sizeof(t))
			{
				int olderrno = errno;
				errorHandler(15, olderrno, fd1, fd2, fd3);
			}
		}

		currPos++;
	}	

	if (readSize1 == -1)
	{
		int olderrno = errno;
		errorHandler(16, olderrno, fd1, fd2, fd3);
	}
	if (readSize2 == -1)
	{
		int olderrno = errno;
		errorHandler(17, olderrno, fd1, fd2, fd3);
	}
	
	close(fd1);
	close(fd2);
	close(fd3);
	exit(0);
}
