#pragma once
#ifndef __LEXER_H__
#define __LEXER_H__

#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <string>

const int patternSize = 19;

enum Token
{
	NumberInteger,				// 정수 ex) 12348579
	NumberDouble,				// 실수 ex) 1358.74897
	KeywordInt,				// 정수
	KeywordDouble,			// 실수
	KeywordIf,				// 만약
	KeywordElse,			// 아니면
	KeywordDeclaration,		// - 있
	OperatorAssign,			// 이다
	OperatorAdd,			// 더한다
	OperatorSubstract,		// 뺀다
	OperatorMultiply,		// 곱한다
	OperatorDivide,			// 나눈다
	KeywordReturn,			// 반환한다
	KeywordWhile,			// ~ 한다 ex) (n이 3보다 크면) { 실행할 것 } 한다
	KeywordBreak,			// 그만한다
	Identifier,				// 식별자
	
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

	EndOfFile,					// -1

	ERROR,					// ex) ㉠ ㉡ ㉢

	UNKNOWN				
};

std::regex pattern[patternSize] =	{
	std::regex("^-?\\d+"),
	std::regex("^-?\\d+\\.\\d+?$"),
	std::regex("정수"),
	std::regex("실수"),
	std::regex("만약"),
	std::regex("아니면"),
	std::regex("있다"),
	std::regex("이다"),
	std::regex("더한다"),
	std::regex("뺀다"),
	std::regex("곱한다"),
	std::regex("나눈다"),
	std::regex("반환한다"),
	std::regex("한다"),
	std::regex("그만한다"),
	std::regex("([a-zA-Z_가-힣][a-zA-Z_가-힣0-9]*)"),
	std::regex("[{}]"),
	std::regex("[()]")	
};

std::regex removePattern[5] =	{
	std::regex("([가-힣][가-힣]*다)(\\.)"),
	std::regex("([a-zA-Z_가-힣][a-zA-Z_가-힣0-9]*)([(])"),
	std::regex("([a-zA-Z_가-힣][a-zA-Z_가-힣0-9]*)(은|는|이|가|을|를|와|에)"),
	std::regex("([()])"),
	std::regex("( )+")
};



int GetToken();

#endif