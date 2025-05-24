// SimpleCalc.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "..\CalcLib\CalcLib.h"
#include "..\Alogrithms\Sort.h"
#include "ConsoleIo.h"
#include "EchoService.h"


void MergeSortDemo(int nElements)
{
	int* pElements = (int*)malloc(nElements * sizeof(int));

	BuildElements(pElements, nElements);
	printf("Original:\n");
	ShowElements(pElements, nElements);
	MergeSort(pElements, 0, nElements - 1);
	printf("Sorted:\n");
	ShowElements(pElements, nElements);

	free(pElements);
	pElements = NULL;
}



void TokenDemo()
{
	const char* pDelimeter = "abcde";
	Tokenizer::IsDelimeterOrSpace('b', pDelimeter);
	Tokenizer::IsSpace('c');
    std::cout << "Hello World!\n";
}


int main()
{
	//TokenDemo();
	//EchoService();
	MergeSortDemo(10);
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
