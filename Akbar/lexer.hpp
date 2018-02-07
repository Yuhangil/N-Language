#ifndef __LEXER_H__
#define __LEXER_H__

#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <string>

const int patternSize = 23;

int StoreToken(std::ifstream &fileStream);

extern std::vector<std::string> valueArray;
extern std::vector<int> tokenArray;


#endif