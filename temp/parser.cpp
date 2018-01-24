#include "parser.hpp"
#include "lexer.hpp"

extern std::vector<std::string> valueArray;
extern std::vector<int> tokenArray;

unsigned int currentIterator = 0;
unsigned int operatorIterator = 0;

int main(int argc, char** argv)	{
    if(argc < 2)    {
        std::cout << "error : no such input file" << "\n";
        exit(1);
    }
    std::ifstream fileStream(argv[1]);
    StoreToken();

    for(currentIterator = 0; currentIterator < tokenArray.size(); ++currentIterator)    {
        switch(tokenArray[currentIterator]) {
            case tokenReturnType: HandleDefinition();
            case tokenIf:
            case tokenWhile:
            default: HandleTopLevelExpression();
        }
    }
}

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

void HandleDefinition() {
    if(ParseDefinition())  {
        std::cout << "Parsed a function definition" << "\n";
    }
    else    {
        GetNextToken();
    }
}

void HandleTopLevelExpression() { 
    if (ParseTopLevelExpr()) { 
        std::cout << "Parsed a top-level expr" << "\n";
    } 
    else { 
        GetNextToken(); 
    } 
}

std::unique_ptr<FunctionAST> ParseTopLevelExpr() {
    if (auto E = ParseExpression()) {
        auto prototype = std::make_unique<PrototypeAST>("", std::vector<std::string>());
        return std::make_unique<FunctionAST>(std::move(prototype), std::move(E));
    }
    return nullptr;
}

std::unique_ptr<ExprAST> ParsePrimary() { 
    switch (tokenArray[currentIterator]) { 
        case tokenType: return ParseTypeExpr();
        case tokenIdentifier: return ParseIdentifierExpr(); 
        case tokenDouble: return ParseDoubleExpr();
        case tokenInteger: return ParseIntegerExpr(); 
        case '(': return ParseParentExpr(); 
        default: break;
    }
}

std::unique_ptr<ExprAST> ParseExpression() { 
    auto LHS = ParsePrimary(); 
    if (!LHS)   {
        return nullptr;
    }

    return ParseOptrExpr(0, std::move(LHS)); 
}

std::unique_ptr<ExprAST> ParseIntegerExpr() {
    auto result = std::make_unique<IntegerExprAST>(stoi(valueArray[currentIterator]));
    GetNextToken();
    return std::move(result);
}

std::unique_ptr<ExprAST> ParseDoubleExpr() {
    auto result = std::make_unique<DoubleExprAST>(stod(valueArray[currentIterator]));
    GetNextToken();
    return std::move(result);
}

std::unique_ptr<ExprAST> ParseTypeExpr()    {
    std::string 
}

std::unique_ptr<ExprAST> ParseIdentifierExpr() {
    std::string idName = valueArray[currentIterator];

    GetNextToken();

    if (valueArray[currentIterator] != "(")
        return std::make_unique<VarExprAST>(idName);

    GetNextToken();
    std::vector<std::unique_ptr<ExprAST>> Args;
    if (valueArray[currentIterator] != ")") {
        while (1) {
            if (auto Arg = ParseExpression())
                Args.push_back(std::move(Arg));
            else
                return nullptr;

            if (valueArray[currentIterator] == ")")    {
                break;
            }

            if (valueArray[currentIterator] != ",")
                return LogError("Expected ')' or ',' in argument list");
            GetNextToken();
        }
    }

    GetNextToken();

    return std::make_unique<CallExprAST>(idName, std::move(Args));
}

std::unique_ptr<ExprAST> ParseParentExpr() {
    GetNextToken();
    auto V = ParseExpression();
    if (!V) {
        return nullptr;
    }

    if (currentToken != ')')    {
        return LogError("expected ')'");
    }
    
    GetNextToken();
    return V;
}

std::unique_ptr<ExprAST> ParseOptrExpr(int exprPriority, std::unique_ptr<ExprAST>LLHS, std::unique_ptr<ExprAST> LHS) { 
    while (1) { 
        int tokenPriority = GetTokenPriority(); 
        if (tokenPriority < exprPriority) {
            return LHS;
        }
        int BinOp = currentToken; 
        GetNextToken(); 
        auto RHS = ParsePrimary();
        if (!RHS)   {
            return nullptr;
        }
        int nextPriority = GetTokenPriority();
        if (tokenPriority < nextPriority) {
            RHS = ParseOptrExpr(tokenPriority + 1, std::move(RHS));
            if (!RHS)   {
                return nullptr;
            }
        }
        LHS = std::make_unique<OpExprAST>(BinOp, std::move(LHS), std::move(RHS));
    }
}

std::unique_ptr<FunctionAST> ParseDefinition() {
    GetNextToken();

    auto prototype = ParsePrototype();
    if (!prototype) {
        return nullptr;
    }

    if (auto E = ParseExpression()) {
        return std::make_unique<FunctionAST>(std::move(prototype), std::move(E));
    }
    return nullptr;
}

std::unique_ptr<PrototypeAST> ParsePrototype() {
    if (tokenArray[currentIterator] != tokenIdentifier)    {
        return LogErrorP("Expected function name in prototype");
    }

    std::string functionName = valueArray[currentIterator];
    GetNextToken();

    if (valueArray[currentIterator] != "(")    {
        return LogErrorP("Expected '(' in prototype");
    }

    std::vector<std::string> ArgNames;
    while (tokenArray[GetNextToken()] == tokenIdentifier)   {
        ArgNames.push_back(valueArray[currentIterator]);
    }
    if (valueArray[currentIterator] != ")")    {
        return LogErrorP("Expected ')' in prototype");
    }

    GetNextToken();

    return std::make_unique<PrototypeAST>(functionName, std::move(ArgNames));
}

