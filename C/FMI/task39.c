#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

void errHandler(int code, int errnum, int fd1, int fd2, int fd3, void* buff, const char* msg)
{
	if (fd1 != -1) close(fd1);
	if (fd2 != -1) close(fd2);
	if (fd3 != -1) close(fd3);
	free(buff);
	errno = errnum;
	err(code, "%s", msg);
}

int cmp(const void* a, const void* b)
{
	if (*(uint32_t*)a < *(uint32_t*)b)
		return -1;
	else if (*(uint32_t*)a == *(uint32_t*)b)
		return 0;
	else return 1;
}

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

	if (st.st_size % sizeof(uint32_t) != 0)
		errx(5, "%s does not consist only of uint32_t numbers", argv[1]);

	uint32_t numElements = st.st_size / sizeof(uint32_t);
	uint32_t leftSideCnt = numElements / 2;
	uint32_t rightSideCnt = numElements - leftSideCnt;

	int source = open(argv[1], O_RDONLY);
	if (source == -1)
		err(6, "Could not open file %s", argv[1]);

	int temp1 = open("temp1", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
	if (temp1 == -1)
		errHandler(7, errno, source, temp1, -1, NULL, "Could not open first temp file");

	uint32_t* buffer = malloc(leftSideCnt * sizeof(uint32_t));
	if (buffer == NULL)
		errHandler(8, errno, source, temp1, -1, NULL, "No space left to allocate");	

	size_t size = read(source, buffer, leftSideCnt * sizeof(uint32_t));
	if (size != leftSideCnt * sizeof(uint32_t))
		errHandler(9, errno, source, temp1, -1, buffer, "Error occurred while reading from soruce file");

	qsort(buffer, leftSideCnt, sizeof(uint32_t), cmp);

	size = write(temp1, buffer, leftSideCnt * sizeof(uint32_t));
	if (size != leftSideCnt * sizeof(uint32_t))
		errHandler(10, errno, source, temp1, -1, buffer, "Error occurred while writing to first temporary file");

	free(buffer);

	if (lseek(temp1, 0, SEEK_SET) == -1)
		errHandler(11, errno, source, temp1, -1, NULL, "Error occurred while lseek-ing first temporary file");

	buffer = malloc(rightSideCnt * sizeof(uint32_t));
	if (buffer == NULL)
		errHandler(12, errno, source, temp1, -1, NULL, "No space left to allocate");

	int temp2 = open("temp2", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
	if (temp2 == -1)
		errHandler(13, errno, source, temp1, temp2, buffer, "Could not open second temp file");

	size = read(source, buffer, rightSideCnt * sizeof(uint32_t));
	if (size != rightSideCnt * sizeof(uint32_t))
		errHandler(14, errno, source, temp1, temp2, buffer, "Error occurred while reading from source file");

	qsort(buffer, rightSideCnt, sizeof(uint32_t), cmp);

	size = write(temp2, buffer, rightSideCnt * sizeof(uint32_t));
	if (size != rightSideCnt * sizeof(uint32_t))
		errHandler(15, errno, source, temp1, temp2, buffer, "Error occurred while writing to second temp file");

	free(buffer);
	close(source);
	
	if (lseek(temp2, 0, SEEK_SET) == -1)
		errHandler(16, errno, -1, temp1, temp2, NULL, "Error occurred while lseek-ing second temp file");

	int dest = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
	if (dest == -1)
		errHandler(17, errno, dest, temp1, temp2, NULL, "Could not open destination file");

	ssize_t rd1, rd2;
	uint32_t left, right;
	while ((rd1=read(temp1, &left, sizeof(left))) > 0 && ((rd2=read(temp2, &right, sizeof(right))) > 0))
	{
		if (left < right)
		{
			if (write(dest, &left, sizeof(left)) != sizeof(left))
				errHandler(18, errno, dest, temp1, temp2, NULL, "Error occurred while writing to dest file");
	
			if (lseek(temp2, -1 * sizeof(uint32_t), SEEK_CUR) == -1)
				errHandler(19, errno, dest, temp1, temp2, NULL, "Error occurred while lseek-ing temp2 file");
		}
		else
		{
			if (write(dest, &right, sizeof(right)) != sizeof(left))
				errHandler(20, errno, dest, temp1, temp2, NULL, "Error occurred while writing to dest file");

			if (lseek(temp1, -1 * sizeof(uint32_t), SEEK_CUR) == -1)
				errHandler(21, errno, dest, temp1, temp2, NULL, "Error occurred while lseek-ing temp1 file");
		}
	}

	if (rd1 == sizeof(uint32_t))
	{
		if (write(dest, &left, sizeof(left)) > 0)
			errHandler(22, errno, dest, temp1, temp2, NULL, "Error occurred while writing to dest file");
	} 

	if (rd1 == -1 || rd2 == -1)
		errHandler(23, errno, dest, temp1, temp2, NULL, "Error occurred while reading from temp files");

	while ((rd1=read(temp1, &left, sizeof(left))) > 0)
	{
		if (write(dest, &left, sizeof(left)) != sizeof(left))
			errHandler(24, errno, dest, temp1, temp2, NULL, "Error occurred while writing to dest file");
	}

	while ((rd2=read(temp2, &right, sizeof(right))) > 0)
	{
		if (write(dest, &right, sizeof(right)) != sizeof(right))
			errHandler(25, errno, dest, temp1, temp2, NULL, "Error occurred while writing to dest file");
	}

	close(dest);
	close(temp1);
	close(temp2);

	if (unlink("temp1") == -1)
		err(26, "Could not unlink file temp1");
	
	if (unlink("temp2") == -1)
		err(27, "Could not unlink file temp2");
	exit(0);
}
