#include "pch.h"
#include "..\CalcLib\CalcLib.h"
namespace Lexer_Unit_Test
{
	void TokenSink(TokenDescriptor token, bool bFirstToken)
	{

	}
	
	TEST(Lexer_Parser, Can_Add_Multiple_Token_Sinks) 
	{
		LexerDescriptor desc;
		Construct(&desc);
		AddTokenSink(&desc, TokenSink);
		AddTokenSink(&desc, TokenSink);
		AddTokenSink(&desc, TokenSink);
		ASSERT_EQ(3, desc.nSinks);
		Destruct(&desc);
	}
	
	TEST(Lexer_Parser, Can_Parse_Simple_Expression) 
	{
		char pText[] = " 8 * (5 * (7 *6) )";
		LexerDescriptor desc;
		Construct(&desc);
		AddTokenSink(&desc, TokenSink);
		bool bSucceeded = Parse(&desc, pText);
		Destruct(&desc);
		ASSERT_TRUE(bSucceeded);
	}

}