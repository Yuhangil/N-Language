#pragma once
#ifndef __PARSER_H__
#define __PARSER_H__
#include <iostream>
#include <fstream>
#include <regex>
#include <string>


enum Token
{
	Keyword_RETURN,			// 반환한다.
	Keyword_IF,				// 만약
	Keyword_ELSE,			// 아니면
	Keyword_WHILE,			// ~ 한다 ex) (n이 3보다 크면) { 실행할 것 } 한다.
	Keyword_BREAK,			// 그만한다.
	Keyword_INT,			// 정수
	Keyword_DOUBLE,			// 실수
	Keyword_DECLARATION,	// - 있

	Op_ASSIGN,				// 이다
	Op_ADD,					// 더한
	Op_SUB,					// 뺀
	Op_MUL,					// 곱한
	Op_DIV,					// 나눈
	Op_EQUAL,				// 같으면
	Op_NOTEQUAL,			// 다르면
	Op_LESSTHAN,			// ~보다 작으면
	Op_GREATERTHAN,			// ~보다 크면
	Op_LESSTHANEQUAL,		// ~보다 작거나 같으면
	Op_GREATERTHANEQUAL,	// ~보다 크거나 같으면
	
	Punct_LPAREN,			// (
	Punct_RPAREN,			// )
	Punct_LBRACE,			// {
	Punct_RBRACE,			// }
	Punct_LBRACK,			// [
	Punct_RBRACK,			// ]

	Id,						// 식별자

	Num_INT,				// 정수 ex) 12348579
	Num_DOUBLE,				// 실수 ex) 1358.74897

	END_OF_FILE,			// -1

	ERROR,					// ex) ㉠ ㉡ ㉢

	UNKNOWN,				

};

int GetToken();

#endif // ! Parser_H