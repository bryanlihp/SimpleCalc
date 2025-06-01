#pragma once
#include "LexState.h"
#include "LexicalToken.h"
struct LexTokenSink;

enum ResultType
{
	NUMBER,
	TEXT
};
struct TokenList
{
	TokenDescriptor* pTokens;
	int nTokens;
	int nCapacity;
};
void InitTokenList(TokenList* pTokenList);
void DestroyTokenList(TokenList* pTokenList);


struct LexResult
{
	ResultType resultType;
	TokenList operators;
	TokenList operands;
	union
	{
		int nValue;
		char* pResultText;
	}  resultValue;

};

typedef void (*pfnTokenSink)(TokenDescriptor token, bool bReset);
typedef void (*pfnLexResultBuilder)(LexResult* pResult, TokenDescriptor token, bool bReset);

struct LexTokenSink
{
	LexResult result;
	pfnLexResultBuilder OnToken;
};

bool InitTokenSink(LexTokenSink* pSink, ResultType resultType, pfnLexResultBuilder builder);
void DestroyTokenSink(LexTokenSink* pSink);

void InFixExpBuilder(LexResult* pResult, TokenDescriptor token, bool bReset);

//shunting-yard algorithm
void RpnExpBuilder(LexResult* pResult, TokenDescriptor token, bool bReset);

void EvalExpBuilder(LexResult* pResult, TokenDescriptor token, bool bReset);

struct LexerDescriptor
{
	const char* pOperators;
	LEXER_STATE lexState;
	bool bOprandPending;
	char* pTokenStart;
	int nPendingBrackets;
	int nTokens;
	bool bSucceeded;
	char* pErrorText;

	LexTokenSink** ppTokenSinks;
	int nSinks;

};


bool InitLexer(LexerDescriptor* pDesc);
void DestroyLexer(LexerDescriptor* pDesc);
int AddTokenSink(LexerDescriptor* pDesc, ResultType resultType, pfnLexResultBuilder builder);
bool Parse(LexerDescriptor* pDesc, char* pExpressionText);
