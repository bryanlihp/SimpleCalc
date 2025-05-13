#include "pch.h"
#include "..\CalcLib\CalcLib.h"
namespace Tokenizer_Unit_Test
{
	TEST(Tokenizer_isDelimiterOrSpace, Return_True_If_Char_Is_Delimeter) 
	{
		const char* pDelimeter = "abcde";
		EXPECT_TRUE(Tokenizer::IsDelimeterOrSpace('b', pDelimeter));
	}

	TEST(Tokenizer_isDelimiterOrSpace, Return_False_If_Char_Is_Not_Delimeter)
	{
		const char* pDelimeter = "abcde";
		EXPECT_FALSE(Tokenizer::IsDelimeterOrSpace('8', pDelimeter));
	}

	TEST(Tokenizer_isDelimiterOrSpace, Return_True_If_Char_Is_Tab)
	{
		const char* pDelimeter = "abcde";
		EXPECT_TRUE(Tokenizer::IsDelimeterOrSpace('\t', pDelimeter));
	}

	TEST(Tokenizer_isDelimiterOrSpace, Return_True_If_Char_Is_Space)
	{
		const char* pDelimeter = "abcde";
		EXPECT_TRUE(Tokenizer::IsDelimeterOrSpace(' ', pDelimeter));
	}

	TEST(Tokenizer_GetToken, Can_Get_original_string_if_Delimeter_is_Empty) {
		char pText[] = "The+quick-brown-fox-jumps-over-the-lazy-dog"; //space characters are always treated as delimeters
		const char pDelimeter[2] = "";
		char* pToken = Tokenizer::GetToken(pText, pDelimeter);
		ASSERT_STREQ("The+quick-brown-fox-jumps-over-the-lazy-dog", pToken);
		ASSERT_EQ(nullptr, Tokenizer::GetToken(nullptr, " "));
	}


	TEST(Tokenizer_GetToken, Can_Get_Tokens_From_string) {
		char pText[] = "The*quick^brown - fox^jumps ^ over* the lazy*dog";
		const char pDelimeter[] = " *^"; // space characters are always treated as delimeters
		char* pToken = Tokenizer::GetToken(pText, pDelimeter);
		ASSERT_STREQ("The", pToken);
		ASSERT_STREQ("quick", Tokenizer::GetToken(nullptr, pDelimeter));
		ASSERT_STREQ("brown", Tokenizer::GetToken(nullptr, pDelimeter));
		ASSERT_STREQ("-", Tokenizer::GetToken(nullptr, pDelimeter));
		ASSERT_STREQ("fox", Tokenizer::GetToken(nullptr, pDelimeter));
		ASSERT_STREQ("jumps", Tokenizer::GetToken(nullptr, pDelimeter));
		ASSERT_STREQ("over", Tokenizer::GetToken(nullptr, pDelimeter));
		ASSERT_STREQ("the", Tokenizer::GetToken(nullptr, pDelimeter));
		ASSERT_STREQ("lazy", Tokenizer::GetToken(nullptr, pDelimeter));
		ASSERT_STREQ("dog", Tokenizer::GetToken(nullptr, pDelimeter));
		ASSERT_EQ(nullptr, Tokenizer::GetToken(nullptr, pDelimeter));
	}

	TEST(Tokenizer_GetTokenFromExperssion, Can_Get_Tokens_From_Experssion) {
		char pText[] = "The+quick+brown - fox+jumps*over+the   (lazy   ++++ dog) ";
		const char pDelimeter[] = "+-*/()";
		char* pToken = Tokenizer::GetTokenFromExpression(pText, pDelimeter);
		ASSERT_STREQ("The", pToken);
		ASSERT_STREQ("+", Tokenizer::GetTokenFromExpression(nullptr, pDelimeter));
		ASSERT_STREQ("quick", Tokenizer::GetTokenFromExpression(nullptr, pDelimeter));
		ASSERT_STREQ("+", Tokenizer::GetTokenFromExpression(nullptr, pDelimeter));
		ASSERT_STREQ("brown", Tokenizer::GetTokenFromExpression(nullptr, pDelimeter));
		ASSERT_STREQ("-", Tokenizer::GetTokenFromExpression(nullptr, pDelimeter));
		ASSERT_STREQ("fox", Tokenizer::GetTokenFromExpression(nullptr, pDelimeter));
		ASSERT_STREQ("+", Tokenizer::GetTokenFromExpression(nullptr, pDelimeter));
		ASSERT_STREQ("jumps", Tokenizer::GetTokenFromExpression(nullptr, pDelimeter));
		ASSERT_STREQ("*", Tokenizer::GetTokenFromExpression(nullptr, pDelimeter));
		ASSERT_STREQ("over", Tokenizer::GetTokenFromExpression(nullptr, pDelimeter));
		ASSERT_STREQ("+", Tokenizer::GetTokenFromExpression(nullptr, pDelimeter));
		ASSERT_STREQ("the", Tokenizer::GetTokenFromExpression(nullptr, pDelimeter));
		ASSERT_STREQ("(", Tokenizer::GetTokenFromExpression(nullptr, pDelimeter));
		ASSERT_STREQ("lazy", Tokenizer::GetTokenFromExpression(nullptr, pDelimeter));
		ASSERT_STREQ("+", Tokenizer::GetTokenFromExpression(nullptr, pDelimeter));
		ASSERT_STREQ("+", Tokenizer::GetTokenFromExpression(nullptr, pDelimeter));
		ASSERT_STREQ("+", Tokenizer::GetTokenFromExpression(nullptr, pDelimeter));
		ASSERT_STREQ("+", Tokenizer::GetTokenFromExpression(nullptr, pDelimeter));
		ASSERT_STREQ("dog", Tokenizer::GetTokenFromExpression(nullptr, pDelimeter));
		ASSERT_STREQ(")", Tokenizer::GetTokenFromExpression(nullptr, pDelimeter));
		ASSERT_EQ(nullptr, Tokenizer::GetTokenFromExpression(nullptr, pDelimeter));
	}
}
