#include "pch.h"
#include <stdlib.h>
#include <stdio.h>
#include "Tokenizer.h"
#include "LexicalToken.h"
#include "Lexer.h"
#include <string.h>


char* ParseInput(LexerDescriptor* pDesc, char* pTextToParse, int* pPos);
void OnLexToken(LexerDescriptor* pDesc, TokenDescriptor token, int nPos);
void OnError(LexerDescriptor* pDesc, const char* pErrorText, int nPos);

char* ParseOperand(LexerDescriptor* pDesc, char* pTextToParse, int* pPos);
char* ParseValue(LexerDescriptor* pDesc, char* pTextToParse, int* pPos);
char* ParseIdentifier(LexerDescriptor* pDesc, char* pTextToParse, int* pPos);
char* ParseOperator(LexerDescriptor* pDesc, char* pTextToParse, int* pPos);


const char OPERATORS[] = "+-*/";
bool Construct(LexerDescriptor* pDesc)
{
	if (!pDesc)
	{
		return false;
	}
	pDesc->pOperators = OPERATORS;
	pDesc->pTokenSinks = nullptr;
	pDesc->nSinks = 0;
	pDesc->lexState = LEXER_STATE::OPERAND;
	pDesc->bOprandPending = false;
	pDesc->pTokenStart = nullptr;
	pDesc->nPendingBrackets = 0;
	pDesc->nTokens = 0;
	pDesc->bSucceeded = true;
	pDesc->pErrorText = nullptr;
	return true;
}

void Destruct(LexerDescriptor* pDesc)
{
	if (pDesc)
	{
		pDesc->pOperators = nullptr;
		free(pDesc->pTokenSinks);
		pDesc->pTokenSinks = nullptr;
		pDesc->nSinks = 0;
		free(pDesc->pErrorText);
		pDesc->pErrorText = nullptr;
	}
}

void AddTokenSink(LexerDescriptor* pDesc, pfnTokenSink tokenSink)
{
	if (pDesc)
	{
		pfnTokenSink* pNewSinks = (pfnTokenSink*)realloc(pDesc->pTokenSinks, sizeof(pfnTokenSink));
		if (nullptr != pNewSinks)
		{
			pNewSinks[pDesc->nSinks] = tokenSink;
			pDesc->nSinks++;
			pDesc->pTokenSinks = pNewSinks;
		}
		//else
		//{
		//	free(pDesc->pTokenSinks);
		//	pDesc->pTokenSinks = nullptr;
		//	pDesc->nSinks = 0;
		//}
	}
}

bool Parse(LexerDescriptor* pDesc, char* pExpressionText)
{
	if (!pDesc)
	{
		return false;
	}

	char* pText = pExpressionText;
	int nPos = 0;
	char* pTextToParse = pText;
	pDesc->lexState = LEXER_STATE::OPERAND;
	while (pTextToParse != nullptr)
	{
		//if ('\0' == *pTextToParse)
		//{
		//	TokenDescriptor token = CreateToken(TokenType::EOL);
		//	OnLexToken(pDesc, token, nPos);
		//	DestroyToken(token);
		//	break; // EOL marks the exd of parsing
		//}
		pTextToParse = ParseInput(pDesc, pTextToParse, &nPos);
	}
	return pDesc->bSucceeded;
}

char* ParseOperand(LexerDescriptor* pDesc, char* pTextToParse, int* pPos)
{
	char c = *pTextToParse;
	if (IsSpace(c))
	{
		// skipping leading spaces
		++pTextToParse;
		++(*pPos);
		return pTextToParse;
	};

	//valid characters to start: digits, +, -, (,
	char* pToken = pTextToParse;
	if ('(' == c) // found token "(", expecting an oprand
	{
		TokenDescriptor token = CreateSingleCharacterToken(TokenType::LPAR, c);
		OnLexToken(pDesc, token, *pPos);
		DestroyToken(token);
		++pTextToParse; // move to next character in the expression to continue parsing
		++(*pPos);
		return pTextToParse;
	}

	if (')' == c) // found Token ), next should be a oprator
	{
		if (pDesc->nPendingBrackets >0)
		{
			TokenDescriptor token = CreateSingleCharacterToken(TokenType::RPAR, c);
			OnLexToken(pDesc, token, *pPos);
			DestroyToken(token);
			++pTextToParse; // move to next character in the expression to continue parsing
			++(*pPos);
			pDesc->lexState = LEXER_STATE::OPERATOR;
			return pTextToParse;
		}
		else
		{
			OnError(pDesc, "no matching left bracket.", *pPos);
			return nullptr;
		}
	}

	if ('-' == c || '+' == c)
	{
		TokenDescriptor token = CreateSingleCharacterToken(TokenType::UOP, c);
		OnLexToken(pDesc, token, *pPos);
		DestroyToken(token);
		++pTextToParse; // move to next character in the expression to continue parsing
		++(*pPos);
		return pTextToParse;
	}

	if (IsDigit(c))
	{
		pDesc->lexState = LEXER_STATE::VALUE;
		pDesc->pTokenStart = pTextToParse;
		return pTextToParse;
	}

	if (IsIdentifierStart(c))
	{
		pDesc->lexState = LEXER_STATE::IDENTIFIER;
		pDesc->pTokenStart = pTextToParse;
		return pTextToParse;
	}

	if ('\0' == c)
	{
		TokenDescriptor token = CreateToken(TokenType::EOL);
		OnLexToken(pDesc, token, *pPos);
		DestroyToken(token);
		return nullptr;
	}

	OnError(pDesc, "unexpected character.", *pPos);
	return nullptr; // terminate parsing process
}
char* ParseValue(LexerDescriptor* pDesc, char* pTextToParse, int* pPos)
{
	char c = *pTextToParse;
	if (IsDigit(c))
	{
		pTextToParse++;
		++(*pPos);
		return pTextToParse; // continue paring number
	}
	if ('\0' == c || ')' == c || IsDelimeterOrSpace(c, pDesc->pOperators))
	{
		*pTextToParse = '\0';
		TokenDescriptor token = CreateStringToken(TokenType::VAL, pDesc->pTokenStart);
		OnLexToken(pDesc, token, *pPos);
		DestroyToken(token);
		*pTextToParse = c;
		pDesc->lexState = LEXER_STATE::OPERATOR;
		return pTextToParse;
	}

	OnError(pDesc, "Invalud number.", *pPos);
	return nullptr;
}

char* ParseIdentifier(LexerDescriptor* pDesc, char* pTextToParse, int* pPos)
{
	char c = *pTextToParse;
	if (IsValidIdentifierChar(c))
	{
		pTextToParse++;
		++(*pPos);
		return pTextToParse; // continue paring number
	}
	if ('\0' == c || ')' == c || IsDelimeterOrSpace(c, pDesc->pOperators))
	{
		*pTextToParse = '\0';
		TokenDescriptor token = CreateStringToken(TokenType::ID, pDesc->pTokenStart);
		OnLexToken(pDesc, token, *pPos);
		DestroyToken(token);
		*pTextToParse = c;
		pDesc->lexState = LEXER_STATE::OPERATOR;
		return pTextToParse;
	}
	OnError(pDesc, "Invalud identifier.", *pPos);
	return nullptr;
}

char* ParseOperator(LexerDescriptor* pDesc, char* pTextToParse, int* pPos)
{
	char c = *pTextToParse;
	if (IsSpace(c))
	{
		// skipping leading spaces
		++pTextToParse;
		++(*pPos);
		return pTextToParse;
	};

	if (IsDelimeter(c, pDesc->pOperators))
	{
		TokenDescriptor token = CreateSingleCharacterToken(TokenType::BOP, c);
		OnLexToken(pDesc, token, *pPos);
		DestroyToken(token);
		++pTextToParse;
		++(*pPos);
		pDesc->lexState = LEXER_STATE::OPERAND;
		return pTextToParse;
	}

	if (')' == c)
	{
		if (pDesc->nPendingBrackets>0)
		{
			TokenDescriptor token = CreateSingleCharacterToken(TokenType::RPAR, c);
			OnLexToken(pDesc, token, *pPos);
			++pTextToParse; // continue with the next character
			++(*pPos);
			return pTextToParse;
		}
		OnError(pDesc, "unexpected \")\"", *pPos);
		return nullptr;
	}

	if ('\0' == c)
	{
		TokenDescriptor token = CreateToken(TokenType::EOL);
		OnLexToken(pDesc, token, *pPos);
		return nullptr;
	}

	OnError(pDesc, "Invalid operator", *pPos);
	return nullptr;
}


char* ParseInput(LexerDescriptor* pDesc, char* pTextToParse, int* pPos)
{
	//if (!pDesc)
	//{
	//	return nullptr;
	//}
	char* pNewTextToParse = nullptr;
	switch (pDesc->lexState)
	{
	case LEXER_STATE::OPERAND:
		pNewTextToParse = ParseOperand(pDesc, pTextToParse, pPos);
		break;
	case LEXER_STATE::VALUE:
		pNewTextToParse = ParseValue(pDesc, pTextToParse, pPos);
		break;
	case LEXER_STATE::IDENTIFIER:
		pNewTextToParse = ParseIdentifier(pDesc, pTextToParse, pPos);
		break;
	case LEXER_STATE::OPERATOR:
		pNewTextToParse = ParseOperator(pDesc, pTextToParse, pPos);
		break;
	case LEXER_STATE::ERROR:
		//pNewTextToParse = ParseOperand();
		break;
	}
	return pNewTextToParse;
}

void OnLexToken(LexerDescriptor* pDesc, TokenDescriptor token, int nPos)
{
	//if(nullptr == pDesc) 
	//{
	//	return;
	//}
	bool bFirstToken = 0 == pDesc->nTokens;
	for (int i = 0; i < pDesc->nSinks; i++)
	{
		pDesc->pTokenSinks[i](token, bFirstToken);
	}
	pDesc->nTokens++;
	switch (token.tokenType)
	{
	case TokenType::VAL:
	case TokenType::ID:
		pDesc->bOprandPending = false;
		break;
	case TokenType::LPAR:
		pDesc->bOprandPending = true;
		pDesc->nPendingBrackets++;
		break;
	case TokenType::RPAR:
		pDesc->bOprandPending = false;
		pDesc->nPendingBrackets--;
		break;
	case TokenType::UOP:
	case TokenType::BOP:
		pDesc->bOprandPending = true;
		break;
	case TokenType::EOL:
		if (pDesc->nPendingBrackets > 0)
		{
			OnError(pDesc, "\")\" is expected.", nPos);
		}
		if (pDesc->bOprandPending)
		{
			OnError(pDesc, "missing oprand.", nPos);
		}
		break;
	default:
		break;
	}

}

void OnError(LexerDescriptor* pDesc, const char* pErrorText, int nPos)
{
	//if (!pDesc) 
	//{
	//	return; 
	//}
	char buffer[1024];
	pDesc->lexState = LEXER_STATE::ERROR;
	pDesc->bSucceeded = false;
	snprintf(buffer, 1024, "Parsing error: at %d, %s", nPos+1, pErrorText);
	int nSize = strlen(buffer);
	pDesc->pErrorText = (char*)malloc(nSize+1);
	memcpy(pDesc->pErrorText, buffer, nSize);
	pDesc->pErrorText[nSize] = '\0';
}

