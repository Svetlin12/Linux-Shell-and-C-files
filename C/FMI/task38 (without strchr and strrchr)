#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <stdio.h>

char* findLastOccurrence(char* str, const char c)
{
	char* s = str;
	s += strlen(str) - 1;
	while (1)
	{
		if (*s == c)
			return s;

		if (s == str)
			break;
		s--;
	}
	return NULL;
}

int main(int argc, char** argv)
{
	if (argc == 3 && strcmp(argv[1], "-d") == 0)
	{
		char c;
		while (read(0, &c, 1) > 0)
		{
			if (findLastOccurrence(argv[2], c) == 0)
				write(1, &c, 1);		
		}
	}
	else if (argc == 3 && strcmp(argv[1], "-s") == 0)
	{
		char c, prev = 0;
		while (read(0, &c, 1) > 0)
		{
			if ((findLastOccurrence(argv[2], c) && c != prev) || (!findLastOccurrence(argv[2], c)))
				write(1, &c, 1);
			prev = c;
		}
	}
	else if (argc == 3)
	{
		if (strlen(argv[1]) != strlen(argv[2]))
			errx(1, "[SET1] and [SET2] have inequal length");

		char c;
		while (read(0, &c, 1) > 0)
		{
			char* occurrence = findLastOccurrence(argv[1], c);
			if (occurrence)
			{	
				// determine where the character was found in the first SET
				int at = strlen(argv[2]) - strlen(occurrence);
				write(1, &argv[2][at], 1);
			}
			else
				write(1, &c, 1);
		} 
	}
	else
		errx(2, "Invalid argument count. Usage:\n%s -s [SET]\n%s -d [SET]\n%s [SET1] [SET2]\n", argv[0], argv[0], argv[0]);

	exit(0);
}
