#include "pch.h"
#include "Tokenizer.h"

bool IsSpace(char ch)
{
	return ' ' == ch || '\t' == ch;
}

bool IsDigit(char c)
{
	return c >= '0' && c <= '9';
}

bool IsIdentifierStart(char c)
{
	return '_' == c || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool IsValidIdentifierChar(char c)
{
	return IsIdentifierStart(c) || IsDigit(c);
}

bool IsDelimeter(char ch, const char* pDelimeters)
{
	const char* pDelimeterChar = pDelimeters;
	while ('\0' != *pDelimeterChar && ch != *pDelimeterChar) pDelimeterChar++;
	return '\0' != *pDelimeterChar;
}

bool IsDelimeterOrSpace(char ch, const char* pDelimeters)
{
	return IsDelimeter(ch, pDelimeters) || IsSpace(ch);
}

char* GetToken(char* pInputText, const char* pDelimeters)
{
	static char* pTextToParse = nullptr;
	static char currentChar;
	if (nullptr != pInputText)
	{
		// reset the parsing text when input text is not null
		pTextToParse = pInputText;
		currentChar = *pTextToParse;
	}
	*pTextToParse = currentChar; // restore the current char (previously set to '\0' by the last call)

	// skip leading delimeters
	while ('\0' != currentChar && IsDelimeterOrSpace(currentChar, pDelimeters))
	{
		++pTextToParse;
		currentChar = *pTextToParse;
	}

	if ('\0' == *pTextToParse)
	{
		return nullptr; // bail out when reaching EOL
	}

	char* pToken = pTextToParse; // Token starts

	while ('\0' != currentChar && !IsDelimeterOrSpace(currentChar, pDelimeters))
	{
		++pTextToParse;
		currentChar = *pTextToParse;
	}
	*pTextToParse = '\0'; // Terminate the string, need to restore with currentChar when getting next token. 
	return pToken;
}


char* GetTokenFromExpression(char* pInputText, const char* pOperators)
{
	static char* pTextToParse = nullptr;
	static char currentChar;
	if (nullptr == pTextToParse)
	{
		// reset the parsing text when input text is not null
		pTextToParse = pInputText;
		currentChar = *pInputText;
	}
	*pTextToParse = currentChar; // restore the current char (previously set to '\0' by the last call)

	// skip leading spaces
	while (IsSpace(currentChar) && '\0' != currentChar)
	{
		++pTextToParse;
		currentChar = *pTextToParse;
	}

	if ('\0' == *pTextToParse)
	{
		return nullptr; // bail out when reaching EOL
	}

	char* pToken = pTextToParse; // Token starts
	if (IsDelimeterOrSpace(currentChar, pOperators) && '\0' != currentChar)
	{
		//move to the next character
		++pTextToParse;
		currentChar = *pTextToParse;
	}
	else
	{
		while ('\0' != currentChar && !IsDelimeterOrSpace(currentChar, pOperators)) {
			pTextToParse++;
			currentChar = *pTextToParse;
		}
	}
	*pTextToParse = '\0'; // Terminate the string, need to restore with currentChar when getting next token. 
	return pToken;
}

