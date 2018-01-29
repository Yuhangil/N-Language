#include "parser.hpp"
#include "lexer.hpp"

unsigned int currentIterator = 0;
unsigned int operatorIterator = 0;

std::unique_ptr<ExprAST> LogError(const char *Str) {
    fprintf(stderr, "LogError: %s\n", Str);
    return nullptr;
}
std::unique_ptr<PrototypeAST> LogErrorP(const char *Str) {
    LogError(Str);
    return nullptr;
}

int GetNextToken() {
    return ++currentIterator;
}

static std::unique_ptr<ExprAST> ParseExpression(int mode);

static std::unique_ptr<ExprAST> ParseNumberExpr()   {
    std::string type = valueArray[currentIterator];

    GetNextToken();

    double value = std::stod(valueArray[currentIterator]);
    
    auto result = std::make_unique<NumberExprAST>(type, value);

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
    std::string idName = valueArray[GetNextToken()];
    std::vector<std::pair<std::string, std::unique_ptr<ExprAST>>> varNames;

    // At least one variable name is required.
    if (tokenArray[currentIterator] != tokenIdentifier) {
        return LogError("expected identifier after var");
    }
    
    GetNextToken(); // eat identifier.

    std::unique_ptr<ExprAST> initializer = nullptr;

    initializer = ParseExpression(modeDot);

    varNames.push_back(std::make_pair(idName, std::move(initializer)));

    return std::make_unique<DeclareExprAST>(std::move(type), std::move(varNames));
}

static std::unique_ptr<ExprAST> ParsePrimary() {
    switch (tokenArray[currentIterator]) {
    case tokenIdentifier:
        return ParseIdentifierExpr();
    case tokenInteger:
    case tokenDouble:
        return ParseNumberExpr();
    //case tokenIf:
    //    return ParseIfExpr();
    //case tokenWhile:
    //    return ParseForExpr();
    case tokenType:
        return ParseTypeExpr();
    // default:
    //     return LogError("unknown token when expecting an expression");
    }
}

static std::unique_ptr<ExprAST> ParseExpression(int mode) {
    if(tokenArray[currentIterator] == tokenPunctuation) {
        return nullptr;
    }

    std::string condition;
    std::vector<std::unique_ptr<ExprAST>> exprArray;
    std::unique_ptr<ExprAST> result;

    switch(mode)    {
        case modeDot: condition = "."; break;
        case modeBracket: condition = ")"; break;
    }

    while(valueArray[currentIterator] != condition)    {
        if(tokenArray[currentIterator] == tokenOperator)    {
            std::unique_ptr<ExprAST> LLHS = std::move(exprArray.back());
            exprArray.erase(exprArray.end());
            std::unique_ptr<ExprAST> LHS = std::move(exprArray.back());
            exprArray.erase(exprArray.end());
            std::unique_ptr<ExprAST> E = std::make_unique<OpExprAST>(valueArray[currentIterator], std::move(LLHS), std::move(LHS));
            exprArray.push_back(std::move(E));
            GetNextToken();
        }

        if(tokenArray[currentIterator] == tokenReturn)  {
            std::unique_ptr<ExprAST> E = std::move(exprArray.back());

            GetNextToken();
            return std::make_unique<ReturnExprAST>(std::move(E));
        }

        if((result = ParsePrimary())) {

            exprArray.push_back(std::move(result));
            GetNextToken();
        }
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

    std::vector<std::string> argNames;
    while (tokenArray[GetNextToken()] == tokenIdentifier)   {
        argNames.push_back(valueArray[currentIterator]);    // fix
    }
    if (valueArray[currentIterator] != ")") {
        return LogErrorP("Expected ')' in prototype");
    }

    // success.
    GetNextToken(); // eat ')'.

    return std::make_unique<PrototypeAST>(functionName, argNames);
}

static std::unique_ptr<FunctionAST> ParseDefinition() {
    std::string type = valueArray[currentIterator];
    GetNextToken(); // eat type.
    auto proto = ParsePrototype();
    std::vector<std::unique_ptr<ExprAST>> E;
    std::unique_ptr<ReturnExprAST> returns;
    if (!proto) {
        return nullptr;
    }

    GetNextToken(); //eat '{'.

    while (std::unique_ptr<ExprAST> V = ParseExpression(modeDot)) {
        if(typeid(*V) == typeid(std::unique_ptr<ReturnExprAST>))   {
            returns = std::make_unique<ReturnExprAST>(std::move(V));
        }
        E.push_back(std::move(V));
    }

    GetNextToken();

    if(E.empty())   {
        return nullptr;
    } else  {
        return std::make_unique<FunctionAST>(std::move(proto), std::move(E), std::move(returns));
    }
}

static void HandleDefinition() {
    if (!ParseDefinition()) {
        std::cout << "Hi" << "\n";
    } else {
        // Skip token for error recovery.
        GetNextToken();
    }
}

int main(int argc, char** argv)	{
    if(argc < 2)        {
        std::cout << "error : no such input file" << "\n";
        exit(1);
    }
    std::ifstream fileStream(argv[1]);
    StoreToken(fileStream);
    
    while(true) {
        HandleDefinition();
    }
}