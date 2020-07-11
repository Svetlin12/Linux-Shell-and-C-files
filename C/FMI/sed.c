#include <err.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char** argv)
{
	if (argc < 3 || argc > 5)
		errx(1, "Invalid argument count. Usage: %s (-s | -d) str1 str2 (-g)", argv[0]); 	

	if (argc >= 3 && strcmp(argv[1], "-d") == 0)
	{
		if (argc ==4 && strcmp(argv[3], "-g"))
			errx(2, "Expected -g as last argument");

		int noGlobal = 0;
		if (argc == 3)
			noGlobal = 1;

		char* toDelete = argv[2];
		ssize_t readSize, wordSize = strlen(toDelete);
		char buff[256], str[256];
		int isNewLine = 1;
		while ((readSize=read(0, &buff, sizeof(buff))) > 0)
		{
			for (int i = 0; i < readSize;)
			{
				if (buff[i] == '\n')
				{
					write(1, "\n", 1);
					i++;
					isNewLine = 1;
					continue;
				}

				strncpy(str, buff + i, wordSize);
				str[wordSize] = 0;
				if (strcmp(str, toDelete) != 0)
				{
					if (i + wordSize >= readSize)
					{
						write(1, &str, strlen(str));
						i+=wordSize;
					}
					else
					{
						if (str[0] != '\n')
							write(1, &str, 1);
						i++;
					}
				}
				else 
				{
					i += wordSize;
					if (noGlobal && !isNewLine)
						write(1, &str, strlen(str));
					else if (noGlobal && isNewLine)
						isNewLine = 0;
				}
			}
		}

	}
	else if (argc > 3 && strcmp(argv[1], "-s") == 0)
	{
		int noGlobal = 1;
		if (argc == 5 && strcmp(argv[4], "-g") == 0)
			noGlobal = 0;
		else if (argc == 5 && strcmp(argv[4], "-g"))
			errx(3, "Expected -g as last argument");

		char* toSubstitute = argv[2];
		char* substWith = argv[3];
		ssize_t readSize, substLen = strlen(toSubstitute);
		char str[256], buff[256];
		int isNewLine = 1;
		while ((readSize=read(0, &buff, sizeof(buff))) > 0)
		{
			for (int i = 0; i < readSize;)
			{
				if (buff[i] == '\n')
				{
					write(1, "\n", 1);
					i++;
					isNewLine = 1;
					continue;
				}

				strncpy(str, buff + i, substLen);
				str[substLen] = 0;
				if (strcmp(str, toSubstitute) != 0)
				{
					if (i + substLen >= readSize)
					{
						write(1, &str, strlen(str));
						i+=substLen;
					}
					else
					{
						if (str[0] != '\n')
							write(1, &str, 1);
						i++;
					}
				}
				else
				{
					if ((noGlobal && isNewLine) || !noGlobal)
					{
						write(1, substWith, strlen(substWith));
						isNewLine = 0;
					}
					else if (noGlobal && !isNewLine)
						write(1, &str, strlen(str));
					i+=substLen;	
				}	
			}
		}
	}
	else
		errx(4, "Invalid arguments. Usage: %s -s str1 str2 (-g)\n%s -d str (-g)", argv[0], argv[0]);

	exit(0);
}
