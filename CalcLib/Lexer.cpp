#include "pch.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Utility.h"
#include "Tokenizer.h"
#include "LexicalToken.h"
#include "Lexer.h"


char* ParseInput(LexerDescriptor* pDesc, char* pTextToParse, int* pPos);
void OnLexToken(LexerDescriptor* pDesc, TokenDescriptor token, int nPos);
void OnError(LexerDescriptor* pDesc, const char* pErrorText, int nPos);

char* ParseOperand(LexerDescriptor* pDesc, char* pTextToParse, int* pPos);
char* ParseValue(LexerDescriptor* pDesc, char* pTextToParse, int* pPos);
char* ParseIdentifier(LexerDescriptor* pDesc, char* pTextToParse, int* pPos);
char* ParseOperator(LexerDescriptor* pDesc, char* pTextToParse, int* pPos);

const char OPERATORS[] = "+-*/";


void InitTokenList(TokenList* pTokenList)
{
	pTokenList->nCapacity = 0;
	pTokenList->nTokens = 0;
	pTokenList->pTokens = nullptr;
}
void DestroyTokenList(TokenList* pTokenList)
{
	for (int i = 0; i < pTokenList->nTokens; i++)
	{
		DestroyToken(pTokenList->pTokens[i]);
	}
	free(pTokenList->pTokens);
	pTokenList->nCapacity = 0;
	pTokenList->nTokens = 0;
	pTokenList->pTokens = nullptr;
}

bool PushToken(TokenList* pTokenList, TokenDescriptor token)
{
	if (!pTokenList)
	{
		return false;
	}
	if (pTokenList->nTokens == pTokenList->nCapacity)
	{
		int nNewCapacity = pTokenList->nCapacity + 10;
		TokenDescriptor* pList = (TokenDescriptor*)realloc(pTokenList->pTokens, nNewCapacity * sizeof(TokenDescriptor));
		if (!pList)
		{
			return false;
		}
		pTokenList->nCapacity = nNewCapacity;
		pTokenList->pTokens = pList;
	}
	pTokenList->pTokens[pTokenList->nTokens] = CloneToken(token);
	pTokenList->nTokens++;
	return true;
}

void PopToken(TokenList* pTokenList)
{
	if (!pTokenList)
	{
		return;
	}
	if (pTokenList->nTokens >0)
	{
		DestroyToken(pTokenList->pTokens[pTokenList->nTokens-1]);
		pTokenList->nTokens--;
	}
}

TokenDescriptor* PeekTop(TokenList* pTokenList)
{
	if (!pTokenList || pTokenList->nTokens <=0)
	{
		return nullptr;
	}
	return &(pTokenList->pTokens[pTokenList->nTokens - 1]);
}

bool IsEmpty(TokenList* pTokenList)
{
	return 0==pTokenList->nTokens;
}

void AddTextToResult(LexResult* pResult, char* pText)
{
	if (pResult->resultValue.pResultText)
	{
		char* pNewText = Append(pResult->resultValue.pResultText, " ");
		if (pNewText)
		{
			pResult->resultValue.pResultText = pNewText;
		}
	}
	char* pNewText = Append(pResult->resultValue.pResultText, pText);
	if (pNewText)
	{
		pResult->resultValue.pResultText = pNewText;
	}
}

bool InitTokenSink(LexTokenSink* pSink, ResultType resultType, pfnLexResultBuilder builder)
{
	if (!pSink)
	{
		return false;
	}
	InitTokenList(&(pSink->result.operators));
	InitTokenList(&(pSink->result.operands));

	pSink->result.resultType = resultType;
	pSink->OnToken = builder;

	if (ResultType::NUMBER == resultType)
	{
		pSink->result.resultValue.nValue = 0;
	}
	else
	{
		pSink->result.resultValue.pResultText = nullptr;
	}
	return true;
}

void DestroyTokenSink(LexTokenSink* pSink)
{
	pSink->OnToken = nullptr;

	DestroyTokenList(&(pSink->result.operators));
	DestroyTokenList(&(pSink->result.operands));
	
	if (ResultType::NUMBER == pSink->result.resultType)
	{
		pSink->result.resultValue.nValue = 0;
	}
	else
	{
		free(pSink->result.resultValue.pResultText);
		pSink->result.resultValue.pResultText = nullptr;
	}

}


bool InitLexer(LexerDescriptor* pDesc)
{
	if (!pDesc)
	{
		return false;
	}
	pDesc->pOperators = OPERATORS;
	pDesc->lexState = LEXER_STATE::OPERAND;
	pDesc->bOprandPending = false;
	pDesc->pTokenStart = nullptr;
	pDesc->nPendingBrackets = 0;
	pDesc->nTokens = 0;
	pDesc->bSucceeded = true;
	pDesc->pErrorText = nullptr;
	pDesc->ppTokenSinks = nullptr;
	pDesc->nSinks = 0;
	return true;
}

void DestroyLexer(LexerDescriptor* pDesc)
{
	if (pDesc)
	{
		pDesc->pOperators = nullptr;

		for (int i = 0; i < pDesc->nSinks; i++)
		{
			DestroyTokenSink(pDesc->ppTokenSinks[i]);
			free(pDesc->ppTokenSinks[i]);
		}

		free(pDesc->ppTokenSinks);
		pDesc->ppTokenSinks = nullptr;
		pDesc->nSinks = 0;

		free(pDesc->pErrorText);
		pDesc->pErrorText = nullptr;

	}
}

int _AddTokenSink(LexerDescriptor* pDesc, LexTokenSink * pTokenSink)
{
	LexTokenSink** ppNewSinks = (LexTokenSink**)realloc(pDesc->ppTokenSinks, (pDesc->nSinks+1) * sizeof(LexTokenSink *));
	if (nullptr == ppNewSinks)
	{
		return -1;
	}
	int nIndex = pDesc->nSinks;
	ppNewSinks[nIndex] = pTokenSink;
	pDesc->nSinks++;
	pDesc->ppTokenSinks = ppNewSinks;
	return nIndex;
}

int AddTokenSink(LexerDescriptor* pDesc, ResultType resultType, pfnLexResultBuilder builder)
{
	if (!pDesc)
	{
		return -1;
	}
	LexTokenSink *pTokenSink = (LexTokenSink *)malloc(sizeof(LexTokenSink));
	if (!InitTokenSink(pTokenSink, resultType, builder))
	{
		return -1;
	}
	int nIndex = _AddTokenSink(pDesc, pTokenSink);
	if(nIndex<0)
	{
		DestroyTokenSink(pTokenSink);
		free(pTokenSink);
		pTokenSink = nullptr;
	}
	return nIndex;
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
		LexTokenSink *pSink = pDesc->ppTokenSinks[i];
		pSink->OnToken(&(pSink->result), token, bFirstToken);
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
	size_t nSize = strlen(buffer);
	pDesc->pErrorText = (char*)malloc(nSize+1);
	if (pDesc->pErrorText)
	{
		memcpy(pDesc->pErrorText, buffer, nSize);
		pDesc->pErrorText[nSize] = '\0';
	}
}


void InFixExpBuilder(LexResult* pResult, TokenDescriptor token, bool bReset)
{
	if (ResultType::TEXT != pResult->resultType)
	{
		return;
	}
	//if(TokenType::EOL!=token.tokenType)
	AddTextToResult(pResult, token.pTokenText);
}


void OnRpnOperand(LexResult* pResult, TokenDescriptor token)
{
	AddTextToResult(pResult, token.pTokenText);
}


void OnRpnLeftBracket(LexResult* pResult, TokenDescriptor token)
{
	PushToken(&(pResult->operators), token);
}

void OnRpnRightBracket(LexResult* pResult, TokenDescriptor token)
{
	TokenDescriptor* pToken = nullptr;
	do
	{
		pToken = PeekTop(&(pResult->operators));
		if (pToken)
		{
			if (TokenType::LPAR != pToken->tokenType)
			{
				AddTextToResult(pResult, pToken->pTokenText);
			}
			PopToken(&(pResult->operators));
		}
	} while (nullptr != pToken && TokenType::LPAR != pToken->tokenType);
}
void OnRpnOperator(LexResult* pResult, TokenDescriptor token)
{
	for (;;)
	{
		if (IsEmpty(&(pResult->operators)))
		{
			PushToken(&(pResult->operators), token);
			break;
		}
		TokenDescriptor* pTopToken = PeekTop(&(pResult->operators));
		if (TokenType::LPAR == pTopToken->tokenType)
		{
			PushToken(&(pResult->operators), token);
			break;
		}
		if (token.nPrecedence > pTopToken->nPrecedence)
		{
			PushToken(&(pResult->operators), token);
			break;
		}
		if (token.nPrecedence == pTopToken->nPrecedence && TokenType::UOP == token.tokenType)
		{
			PushToken(&(pResult->operators), token);
			break;
		}
		AddTextToResult(pResult, pTopToken->pTokenText);
		PopToken(&(pResult->operators));
	}
}
void OnRpnEol(LexResult* pResult, TokenDescriptor token)
{
	while (!IsEmpty(&(pResult->operators)))
	{
		TokenDescriptor* pTopToken = PeekTop(&(pResult->operators));
		AddTextToResult(pResult, pTopToken->pTokenText);
		PopToken(&(pResult->operators));
	}
}
//shunting-yard algorithm
void RpnExpBuilder(LexResult* pResult, TokenDescriptor token, bool bReset)
{
	switch (token.tokenType)
	{
	case TokenType::VAL:
	case TokenType::ID:
		OnRpnOperand(pResult, token);
		break;
	case TokenType::LPAR:
		OnRpnLeftBracket(pResult, token);
		break;
	case TokenType::RPAR:
		OnRpnRightBracket(pResult, token);
		break;
	case TokenType::UOP:
	case TokenType::BOP:
		OnRpnOperator(pResult, token);
		break;
	case TokenType::EOL:
		OnRpnEol(pResult, token);
		break;
	}
}


void BuildUnaryOpResult(LexResult* pResult, char op)
{
	if ('-' == op)
	{
		TokenDescriptor* pOprand = PeekTop(&(pResult->operands));
		int v = atoi(pOprand->pTokenText);
		PopToken(&(pResult->operands));
		v = 0 - v;
		char buf[32];
		snprintf(buf, 32, "%d", v);
		TokenDescriptor token = CreateStringToken(TokenType::VAL, buf);
		PushToken(&(pResult->operands), token);
	}
}

void BuildBinaryOpResult(LexResult* pResult, char op)
{
	TokenDescriptor *pOprand2 = PeekTop(&(pResult->operands));
	int v2 = atoi(pOprand2->pTokenText);
	PopToken(&(pResult->operands));
	TokenDescriptor* pOprand1 = PeekTop(&(pResult->operands));
	int v1 = atoi(pOprand1->pTokenText);
	PopToken(&(pResult->operands));

	int v = 0;
	switch (op)
	{
	case '+':
		v = v1 + v2;
		break;
	case '-':
		v = v1 - v2;
		break;
	case '*':
		v = v1 * v2;
		break;
	case '/':
		v = v1 / v2;
		break;
	//case '^':
	//	v = (int)pow(v1, v2);
	//	break;
	}
	char buf[32];
	snprintf(buf, 32, "%d", v);
	TokenDescriptor token = CreateStringToken(TokenType::VAL, buf);
	PushToken(&(pResult->operands), token);
}

void BuildResult(LexResult* pResult, TokenDescriptor token)
{
	if (TokenType::BOP == token.tokenType)
	{
		BuildBinaryOpResult(pResult, token.pTokenText[0]);
	}
	else
	{
		BuildUnaryOpResult(pResult, token.pTokenText[0]);
	}
}



void OnEvlOperand(LexResult* pResult, TokenDescriptor token)
{
	PushToken(&(pResult->operands), token);
}


void OnEvlLeftBracket(LexResult* pResult, TokenDescriptor token)
{
	PushToken(&(pResult->operators), token);
}

void OnEvlRightBracket(LexResult* pResult, TokenDescriptor token)
{
	TokenDescriptor* pTopToken = nullptr;
	do
	{
		pTopToken = PeekTop(&(pResult->operators));
		if (pTopToken)
		{
			if (TokenType::LPAR != pTopToken->tokenType)
			{
				BuildResult(pResult, *pTopToken);
			}
			PopToken(&(pResult->operators));
		}
	} while (nullptr != pTopToken && TokenType::LPAR != pTopToken->tokenType);
}

void OnEvlOperator(LexResult* pResult, TokenDescriptor token)
{
	for (;;)
	{
		if (IsEmpty(&(pResult->operators)))
		{
			PushToken(&(pResult->operators), token);
			break;
		}
		TokenDescriptor* pTopToken = PeekTop(&(pResult->operators));
		if (TokenType::LPAR == pTopToken->tokenType)
		{
			PushToken(&(pResult->operators), token);
			break;
		}
		if (token.nPrecedence > pTopToken->nPrecedence)
		{
			PushToken(&(pResult->operators), token);
			break;
		}
		if (token.nPrecedence == pTopToken->nPrecedence && TokenType::UOP == token.tokenType)
		{
			PushToken(&(pResult->operators), token);
			break;
		}
		BuildResult(pResult, *pTopToken);
		PopToken(&(pResult->operators));
	}

}
void OnEvlEol(LexResult* pResult, TokenDescriptor token)
{
	while (!IsEmpty(&(pResult->operators)))
	{
		TokenDescriptor* pTopToken = PeekTop(&(pResult->operators));
		BuildResult(pResult, *pTopToken);
		PopToken(&(pResult->operators));
	}

	TokenDescriptor* pResultToken = PeekTop(&(pResult->operands));
	int v2 = atoi(pResultToken->pTokenText);
	pResult->resultValue.nValue = v2;
	PopToken(&(pResult->operands));
}
void EvalExpBuilder(LexResult* pResult, TokenDescriptor token, bool bReset)
{
	switch (token.tokenType)
	{
	case TokenType::VAL:
	case TokenType::ID:
		OnEvlOperand(pResult, token);
		break;
	case TokenType::LPAR:
		OnEvlLeftBracket(pResult, token);
		break;
	case TokenType::RPAR:
		OnEvlRightBracket(pResult, token);
		break;
	case TokenType::UOP:
	case TokenType::BOP:
		OnEvlOperator(pResult, token);
		break;
	case TokenType::EOL:
		OnEvlEol(pResult, token);
		break;
	}
}

