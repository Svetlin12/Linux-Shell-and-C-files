#include <err.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int isInvalidDigit(char digit)
{
	if (digit < '1' || digit > '9')
		return 1;
	else return 0;
}

int isValidInterval(char start, char end)
{
	if (start > end)
		return 0;
	else return 1;
}

int isValidSeparator(char separator)
{
	if (separator != '-')
		return 0;
	else return 1;
}

int getDigit(char digit)
{
	return digit - '0';
}

int main(int argc, char** argv)
{
	if (argc == 3 && strcmp(argv[1], "-c") == 0)
	{
		int start, end;
		if (strlen(argv[2]) == 3)
		{
			if (isInvalidDigit(argv[2][0]))
				errx(1, "Invalid start of interval");
	
			if (isInvalidDigit(argv[2][2]))
				errx(2, "Invalid end of interval");
		
			if (!isValidInterval(argv[2][0], argv[2][2]))
				errx(3, "Start of interval is bigger than end of interval");

			if (!isValidSeparator(argv[2][1]))
				errx(4, "Invalid separator for the interval");
		
			start = getDigit(argv[2][0]);
			end = getDigit(argv[2][2]);
		}
		else if (strlen(argv[2]) == 1)
		{
			if (isInvalidDigit(argv[2][0]))
				errx(5, "Invalid number. Valid numbers are 1-9.");
		
			start = getDigit(argv[2][0]);
			end = getDigit(argv[2][0]);
		}
		else
			errx(6, "Invalid interval after option -c. Usage: %s -c [1-9]\n%s -c [1-9]-[1-9]", argv[0], argv[0]);
		
		char c;
		int cnt = 1;
		while (read(0, &c, 1) > 0)
		{
			if ((cnt >= start && cnt <= end) || c == '\n')
				write(1, &c, 1);
			cnt++;
		}
	}
	else if (argc == 5 && strcmp(argv[1], "-d") == 0)
	{
		char delimiter = argv[2][0];
		if (strcmp(argv[3], "-f") != 0)
			errx(7, "Expected -f as third argument");

		int start, end;
		if (strlen(argv[4]) == 3)
		{
			if (isInvalidDigit(argv[4][0]))
				errx(8, "Invalid start of interval");

			if (isInvalidDigit(argv[4][2]))
				errx(9, "Invalid end of interval");

			if (!isValidInterval(argv[4][0], argv[4][2]))
				errx(10, "Start of interval is bigger than end of interval");

			if (!isValidSeparator(argv[4][1]))
				errx(11, "Invalid separator for the interval");

			start = getDigit(argv[4][0]);
			end = getDigit(argv[4][2]);
		}
		else if (strlen(argv[4]) == 1)
		{
			if (isInvalidDigit(argv[4][0]))
				errx(12, "Invalid number. Valid numbers are 1-9.");

			start = getDigit(argv[4][0]);
			end = getDigit(argv[4][0]);			
		}
		else 
			errx(13, "Invalid fourth argument. Usage: %s -d (delimiter) -f [1-9]\n%s -d (delimiter) -f [1-9]-[1-9]", argv[0], argv[0]);

		char c;
		int cnt = 1;
		while (read(0, &c, 1) > 0)
		{
			if (start == end && cnt == start)
			{
				if (c != delimiter)
					write(1, &c, 1);
				else
					cnt++;
				
				if (c == '\n')
					cnt = 1;
				
				continue;
			}
		
			if (cnt == end && c == delimiter)
			{
				cnt++;
				if (c == '\n')
					cnt = 1;
				continue;
			}

			if ((cnt >= start && cnt <= end) || c == '\n')
				write(1, &c, 1);

			if (c == delimiter)
				cnt++;
	
			if (c == '\n')
				cnt = 1;
		}
	}
	else
		errx(14, "Invalid argument count. Usage: %s -c (interval or a single number)\n%s -d (delimiter) -f (interval or a single number)", argv[0], argv[0]);


	exit(0);
}
