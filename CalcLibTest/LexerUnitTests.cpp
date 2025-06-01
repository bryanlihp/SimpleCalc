#include "pch.h"
#include "..\CalcLib\CalcLib.h"
namespace Lexer_Unit_Test
{
	void TokenSink(TokenDescriptor token, bool bFirstToken)
	{

	}

	void LexResultBuiler1(LexResult* pResult, TokenDescriptor token, bool bReset)
	{

	}
	void LexResultBuiler2(LexResult* pResult, TokenDescriptor token, bool bReset)
	{

	}
	void LexResultBuiler3(LexResult* pResult, TokenDescriptor token, bool bReset)
	{

	}
	
	TEST(Lexer_Parser, Can_Add_Multiple_Token_Sinks)
	{
		LexerDescriptor desc;
		InitLexer(&desc);

		ASSERT_GE(AddTokenSink(&desc, ResultType::NUMBER, LexResultBuiler1), 0);
		ASSERT_GE(AddTokenSink(&desc, ResultType::NUMBER, LexResultBuiler2), 0);
		ASSERT_GE(AddTokenSink(&desc, ResultType::TEXT, LexResultBuiler3), 0);
		ASSERT_EQ(3, desc.nSinks);
		DestroyLexer(&desc);
	}


	TEST(Lexer_Parser, Can_Parse_Simple_Expression) 
	{
		char pText[] = " 8 * (5 * (7 *6) )";
		LexerDescriptor desc;
		InitLexer(&desc);

		ASSERT_GE(AddTokenSink(&desc, ResultType::NUMBER, LexResultBuiler1),0);
		bool bSucceeded = Parse(&desc, pText);
		DestroyLexer(&desc);
		ASSERT_TRUE(bSucceeded);
	}

}