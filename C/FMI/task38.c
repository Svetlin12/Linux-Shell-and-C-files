#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	if (argc == 3 && strcmp(argv[1], "-d") == 0)
	{
		char c;
		while (read(0, &c, 1) > 0)
		{
			if (strchr(argv[2], c) == NULL)
				printf("%c", c);		
		}
	}
	else if (argc == 3 && strcmp(argv[1], "-s") == 0)
	{
		char c;
		char prevChar = 0;
		bool isFirst = true;
		while (read(0, &c, 1) > 0)
		{
			if (c != prevChar)
				isFirst = true;
			if (strchr(argv[2], c) != NULL && isFirst)
			{
				printf("%c", c);
				isFirst = false;
			}
			else if (strchr(argv[2], c) == NULL)
			       printf("%c", c);	
			prevChar = c;
		}
	}
	else if (argc == 3)
	{
		ssize_t sizeSet1 = strlen(argv[1]);
		ssize_t sizeSet2 = strlen(argv[2]);
		if (sizeSet1 != sizeSet2)
			errx(1, "%s and %s are not of equal length", argv[1], argv[2]);

		char c;
		while (read(0, &c, 1) > 0)
		{
			char* at = strrchr(argv[1], c);
			if (at != NULL)
			{
				int index = (int)(at - argv[1]);
				printf("%c", argv[2][index]);
			}
			else 
				printf("%c", c);
		}
	}
	else	
		errx(2, "usage: ./main [OPTION] SET1 [SET2]");

	exit(0);
}
