#pragma once
#include "LexState.h"
#include "LexicalToken.h"
struct LexTokenSink;

typedef void (*pfnTokenSink)(TokenDescriptor token, bool bReset);
typedef void (*pfnLexResultBuilder)(LexTokenSink* pSink, TokenDescriptor token, bool bReset);

struct LexTokenSink
{
	union
	{
		int nValue;
		char* pResultText;
	}  result;
	pfnTokenSink OnToken;
};

struct LexerDescriptor
{
	const char* pOperators;
	pfnTokenSink* pTokenSinks;
	int nSinks;
	LEXER_STATE lexState;
	bool bOprandPending;
	char* pTokenStart;
	int nPendingBrackets;
	int nTokens;
	bool bSucceeded;
	char* pErrorText;
};


bool Construct(LexerDescriptor* pDesc);
void Destruct(LexerDescriptor* pDesc);
void AddTokenSink(LexerDescriptor* pDesc, pfnTokenSink tokenSink);
bool Parse(LexerDescriptor* pDesc, char* pExpressionText);
