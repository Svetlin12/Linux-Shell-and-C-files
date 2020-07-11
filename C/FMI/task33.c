#include <err.h>
#include <errno.h>
#include <stdint.h>
#include <sys/types.h>       
#include <sys/stat.h>      
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

void errorHandler(int code, int fd1, int fd2, int fd3, uint32_t* buffer, int errnum)
{
	if (fd1 != -1)
		close(fd1);
	if (fd2 != -1)
		close(fd2);
	if (fd3 != -1)
		close(fd3);

	free(buffer);
	errno = errnum;
	err(code, "error code: %d", code);
}

int cmp(const void* a, const void* b)
{
	if ((*(uint32_t*)a < *(uint32_t*)b))
		return -1;	
	else if ((*(uint32_t*)a == *(uint32_t*)b)) 
		return 0;
	else
		return 1;
}

int main(int argc, char** argv)
{
	if (argc != 2)
		errx(1, "usage: ./main (filename)");

	struct stat st;
	if (stat(argv[1], &st) == -1)
		err(2, "Could not stat file %s", argv[1]);

	if (!(S_ISREG(st.st_mode)))
		errx(3, "%s is not a regular file", argv[1]);

	if (!(st.st_mode & S_IRUSR))
		errx(4, "%s is not readable", argv[1]);

	if (!(st.st_mode & S_IWUSR))
		errx(5, "%s is not writable", argv[1]);
	
	if (st.st_size % sizeof(uint32_t) != 0)
		errx(6, "file does not have only uint32_t numbers");

	uint32_t numEl = st.st_size / sizeof(uint32_t);
	uint32_t lHalf = numEl / 2;
	uint32_t *left = malloc(lHalf * sizeof(uint32_t));
	if (left == NULL)
		errorHandler(7, -1, -1, -1, NULL, errno);

	int fd = open(argv[1], O_RDWR);
	if (fd == -1)
		errorHandler(8, fd, -1, -1, left, errno);

	int temp1 = open("temp1", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
	if (temp1 == -1)
		errorHandler(9, fd, temp1, -1, left, errno);

	size_t res = read(fd, left, lHalf*sizeof(uint32_t));
	if (res != lHalf*sizeof(uint32_t))
		errorHandler(10, fd, temp1, -1, left, errno);

	qsort(left, lHalf, sizeof(uint32_t), cmp);
	res = write(temp1, left, lHalf*sizeof(uint32_t));
	if (res != lHalf*sizeof(uint32_t))
		errorHandler(11, fd, temp1, -1, left, errno);

	free(left);

	int temp2 = open("temp2", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
	if (temp2 == -1)
		errorHandler(12, fd, temp1, temp2, NULL, errno);

	uint32_t rHalf = numEl - lHalf;
	uint32_t *right = malloc(rHalf * sizeof(uint32_t));
	if (right == NULL)
		errorHandler(13, fd, temp1, temp2, right, errno);

	res = read(fd, right, rHalf*sizeof(uint32_t));
	if (res != rHalf*sizeof(uint32_t))
		errorHandler(14, fd, temp1, temp2, right, errno);

	qsort(right, rHalf, sizeof(uint32_t), cmp);
	res = write(temp2, right, rHalf*sizeof(uint32_t));
	if (res != rHalf*sizeof(uint32_t))
		errorHandler(15, fd, temp1, temp2, right, errno);

	free(right);
	if (lseek(fd, 0, SEEK_SET) == -1)
		errorHandler(16, fd, temp1, temp2, NULL, errno);

	if (lseek(temp1, 0, SEEK_SET) == -1)
		errorHandler(17, fd, temp1, temp2, NULL, errno);

	if (lseek(temp2, 0, SEEK_SET) == -1)
		errorHandler(18, fd, temp1, temp2, NULL, errno);
	
	uint32_t leftNums[lHalf];
	uint32_t rightNums[rHalf];
	
	res = read(temp1, &leftNums, sizeof(leftNums));
	if (res != lHalf * sizeof(uint32_t))
		errorHandler(19, fd, temp1, temp2, NULL, errno);

	res = read(temp2, &rightNums, sizeof(rightNums));
	if (res != rHalf * sizeof(uint32_t))
		errorHandler(20, fd, temp1, temp2, NULL, errno);

	uint32_t leftCnt = 0, rightCnt = 0;	
	while (leftCnt < lHalf && rightCnt < rHalf)
	{
		if (leftNums[leftCnt] < rightNums[rightCnt])
		{
			if (write(fd, &leftNums[leftCnt], sizeof(leftNums[leftCnt])) != sizeof(leftNums[leftCnt]))
				errorHandler(21, fd, temp1, temp2, NULL, errno);
			leftCnt++;
		}
		else
		{
			if (write(fd, &rightNums[rightCnt], sizeof(rightNums[rightCnt])) != sizeof(rightNums[rightCnt]))
				errorHandler(22, fd, temp1, temp2, NULL, errno);
			rightCnt++;
		}
	}

	while (leftCnt < lHalf)
	{
		if (write(fd, &leftNums[leftCnt], sizeof(leftNums[leftCnt])) != sizeof(leftNums[leftCnt]))
			errorHandler(23, fd, temp1, temp2, NULL, errno);
		leftCnt++;
	}
	
	while (rightCnt < rHalf)
	{
		if (write(fd, &rightNums[rightCnt], sizeof(rightNums[rightCnt])) != sizeof(rightNums[rightCnt]))
			errorHandler(24, fd, temp1, temp2, NULL, errno);
		rightCnt++;
	}

	close(temp1);
	close(temp2);
	close(fd);

	if (unlink("temp1") == -1)
		err(25, "Could not unlink temp1");
	
	if (unlink("temp2") == -1)
		err(26, "Could not unlink temp2");

	exit(0);
}
