#pragma once

namespace Tokenizer
{
	bool IsSpace(char ch);
	bool IsDelimeter(char ch, const char *pDelimeters);
	bool IsDelimeterOrSpace(char ch, const char* pDelimeters);
	char* GetToken(char* pInputText, const char* pDelimeters);
	char* GetTokenFromExpression(char* pInputText, const char* pOperators);
}
