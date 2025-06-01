#include "pch.h"
#include <stdlib.h>
#include "LexicalToken.h"
#include <string.h>

void DestroyToken(TokenDescriptor token)
{
	token.tokenType = EOL;
	if (token.pTokenText)
	{
		free(token.pTokenText);
		token.pTokenText = nullptr;
	}
}

TokenDescriptor CreateToken(TokenType tokenType)
{
	TokenDescriptor token;
	token.tokenType = tokenType;
	token.pTokenText = nullptr;
	token.nPrecedence = -1;
	return token;
}

TokenDescriptor CreateSingleCharacterToken(TokenType tokenType, char c)
{
	TokenDescriptor token;
	token.tokenType = tokenType;
	token.pTokenText = (char*)malloc(2);
	token.pTokenText[0] = c;
	token.pTokenText[1] = '\0';
	token.nPrecedence = -1;
	switch (tokenType)
	{
	case TokenType::LPAR:
		token.nPrecedence = 0;
		break;
	case TokenType::RPAR:
		token.nPrecedence = 10;
		break;
	case TokenType::UOP:
		token.nPrecedence = 5;
		break;
	case TokenType::BOP:
		switch (c)
		{
		case '+':
		case '-':
			token.nPrecedence = 1;
			break;
		case '*':
		case '/':
			token.nPrecedence = 2;
		}
		break;
	}
	return token;
}

TokenDescriptor CreateStringToken(TokenType tokenType, char* pToken)
{
	TokenDescriptor token;
	token.tokenType = tokenType;
	token.nPrecedence = -1;
	if (pToken)
	{
		int nSize = (int)strlen(pToken);
		token.pTokenText = (char*)malloc(nSize + 1);
		if (token.pTokenText)
		{
			memcpy(token.pTokenText, pToken, nSize);
			token.pTokenText[nSize] = '\0';
		}
	}
	else
	{
		token.pTokenText = nullptr;
	}
	return token;
}

TokenDescriptor CloneToken(TokenDescriptor token)
{
	TokenDescriptor newToken;
	newToken.tokenType = token.tokenType;
	newToken.nPrecedence = token.nPrecedence;
	if (token.pTokenText)
	{
		size_t size = strlen(token.pTokenText);
		newToken.pTokenText = (char*)malloc((size + 1) * sizeof(char));
		memcpy(newToken.pTokenText, token.pTokenText, size);
		newToken.pTokenText[size] = '\0';
	}
	else
	{
		newToken.pTokenText = nullptr;
	}
	return newToken;
}
