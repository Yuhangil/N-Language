#ifndef __LEXER_H__
#define __LEXER_H__

#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <string>

const int patternSize = 23;

int StoreToken(std::ifstream &fileStream);

static std::regex pattern[patternSize] =
{
	std::regex("-?\\d+[^.]"),
	std::regex("-?\\d+\\.\\d+"),
	std::regex("정수형|실수형"),
	std::regex("정수|실수"),
	std::regex("만약"),
	std::regex("아니면"),
	std::regex("(더한다|뺀다|곱한다|나눈다|이다|더한|뺀|곱한|나눈)"),
	std::regex("반환한다"),
	std::regex("그만한다"),
	std::regex("한다"),
	std::regex("있다"),
	std::regex("([a-zA-Z_가-힣][a-zA-Z_가-힣0-9]*)"),
	std::regex("\\{|\\}|\\(|\\)|\\[|\\]|\\.|\\,")
};


static std::regex removePattern[] =
{
	std::regex("([a-zA-Z_가-힣][a-zA-Z_가-힣0-9]*)([\\(])"),
	std::regex("([a-zA-Z_가-힣][a-zA-Z_가-힣0-9]*|-?\\d+\\.?\\d*)(이다\\.)"),
	std::regex("([a-zA-Z_가-힣][a-zA-Z_가-힣0-9]*|-?\\d+\\.?\\d*|[\\)])(은|는|이|가|을|를|와|과|에)"),
	std::regex("([\\(]|[\\)]|[\\,])"),
	std::regex("( )+"),
	std::regex("(더한|뺀|곱한|나눈)( )(값)"),
	std::regex("([^\\d])(\\.)"),
	std::regex("핵심함수")
};

#endif