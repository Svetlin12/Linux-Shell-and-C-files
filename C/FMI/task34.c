#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h> 
#include <sys/stat.h>       
#include <fcntl.h>
#include <stdbool.h>

void errorHandler(int code, int errnum, int f1, int f2, int f3, int f4)
{
	if (f1 == -1)
		close(f1);

	if (f2 == -1)
		close(f2);
	
	if (f3 == -1)
		close(f3);

	if (f4 == -1)
		close(f4);

	errno = errnum;
	err(code, "error code: %d", code);
}

int main(int argc, char** argv)
{
	if (argc != 5)
		errx(1, "usage: ./main filename1.dat filename2.idx filename2.dat filename2.idx");

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

	if (access(argv[4], F_OK) != -1 && access(argv[4], W_OK) == -1)
		errx(7, "file %s is not writable", argv[4]);

	int f1 = open(argv[1], O_RDONLY);
	if (f1 == -1)
	{
		int olderrno = errno;
		errorHandler(8, olderrno, f1, -1, -1, -1);
	}

	struct stat st;
	if (stat(argv[1], &st) == -1)
	{
		int olderrno = errno;
		errorHandler(9, olderrno, f1, -1, -1, -1);
	}

	if (st.st_size % sizeof(uint8_t) != 0)
		errx(10, "file %s does not consist only of uint8_t type numbers", argv[1]);

	int f2 = open(argv[2], O_RDONLY);
	if (f2 == -1)
	{
		int olderrno = errno;
		errorHandler(11, olderrno, f1, f2, -1, -1);
	}

	int f3 = open(argv[3], O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
	if (f3 == -1)
	{
		int olderrno = errno;
		errorHandler(12, olderrno, f1, f2, f3, -1);
	}

	int f4 = open(argv[4], O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
	if (f4 == -1)
	{
		int olderrno = errno;
		errorHandler(13, olderrno, f1, f2, f3, f4);	
	}

	struct format
	{
		uint16_t offset;
		uint8_t len;
		uint8_t special;
	};

	struct format first;
	struct format second;
	
	if (stat(argv[2], &st) == -1)
	{
		int olderrno = errno;
		errorHandler(14, olderrno, f1, f2, f3, f4);
	}

	if (st.st_size % sizeof(first) != 0)
		errx(15, "file %s does not consist only of tripples (uint16_t, uint8_t, uint8_t)", argv[2]);

	ssize_t readSize;
	uint16_t currPos = 0;
	while ((readSize=read(f2, &first, sizeof(first))) == sizeof(first))
	{
		off_t location = lseek(f1, first.offset*sizeof(uint8_t), SEEK_SET);	
		if (location == -1)
		{
			int olderrno = errno;
			errorHandler(16, olderrno, f1, f2, f3, f4);
		}

		uint8_t num;
		ssize_t readBytes;
		uint8_t cnt = 0;
		bool isFirstByte = true;
		while ((readBytes=read(f1, &num, sizeof(num))) == sizeof(num))
		{
			if ((num < 'A' || num > 'Z') && isFirstByte)
				break;
			else
				isFirstByte = false;

			if (write(f3, &num, sizeof(num)) != sizeof(num))
			{
				int olderrno = errno;
			       	errorHandler(17, olderrno, f1, f2, f3, f4);
			}

			cnt++;
			if (cnt == first.len)
				break;		

		}

		if (readBytes == -1)
		{
			int olderrno = errno;
			errorHandler(18, olderrno, f1, f2, f3, f4);
		}

		second.special = 0;
		second.len = first.len;
		second.offset = currPos;
		if (!isFirstByte && write(f4, &second, sizeof(second)) != sizeof(second))
		{
			int olderrno = errno;
			errorHandler(19, olderrno, f1, f2, f3, f4);
		}

		currPos += first.len;
	}

	if (readSize == -1)
	{
		int olderrno = errno;
		errorHandler(20, olderrno, f1, f2,f3, f4);
	}

	close(f1);
	close(f2);
	close(f3);
	close(f4);
	exit(0);
}
