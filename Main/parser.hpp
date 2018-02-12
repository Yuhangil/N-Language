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
	tokenReturnType = 3,
    tokenType = 4,
    tokenIf = 5,
    tokenElse = 6,
    tokenWhile = 7,
	tokenOperator = 8,
	tokenReturn = 9,
	tokenBreak = 10,
    tokenExist = 11,
	tokenIdentifier = 12,
	tokenPunctuation = 13
};

enum Mode   {
    modeDot = 1,
    modeBracket = 2
};

#endif
