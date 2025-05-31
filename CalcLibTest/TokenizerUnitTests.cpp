#include "pch.h"
#include "..\CalcLib\CalcLib.h"
namespace Tokenizer_Unit_Test
{
	TEST(Tokenizer_isDelimiterOrSpace, Return_True_If_Char_Is_Delimeter) 
	{
		const char* pDelimeter = "abcde";
		EXPECT_TRUE(IsDelimeterOrSpace('b', pDelimeter));
	}

	TEST(Tokenizer_isDelimiterOrSpace, Return_False_If_Char_Is_Not_Delimeter)
	{
		const char* pDelimeter = "abcde";
		EXPECT_FALSE(IsDelimeterOrSpace('8', pDelimeter));
	}

	TEST(Tokenizer_isDelimiterOrSpace, Return_True_If_Char_Is_Tab)
	{
		const char* pDelimeter = "abcde";
		EXPECT_TRUE(IsDelimeterOrSpace('\t', pDelimeter));
	}

	TEST(Tokenizer_isDelimiterOrSpace, Return_True_If_Char_Is_Space)
	{
		const char* pDelimeter = "abcde";
		EXPECT_TRUE(IsDelimeterOrSpace(' ', pDelimeter));
	}

	TEST(Tokenizer_GetToken, Can_Get_original_string_if_Delimeter_is_Empty) {
		char pText[] = "The+quick-brown-fox-jumps-over-the-lazy-dog"; //space characters are always treated as delimeters
		const char pDelimeter[2] = "";
		char* pToken = GetToken(pText, pDelimeter);
		ASSERT_STREQ("The+quick-brown-fox-jumps-over-the-lazy-dog", pToken);
		ASSERT_EQ(nullptr, GetToken(nullptr, " "));
	}


	TEST(Tokenizer_GetToken, Can_Get_Tokens_From_string) {
		char pText[] = "The*quick^brown - fox^jumps ^ over* the lazy*dog";
		const char pDelimeter[] = " *^"; // space characters are always treated as delimeters
		char* pToken = GetToken(pText, pDelimeter);
		ASSERT_STREQ("The", pToken);
		ASSERT_STREQ("quick", GetToken(nullptr, pDelimeter));
		ASSERT_STREQ("brown", GetToken(nullptr, pDelimeter));
		ASSERT_STREQ("-", GetToken(nullptr, pDelimeter));
		ASSERT_STREQ("fox", GetToken(nullptr, pDelimeter));
		ASSERT_STREQ("jumps", GetToken(nullptr, pDelimeter));
		ASSERT_STREQ("over", GetToken(nullptr, pDelimeter));
		ASSERT_STREQ("the", GetToken(nullptr, pDelimeter));
		ASSERT_STREQ("lazy", GetToken(nullptr, pDelimeter));
		ASSERT_STREQ("dog", GetToken(nullptr, pDelimeter));
		ASSERT_EQ(nullptr, GetToken(nullptr, pDelimeter));
	}

	TEST(Tokenizer_GetTokenFromExperssion, Can_Get_Tokens_From_Experssion) {
		char pText[] = "The+quick+brown - fox+jumps*over+the   (lazy   ++++ dog) ";
		const char pOperators[] = "+-*/()";
		char* pToken = GetTokenFromExpression(pText, pOperators);
		ASSERT_STREQ("The", pToken);
		ASSERT_STREQ("+", GetTokenFromExpression(nullptr, pOperators));
		ASSERT_STREQ("quick", GetTokenFromExpression(nullptr, pOperators));
		ASSERT_STREQ("+", GetTokenFromExpression(nullptr, pOperators));
		ASSERT_STREQ("brown", GetTokenFromExpression(nullptr, pOperators));
		ASSERT_STREQ("-", GetTokenFromExpression(nullptr, pOperators));
		ASSERT_STREQ("fox", GetTokenFromExpression(nullptr, pOperators));
		ASSERT_STREQ("+", GetTokenFromExpression(nullptr, pOperators));
		ASSERT_STREQ("jumps", GetTokenFromExpression(nullptr, pOperators));
		ASSERT_STREQ("*", GetTokenFromExpression(nullptr, pOperators));
		ASSERT_STREQ("over", GetTokenFromExpression(nullptr, pOperators));
		ASSERT_STREQ("+", GetTokenFromExpression(nullptr, pOperators));
		ASSERT_STREQ("the", GetTokenFromExpression(nullptr, pOperators));
		ASSERT_STREQ("(", GetTokenFromExpression(nullptr, pOperators));
		ASSERT_STREQ("lazy", GetTokenFromExpression(nullptr, pOperators));
		ASSERT_STREQ("+", GetTokenFromExpression(nullptr, pOperators));
		ASSERT_STREQ("+", GetTokenFromExpression(nullptr, pOperators));
		ASSERT_STREQ("+", GetTokenFromExpression(nullptr, pOperators));
		ASSERT_STREQ("+", GetTokenFromExpression(nullptr, pOperators));
		ASSERT_STREQ("dog", GetTokenFromExpression(nullptr, pOperators));
		ASSERT_STREQ(")", GetTokenFromExpression(nullptr, pOperators));
		ASSERT_EQ(nullptr, GetTokenFromExpression(nullptr, pOperators));
	}
}
