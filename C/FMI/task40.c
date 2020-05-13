#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	if (argc == 3 && strcmp(argv[1], "-c") == 0 && strlen(argv[2]) == 1)
	{
		char c;
		int number = (int)(*argv[2]) - '0';
		int cnt = 1;

		if (*argv[2] < '0' || *argv[2] > '9')
			errx(1, "usage: ./main -c [0-9]");

		while (read(0, &c, 1) > 0)
		{
			if (cnt == number || c == '\n')
				write(1, &c, sizeof(c));
			cnt++;
		}
	}
	else if (argc == 3 && strcmp(argv[1], "-c") == 0 && strlen(argv[2]) == 3)
	{
		if (argv[2][0] > argv[2][2])
			errx(2, "expected interval: [0-9]-[0-9]");

		if (argv[2][0] < '0' || argv[2][0] > '9' || argv[2][2] < '0' || argv[2][2] > '9' || argv[2][1] != '-')
			errx(3, "usage: ./main -c [0-9]-[0-9]");

		char first = (int)argv[2][0] - '0';
		char last = (int)argv[2][2] - '0';
		char c;
		int cnt = 1;
		while (read(0, &c, 1) > 0)
		{
			if ((cnt >= first && cnt <= last) || c == '\n')
				write(1, &c, sizeof(c));
			cnt++;
		}
	}	

	else if (argc == 5 && strcmp(argv[1], "-d") == 0 && strlen(argv[4]) == 1)
	{
		if (strcmp(argv[3], "-f") != 0)
		       errx(4, "expected -f after delimiter");
		
		if (*argv[4] < '0' && *argv[4] > '9')
			errx(5, "expected number of column");

		char delimiter = argv[2][0];

		int numColumn = (int)(*argv[4]) - '0';
		int cnt = 1;
		char c;
		while (read(0, &c, 1) > 0)
		{
			if (c == delimiter)
				cnt++;
			
			if (cnt == numColumn && c != '\n' && c != delimiter)
				write(1, &c, sizeof(c));
		}
		write(1, "\n", 1);
	}
	else if (argc == 5 && strcmp(argv[1], "-d") == 0 && strlen(argv[4]) == 3)
	{
		if (strcmp(argv[3], "-f") != 0)
			errx(6, "expected -f after delimiter");
		
		if (argv[4][0] < '0' || argv[4][0] > '9' || argv[4][2] < '0' || argv[4][2] > '9' || argv[4][1] != '-')
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
			{
				cnt++;
				if (cnt == firstColumn)
					continue;
			}

			if (cnt >= firstColumn && cnt <= lastColumn && c != '\n')
				write(1, &c, sizeof(c));
		}
		write(1, "\n", 1);
	}
	else
		errx(9, "usage: ./main -c [0-9]\n./main -c [0-9]-[0-9]\n./main -d [char] -f [0-9]\n./main -d [char] -f [0-9]-[0-9]");
	

	exit(0);
}
