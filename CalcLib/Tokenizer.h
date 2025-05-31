#pragma once
bool IsSpace(char ch);
bool IsDigit(char ch);
bool IsIdentifierStart(char c);
bool IsValidIdentifierChar(char c);
bool IsDelimeter(char ch, const char* pDelimeters);
bool IsDelimeterOrSpace(char ch, const char* pDelimeters);
char* GetToken(char* pInputText, const char* pDelimeters);
char* GetTokenFromExpression(char* pInputText, const char* pOperators);


