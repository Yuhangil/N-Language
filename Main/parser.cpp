#include "parser.hpp"


extern std::vector<std::string> valueArray;
extern std::vector<int> tokenArray;

unsigned int currentIterator = 0;

std::unique_ptr<ExprAST> LogError(const char *string) {
    std::cout << "LogError: " << string << "\n";
    return nullptr;
}
std::unique_ptr<PrototypeAST> LogErrorP(const char *string) {
    LogError(string);
    return nullptr;
}

llvm::Value *LogErrorV(const char *string) {
  LogError(string);
  return nullptr;
}

int GetNextToken() {
    return ++currentIterator;
}

static std::unique_ptr<ExprAST> ParseExpression(int mode);

static std::unique_ptr<ExprAST> ParseNumberExpr()   {
    std::string type;
    switch(tokenArray[currentIterator]) {
        case tokenInteger: type = "int";
        case tokenDouble: type = "double";
    }

    double value = std::stod(valueArray[currentIterator]);
    
    auto result = std::make_unique<NumberExprAST>(type, value);

    GetNextToken();

    return std::move(result);
}

static std::unique_ptr<ExprAST> ParseIdentifierExpr() {
    std::string idName = valueArray[currentIterator];

    GetNextToken(); // eat identifier.

    if (valueArray[currentIterator] != "(") {
        return std::make_unique<IdentifierExprAST>(idName);
    }

    // Call.
    GetNextToken(); // eat (
    std::vector<std::unique_ptr<ExprAST>> args;
    if (valueArray[currentIterator] != ")") {
        while (true) {
            if (auto arg = ParseExpression(modeBracket))   {
                args.push_back(std::move(arg));
            }
            else    {
                return nullptr;
            }

            if (valueArray[currentIterator] == ")") {
                break;
            }

            if (valueArray[currentIterator] != ",") {
                return LogError("Expected ')' or ',' in argument list");
            }
            GetNextToken();
        }
    }

    // Eat the ')'.
    GetNextToken();

    return std::make_unique<CallExprAST>(idName, std::move(args));
}

static std::unique_ptr<ExprAST> ParseTypeExpr() {
    std::string type = valueArray[currentIterator];
    std::string name = valueArray[GetNextToken()];
    GetNextToken();
    return std::make_unique<DeclareExprAST>(name, type);
}

static std::unique_ptr<ExprAST> ParsePrimary() {
    switch (tokenArray[currentIterator]) {
    case tokenType:
        return ParseTypeExpr();
    case tokenIdentifier:
        return ParseIdentifierExpr();
    case tokenInteger:
    case tokenDouble:
        return ParseNumberExpr();
    //case tokenIf:
    //    return ParseIfExpr();
    //case tokenWhile:
    //    return ParseForExpr();
    default:
        return LogError("unknown token when expecting an expression");
        exit(0);
    }
}

static std::unique_ptr<ExprAST> ParseExpression(int mode) {
    std::string condition;
    std::vector<std::unique_ptr<ExprAST>> exprArray;
    std::unique_ptr<ExprAST> result;

    switch(mode)    {
        case modeDot: condition = "."; break;
        case modeBracket: condition = ")"; break;
    }

    while(valueArray[currentIterator] != condition)    {
        if(valueArray[currentIterator] == ",")  {
            break;
        }
        if(tokenArray[currentIterator] == tokenExist)   {
            GetNextToken();
        }
        else if(tokenArray[currentIterator] == tokenOperator)    {
            std::unique_ptr<ExprAST> LLHS = std::move(exprArray.back());
            exprArray.erase(exprArray.end() - 1);
            std::unique_ptr<ExprAST> LHS = std::move(exprArray.back());
            exprArray.erase(exprArray.end() - 1);
            exprArray.push_back(std::make_unique<OpExprAST>(valueArray[currentIterator], std::move(LHS), std::move(LLHS)));
            GetNextToken(); // eat operator.
        }
        else if(tokenArray[currentIterator] == tokenReturn)  {
            std::unique_ptr<ExprAST> E = std::move(exprArray.back());

            GetNextToken();
            return std::make_unique<ReturnExprAST>(std::move(E));
        }

        else if((result = ParsePrimary())) {
            exprArray.push_back(std::move(result));
        }
    }

    if(valueArray[currentIterator] == ".") {
        GetNextToken(); // eat punctuation
    }

    if(exprArray.empty())   {
        return nullptr;
    }

    result = std::move(exprArray.back());

    return result;
}

static std::unique_ptr<PrototypeAST> ParsePrototype() {
    std::string functionName;
    
    functionName = valueArray[currentIterator];
    
    GetNextToken(); // eat function name.

    if (valueArray[currentIterator] != "(") {
        return LogErrorP("Expected '(' in prototype");
    }

    std::vector<std::pair<std::string, std::string>> argNames;
    while (tokenArray[GetNextToken()] == tokenType)   {
        std::string type = valueArray[currentIterator];
        GetNextToken(); // eat type
        if(tokenArray[currentIterator] != tokenIdentifier)  {
            return LogErrorP("Expected id in prototype");
        }
        argNames.push_back(std::make_pair(type, valueArray[currentIterator]));    // fix
        GetNextToken(); // eat identifier
        if(valueArray[currentIterator] != ",")  {
            if(valueArray[currentIterator] == ")")  {
                break;
            }   else    {
            return LogErrorP("Expected ',' in prototype");
            }
        }
    }
    if (valueArray[currentIterator] != ")") {
        return LogErrorP("Expected ')' in prototype");
    }

    // success.
    GetNextToken(); // eat ')'.

    return std::make_unique<PrototypeAST>(functionName, std::move(argNames));
}

std::unique_ptr<FunctionAST> ParseDefinition() {
    if(tokenArray[currentIterator] > 13 || tokenArray[currentIterator] <= 0)    {
        return nullptr;
    }
    std::string f = valueArray[currentIterator];
    GetNextToken(); // eat function declare.
    auto proto = ParsePrototype();
    std::vector<std::unique_ptr<ExprAST>> E;
    if (!proto) {
        return nullptr;
    }

    GetNextToken(); //eat '{'.

    while (std::unique_ptr<ExprAST> V = ParseExpression(modeDot)) {
        E.push_back(std::move(V));
    }

    GetNextToken();

    if(E.empty())   {
        return nullptr;
    } else  {
        return std::make_unique<FunctionAST>(std::move(proto), std::move(E));
    }
}