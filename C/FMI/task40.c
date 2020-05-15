#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int isCorrectDigit(char digit)
{
	if (digit >= '1' && digit <= '9')
		return 1;
	else
		return 0;
}

int main(int argc, char** argv)
{
	if (argc == 3 && strcmp(argv[1], "-c") == 0 && strlen(argv[2]) == 1)
	{
		char c;
		int number = (int)(*argv[2]) - '0';
		int cnt = 0;

		if (!isCorrectDigit(*argv[2]))
			errx(1, "usage: ./main -c [1-9]");

		while (read(0, &c, 1) > 0)
		{
			cnt++;

			if (c == '\n')
			{
				write(1, "\n", 1);
				cnt = 0;
			}
			else if (cnt == number)
				write(1, &c, sizeof(c));
		}
	}
	else if (argc == 3 && strcmp(argv[1], "-c") == 0 && strlen(argv[2]) == 3)
	{
		if (argv[2][0] > argv[2][2])
			errx(2, "expected interval: [1-9]-[1-9]");

		if (!isCorrectDigit(argv[2][0]) || !isCorrectDigit(argv[2][2]) || argv[2][1] != '-')
			errx(3, "usage: ./main -c [1-9]-[1-9]");

		char first = (int)argv[2][0] - '0';
		char last = (int)argv[2][2] - '0';
		char c;
		int cnt = 0;
		while (read(0, &c, 1) > 0)
		{
			cnt++;
			if (c == '\n')
			{
				write(1, &c, sizeof(c));
				cnt = 0;
			}
			else if (cnt >= first && cnt <= last)
				write(1, &c, sizeof(c));
		}
	}	

	else if (argc == 5 && strcmp(argv[1], "-d") == 0 && strlen(argv[4]) == 1)
	{
		if (strcmp(argv[3], "-f") != 0)
		       errx(4, "expected -f after delimiter");
	
		if (!isCorrectDigit(*argv[4]))	
			errx(5, "expected number of column");

		char delimiter = argv[2][0];

		int numColumn = (int)(*argv[4]) - '0';
		int cnt = 1;
		char c;
		while (read(0, &c, 1) > 0)
		{
			if (c == delimiter)
				cnt++;
		
			if (c == '\n')
			{
				write(1, "\n", 1);
				cnt = 1;
			}
			else if (cnt == numColumn && c != delimiter)
				write(1, &c, sizeof(c));
		}
	}
	else if (argc == 5 && strcmp(argv[1], "-d") == 0 && strlen(argv[4]) == 3)
	{
		if (strcmp(argv[3], "-f") != 0)
			errx(6, "expected -f after delimiter");
	
		if (!isCorrectDigit(argv[4][0]) || !isCorrectDigit(argv[4][2]) || argv[4][1] != '-')	
			errx(7, "incorrect interval");

		char delimiter = argv[2][0];

		int firstColumn = (int)argv[4][0] - '0';
		int lastColumn = (int)argv[4][2] - '0';

		if (firstColumn > lastColumn)
			errx(8, "start of interval is bigger than end");

		int cnt = 1;
		char c;
		while (read(0, &c, 1) > 0)
		{
			if (c == delimiter)
				cnt++;

			if (c == '\n')
			{
				write(1, "\n", 1);
				cnt = 1;
			}
			else if (cnt >= firstColumn && cnt <= lastColumn)
			{
				if (cnt == firstColumn && c == delimiter)
					continue;	
				write(1, &c, sizeof(c));
			}
		}
	}
	else
		errx(9, "usage: ./main -c [1-9]\n./main -c [1-9]-[1-9]\n./main -d [char] -f [1-9]\n./main -d [char] -f [1-9]-[1-9]");
	

	exit(0);
}
