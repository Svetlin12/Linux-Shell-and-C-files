#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void errHandler(int code, int errnum, int fd1, int fd2, int fd3, const char* msg)
{
	if (fd1 != -1) close(fd1);
	if (fd2 != -1) close(fd2);
	if (fd3 != -1) close(fd3);
	errno = errnum;
	err(code, "%s", msg);
}

int main(int argc, char** argv)
{
	if (argc != 4)
		errx(1, "Invalid argument count. Usage: %s (patch.bin) (f1.bin) (f2.bin)", argv[1]);

	struct tuple {
		uint16_t offset;
		uint8_t originalByte;
		uint8_t newByte;
	};

	struct stat st;
	if (stat(argv[1], &st) == -1)
		err(2, "Could not stat %s", argv[1]);

	if (!(S_ISREG(st.st_mode)))
		errx(3, "%s is not a regular file", argv[1]);

	if (!(st.st_mode & S_IRUSR))
		errx(4, "%s is not readable", argv[1]); 

	if (st.st_size % sizeof(struct tuple) != 0)
		errx(5, "%s does not comply to the patch.bin structure. It consists of (uint16_t) offset byte, (uint8_t) originalByte and (uint8_t) newByte", argv[1]);

	if (stat(argv[2], &st) == -1)
		err(6, "Could not stat %s", argv[2]);

	if (!(S_ISREG(st.st_mode)))
		errx(7, "%s is not a regular file", argv[2]);

	if (!(st.st_mode & S_IRUSR))
		errx(8, "%s is not readable", argv[2]);	

	int patch = open(argv[1], O_RDONLY);
	if (patch == -1)
		errHandler(9, errno, patch, -1, -1, "Could not open patch file");

	int source = open(argv[2], O_RDONLY);
	if (source == -1)
		errHandler(10, errno, patch, source, -1, "Could not open source binary file");
	
	int dest = open(argv[3], O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
	if (dest == -1)
		errHandler(11, errno, patch, source, dest, "Could not open destination binary file");

	struct tuple t;
	if (read(patch, &t, sizeof(t)) == -1)
		errHandler(12, errno, patch, source, dest, "Error occurred while reading from patch file");
	
	ssize_t readSize;
	uint8_t byte;
	uint16_t pos = 0;
	while ((readSize=read(source, &byte, sizeof(byte))) == sizeof(byte))
	{
		if (pos == t.offset && byte == t.originalByte)
		{
			if (write(dest, &t.newByte, sizeof(t.newByte)) != sizeof(t.newByte))
				errHandler(13, errno, patch, source, dest, "Error occurred while writing to destination file");
			if (read(patch, &t, sizeof(t)) == -1)
				errHandler(14, errno, patch, source, dest, "Error occured while reading from patch file");
		
			pos += 1;
			continue;
		}

		if (write(dest, &byte, sizeof(byte)) != sizeof(byte))
			errHandler(15, errno, patch, source, dest, "Error occurred while writing to destination file");
		
		pos += 1;
	}

	if (readSize == -1)
		errHandler(16, errno, patch, source, dest, "Error occured while reading from source file");
	
	close(patch);
	close(source);
	close(dest);
	exit(0);
}
