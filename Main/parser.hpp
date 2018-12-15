#ifndef __PARSER_H__
#define __PARSER_H__

#include "llvm/IR/Value.h"
#include <iostream>
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <typeinfo>
#include "AST.hpp"

enum Token    {
    tokenInteger = 1,
    tokenDouble = 2,
    tokenString = 3,
	tokenReturnType = 4,
    tokenType = 5,
    tokenIf = 6,
    tokenElse = 7,
    tokenWhile = 8,
	tokenOperator = 9,
	tokenReturn = 10,
	tokenBreak = 11,
    tokenExist = 12,
	tokenIdentifier = 13,
	tokenPunctuation = 14
};

enum Mode   {
    modeDot = 1,
    modeBracket = 2
};

#endif
