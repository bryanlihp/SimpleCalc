#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ConsoleIo.h"


void ConsoleOutput(const char* pText, bool bAddCR)
{
	printf("%s", pText);
	if (bAddCR)
	{
		printf("\n");
	}
}

char* ConsoleInput()
{
	char* pText = nullptr;
	char buf[1024];
	if (nullptr != fgets(buf, 1023, stdin))
	{
		size_t size = strlen(buf);
		if (buf[size - 1] == '\n')
		{
			size--;
		}
		if (size > 0)
		{
			pText = (char*)malloc(size + 1);
			memcpy(pText, buf, size);
			pText[size] = '\0';
		}
	}
	return pText;
}
