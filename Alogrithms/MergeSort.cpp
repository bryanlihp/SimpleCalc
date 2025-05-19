#include "pch.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


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

void Merge(int* pElements, int nStart, int nMid, int nEnd)
{
	int i = nStart, j = nMid + 1;
	int nLeftElements = nMid - nStart + 1;
	int nRightElements = nEnd - nMid;
	while (nLeftElements > 0 && nRightElements > 0)
	{
		int nLeft = pElements[i];
		int nRight = pElements[j];
		if (nLeft > nRight)
		{
			for (int k = 0; k < nLeftElements; k++)
			{
				pElements[j - k] = pElements[j - k - 1];
			}
			pElements[i] = nRight;
			j++;
			nRightElements--;
		}
		else
		{
			nLeftElements--;
		}
		i++;
	}
}

void MergeSortCore(int* pElements, int nStart, int nEnd)
{
	if (nStart >= nEnd) return; // 0 or 1 element, no need to sort

	int nMid = nStart + (nEnd - nStart) / 2;
	MergeSortCore(pElements, nStart, nMid);
	MergeSortCore(pElements, nMid + 1, nEnd);
	Merge(pElements, nStart, nMid, nEnd);
}

void MergeSortDemo(int nElements)
{
	int* pElements = (int*)malloc(nElements * sizeof(int));

	BuildElements(pElements, nElements);
	printf("Original:\n");
	ShowElements(pElements, nElements);
	MergeSortCore(pElements, 0, nElements - 1);
	printf("Sorted:\n");
	ShowElements(pElements, nElements);

	free(pElements);
	pElements = NULL;
}

