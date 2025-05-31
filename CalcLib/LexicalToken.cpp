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
	return token;
}

TokenDescriptor CreateSingleCharacterToken(TokenType tokenType, char c)
{
	TokenDescriptor token;
	token.tokenType = tokenType;
	token.pTokenText = (char*)malloc(2);
	token.pTokenText[0] = c;
	token.pTokenText[1] = '\0';
	return token;
}

TokenDescriptor CreateStringToken(TokenType tokenType, char* pToken)
{
	TokenDescriptor token;
	token.tokenType = tokenType;
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
