#include <err.h>
#include <errno.h>
#include <stdint.h>
#include <sys/types.h>       
#include <sys/stat.h>      
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void errorHandler(int code, int errnum, int fd1, int fd2, int fd3, int fd4, uint32_t* buffer)
{
	if (fd1 != -1)
		close(fd1);

	if (fd2 != -1)
		close(fd2);

	if (fd3 != -1)
		close(fd3);

	if (fd4 != -1)
		close(fd4);

	free(buffer);
	errno = errnum;
	err(code, "error code %d", code);
}

int cmp(const void* a, const void* b)
{
	if ((*(uint32_t*)a) < (*(uint32_t*)b))
		return -1;
	else if ((*(uint32_t*)a) == (*(uint32_t*)b))
		return 0;
	else 
		return 1;
}

int main(int argc, char** argv)
{
	if (argc != 3)
		errx(1, "usage: ./main input.bin output.bin");

	if (access(argv[1], F_OK) == -1)
		errx(2, "%s is not a file", argv[1]);

	if (access(argv[1], R_OK) == -1)
		errx(3, "file %s is not readable", argv[1]);

	if (access(argv[2], F_OK) != -1 && access(argv[2], W_OK) == -1)
		errx(4, "file %s is not writable", argv[2]);
	
	int input = open(argv[1], O_RDONLY);
	if (input == -1)
		errorHandler(5, errno, input, -1, -1, -1, NULL);

	int output = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
	if (output == -1)
		errorHandler(6, errno, input, output, -1, -1, NULL);

	int temp1 = open("temp1", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if (temp1 == -1)
		errorHandler(7, errno, input, output, temp1, -1, NULL);

	struct stat st;
	if (stat(argv[1], &st) == -1)
		errorHandler(7, errno, input, output, temp1, -1, NULL);

	if (st.st_size % sizeof(uint32_t) != 0)
		errx(8, "file %s does not consist of only uint32_t type numbers", argv[1]);

	uint32_t numElements = st.st_size / sizeof(uint32_t);
	uint32_t leftHalf = numElements / 2;
	uint32_t rightHalf = numElements - leftHalf;

	uint32_t* buffer = malloc(leftHalf * sizeof(uint32_t));
	if (buffer == NULL)
		errorHandler(9, errno, input, output, temp1, -1, NULL);

	size_t res = read(input, buffer, leftHalf * sizeof(uint32_t));
	if (res != leftHalf * sizeof(uint32_t))
		errorHandler(10, errno, input, output, temp1, -1, buffer);

	qsort(buffer, leftHalf, sizeof(uint32_t), cmp);
	res = write(temp1, buffer, leftHalf * sizeof(uint32_t));
	if (res != leftHalf * sizeof(uint32_t))
		errorHandler(11, errno, input, output, temp1, -1, buffer);
	
	free(buffer);	

	int temp2 = open("temp2", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if (temp2 == -1) 
		errorHandler(12, errno, input, output, temp1, temp2, NULL);
	
	buffer = malloc(rightHalf * sizeof(uint32_t));
	if (buffer == NULL)
		errorHandler(13, errno, input, output, temp1, temp2, NULL);
	
	res = read(input, buffer, rightHalf * sizeof(uint32_t));
       	if (res != rightHalf * sizeof(uint32_t))
		errorHandler(14, errno, input, output, temp1, temp2, buffer);

	qsort(buffer, rightHalf, sizeof(uint32_t), cmp);
	res = write(temp2, buffer, rightHalf * sizeof(uint32_t));
	if (res != rightHalf * sizeof(uint32_t))
		errorHandler(15, errno, input, output, temp1, temp2, buffer);
	
	close(input);
	free(buffer);

	lseek(temp1, 0, SEEK_SET);
	lseek(temp2, 0, SEEK_SET);

	uint32_t leftNum;
	uint32_t rightNum;

	if (read(temp1, &leftNum, sizeof(leftNum)) != sizeof(leftNum))
		errorHandler(16, errno, -1, output, temp1, temp2, NULL);

	if (read(temp2, &rightNum, sizeof(rightNum)) != sizeof(rightNum))
		errorHandler(17, errno, -1, output, temp1, temp2, NULL);

	uint32_t leftCnt = 0;
	uint32_t rightCnt = 0;
	ssize_t readSize;
	while (leftCnt < leftHalf && rightCnt < rightHalf)
	{
		if (leftNum < rightNum)
		{
			if (write(output, &leftNum, sizeof(leftNum)) != sizeof(leftNum))
				errorHandler(18, errno, -1, output, temp1, temp2, NULL);
			
			if ((readSize=read(temp1, &leftNum, sizeof(leftNum))) != sizeof(leftNum))
				break;

			leftCnt++;
		}
		else
		{
			if (write(output, &rightNum, sizeof(rightNum)) != sizeof(rightNum))
				errorHandler(19, errno, -1, output, temp1, temp2, NULL);
			
			if ((readSize=read(temp2, &rightNum, sizeof(rightNum))) != sizeof(rightNum))
			       break;

			rightCnt++;	
		}
			
	}

	if (readSize == -1)
		errorHandler(20, errno, -1, output, temp1, temp2, NULL);

	while ((readSize=read(temp1, &leftNum, sizeof(leftNum))) == sizeof(leftNum))
	{
		if (write(output, &leftNum, sizeof(leftNum)) != sizeof(leftNum))
			errorHandler(21, errno, -1, output, temp1, temp2, NULL);

	}	

	if (readSize == -1)
		errorHandler(22, errno, -1, output, temp1, temp2, NULL);

	while ((readSize=read(temp1, &rightNum, sizeof(rightNum))) == sizeof(rightNum))
	{
		if (write(output, &rightNum, sizeof(rightNum)) != sizeof(rightNum))
				errorHandler(23, errno, -1, output, temp1, temp2, NULL);
	}

	if (readSize == -1)
		errorHandler(24, errno, -1, output, temp1, temp2, NULL);

	close(output);
	close(temp1);
	close(temp2);
	exit(0);	
}
