#ifndef __LEXER_HPP__
#define __LEXER_HPP__

#include <string>
#include <vector>
#include <regex>
#include <iostream>
#include <fstream>

int LexicalAnalysis(std::string inputString);

enum Token    {
	tokenFigure = 1,
    tokenString = 2,
	tokenReturnType = 3,
    tokenType = 4,
    tokenIf = 5,
    tokenElse = 6,
    tokenWhile = 7,
	tokenOperator = 8,
	tokenReturn = 9,
	tokenBreak = 10,
    tokenExist = 11,
	tokenPunctuation = 12,
	tokenIdentifier = 13
};

static std::vector<std::regex> pattern =
{
	std::regex("-?\\d+(\\.\\d+)?"),
	std::regex("\"([^\"]*)\""),
	std::regex("실수형|문자형"),
	std::regex("실수|문자"),
	std::regex("면|이면|라면"),
	std::regex("아니면"),
	std::regex("동안"),
	std::regex("(더한다|뺀다|곱한다|나눈다|이다|더한|뺀|곱한|나눈|같다|다르다|보다 크다|보다 작다|보다 크거나 같다|보다 작거나 같다)"),
	std::regex("반환한다"),
	std::regex("그만한다"),
	std::regex("있다"),
	std::regex("\\{|\\}|\\(|\\)|\\[|\\]|\\.|\\,"),
	std::regex("(\\s)([^\\d\\s][^\\s]*)(\\s)")
};

static std::vector<std::regex> removePattern =
{
	std::regex("(\\s)([^\\d\\s][^\\s]*|-?\\d+\\.?\\d*)(이다\\.)(\\s))"),
	std::regex("(\\s)([^\\d\\s][^\\s]*|-?\\d+\\.?\\d*)(은|는|이|가|을|를|와|과|에서|에)(\\s)"),
	std::regex("(\\s)([^\\d\\s][^\\s]*|-?\\d+\\.?\\d*)(면|이면|라면)(\\s)"),
	std::regex("([\\(]|[\\)]|[\\,])"),
	std::regex("[\\s]+"),
	std::regex("(더한|뺀|곱한|나눈)(\\s)(값)"),
	std::regex("([^\\d])(\\.)"),
	std::regex("핵심함수"),
	std::regex("(\")([^\"]*)(\")"),
    std::regex("(\\s)([^\\d\\s][^\\s]*|-?\\d+\\.?\\d*)(은|는|이|가|을|를|와|과|에서|에)(\\s)")
};

static std::vector<std::string> removedString = 
{
	std::string("$1$2 $3$4"),
	std::string("$1$2$4"),
	std::string("$1$2 $3$4"),
	std::string(" $1 "),
	std::string(" "),
	std::string("$1 "),
	std::string("$1 $2"),
	std::string("main"),
	std::string("$2"),
    std::string("$1$2$4")
};

#endif