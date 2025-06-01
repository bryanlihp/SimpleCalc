#pragma once
enum TokenType
{
	LPAR,
	RPAR,
	UOP,
	BOP,
	VAL,
	ID,
	EOL
};

struct TokenDescriptor
{
	TokenType tokenType;
	char* pTokenText;
	int nPrecedence;
};


void DestroyToken(TokenDescriptor token);
TokenDescriptor CreateToken(TokenType tokenType);
TokenDescriptor CreateSingleCharacterToken(TokenType tokenType, char c);
TokenDescriptor CreateStringToken(TokenType tokenType, char *pToken);
TokenDescriptor CloneToken(TokenDescriptor token);