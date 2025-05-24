#include "pch.h"
#include <stdlib.h>
#include <stdio.h>
#include "Sort.h"



void Merge(int* pElements, int nStart, int nMid, int nEnd)
{
	int nLeftElements = nMid - nStart + 1;
	int nRightElements = nEnd - nMid;

	int i = nStart, j = nMid + 1;
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

void MergeSort(int* pElements, int nStart, int nEnd)
{
	if (nStart >= nEnd) return; // 0 or 1 element, no need to sort

	int nMid = nStart + (nEnd - nStart) / 2;
	MergeSort(pElements, nStart, nMid);
	MergeSort(pElements, nMid + 1, nEnd);
	Merge(pElements, nStart, nMid, nEnd);
}


