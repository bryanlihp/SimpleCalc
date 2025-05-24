#include "pch.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

void BuildElements(int* pElements, int nSize)
{
	static bool bInit = false;
	if (!bInit)
	{
		srand((unsigned int)time(NULL));
		bInit = true;
	}
	auto n = rand();
	for (int i = 0; i < nSize; i++)
	{
		pElements[i] = rand() * 100 / RAND_MAX;
	}
}
void ShowElements(int* pElements, int nSize)
{
	for (int i = 0; i < nSize; i++)
	{
		printf("Element[%d]=%d\n", i, pElements[i]);
	}
}
