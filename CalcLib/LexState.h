#pragma once

enum LEXER_STATE
{
	OPERAND = 0,
	VALUE = 1,
	IDENTIFIER = 2,
	OPERATOR = 3,
	ERROR = 4
};