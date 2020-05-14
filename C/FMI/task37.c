#include <unistd.h>
#include <stdint.h>
#include <err.h>
#include <errno.h>
#include <sys/types.h>      
#include <sys/stat.h>       
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

void errorHandler(int code, int errnum, int fd1, int fd2, int fd3)
{
	if (fd1 == -1)
		close(fd1);

	if (fd2 == -1)
		close(fd2);

	if (fd3 == -1)
		close(fd3);

	errno = errnum;
	err(code, "error code: %d", code);
}

int main(int argc, char** argv)
{
	if (argc != 4)
		errx(1, "usage: ./main filename1.bin filename2.bin filename3.bin");

	if (access(argv[1], F_OK) == -1)
		errx(2, "%s is not a file", argv[1]);

	if (access(argv[1], R_OK) == -1)
		errx(3, "file %s is not readable", argv[1]);

	if (access(argv[2], F_OK) == -1)
		errx(4, "%s is not file", argv[2]);

	if (access(argv[2], R_OK) == -1)
		errx(5, "file %s is not readable", argv[2]);
	
	if (access(argv[3], F_OK) != -1 && access(argv[3], W_OK) == -1)
		errx(6, "file %s is not writable", argv[3]);

	int patch = open(argv[1], O_RDONLY);
	if (patch == -1)
		errorHandler(7, errno, patch, -1, -1);

	int f1 = open(argv[2], O_RDONLY);
	if (f1 == -1)
		errorHandler(8, errno, patch, f1, -1);

	int f2 = open(argv[3], O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
	if (f2 == -1)
		errorHandler(9, errno, patch, f1, f2);

	struct tuple {
		uint16_t offset;
		uint8_t originalByte;
		uint8_t newByte;
	};

	struct tuple t;

	struct stat st;
	if (stat(argv[1], &st) == -1)
		errorHandler(10, errno, patch, f1, f2);

	if (st.st_size % sizeof(t) != 0)
		errx(11, "file %s does not consist of only tuples", argv[1]);
	
	if (stat(argv[2], &st) == -1)
		errorHandler(12, errno, patch, f1, f2);

	if (st.st_size % sizeof(uint8_t))
		errx(13, "file %s does not consist of only uint8_t types numbers", argv[2]);

	uint8_t* buffer = malloc(st.st_size*sizeof(uint8_t));
	if (buffer == NULL)
		errorHandler(14, errno, patch, f1, f2);

	size_t readSz = read(f1, buffer, st.st_size*sizeof(uint8_t));	
	if (readSz != st.st_size*sizeof(uint8_t))
		errorHandler(15, errno, patch, f1, f2);

	size_t writeSz = write(f2, buffer, st.st_size*sizeof(uint8_t));
	if (writeSz != st.st_size*sizeof(uint8_t))
		errorHandler(16, errno, patch, f1, f2);

	free(buffer);
	
	ssize_t readSize = 0;
	while ((readSize=read(patch, &t, sizeof(t))) == sizeof(t))
	{
		off_t location1 = lseek(f1, t.offset*sizeof(uint8_t), SEEK_SET);
		if (location1 == -1)
			errorHandler(17, errno, patch, f1, f2);

		off_t location2 = lseek(f2, t.offset*sizeof(uint8_t), SEEK_SET);
		if (location2 == -1)
			errorHandler(18, errno, patch, f1, f2);

		uint8_t num;
		if (read(f1, &num, sizeof(num)) != sizeof(num))
			errorHandler(19, errno, patch, f1, f2);
		
		if (num != t.originalByte || write(f2, &t.newByte, sizeof(t.newByte)) != sizeof(t.newByte))
			errorHandler(20, errno, patch, f1, f2);
	}	

	if (readSize == -1)
		errorHandler(21, errno, patch, f1, f2);

	close(patch);
	close(f1);
	close(f2);
	exit(0);
}
