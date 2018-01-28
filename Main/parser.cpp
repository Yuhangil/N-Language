#include "parser.hpp"
#include "lexer.hpp"

extern std::vector<std::string> valueArray;
extern std::vector<int> tokenArray;

unsigned int currentIterator = 0;
unsigned int operatorIterator = 0;

int main(int argc, char** argv)	{
    if(argc < 2)        {
        std::cout << "error : no such input file" << "\n";
        exit(1);
    }
    std::ifstream fileStream(argv[1]);
    StoreToken();
    
    while(true) {
        HandleDefinition();
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

static void HandleDefinition() {
    if (auto functionAST = ParseDefinition()) {
        if (auto *functionIR = functionAST->codegen()) {
            fprintf(stderr, "Read function definition:");
            functionIR->print(errs());
            fprintf(stderr, "\n");
            TheJIT->addModule(std::move(TheModule));
            InitializeModuleAndPassManager();
        }
    } else {
        // Skip token for error recovery.
        GetNextToken();
    }
}

static std::unique_ptr<ExprAST> ParseExpression();

static std::unique_ptr<ExprAST> ParseNumberExpr()   {
    std::string type = valueArray[currentIterator];

    GetNextToken();

    double value = std::stod(valueArray[currentIterator]);
    
    auto result = llvm::make_unique<NumberExprAST>(type, value);

    return std::move(result);
}

static std::unique_ptr<ExprAST> ParseIdentifierExpr() {
    std::string idName = valueArray[currentIterator];

    GetNextToken(); // eat identifier.

    if (valueArray[currentIterator] != "(") {
        return llvm::make_unique<IdentifierExprAST>(idName);
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

    return llvm::make_unique<CallExprAST>(idName, std::move(args));
}

static std::unique_ptr<ExprAST> ParseTypeExpr() {
    std::string type = valueArray[currentIterator];
    std::string idName = valueArray[GetNextToken()];
    std::vector<std::pair<std::string, std::unique_ptr<ExprAST>>> varNames;

    // At least one variable name is required.
    if (tokenArray[currentIterator] != tokenIdentifier) {
        return LogError("expected identifier after var");
    }

    tempIterator = currentIterator;
    
    GetNextToken(); // eat identifier.

    std::unique_ptr<ExprAST> initializer = nullptr;

    initializer = ParseExpression(modeDot);

    varNames.push_back(std::make_pair(idName, std::move(initializer)));

    return llvm::make_unique<DeclareExprAST>(std::move(type), std::move(varNames));
}

static std::unique_ptr<ReturnExprAST> ParseReturnExpr() {
    GetNextToken(); // eat return.

    auto result = ParseExpression(modeDot);

    return llvm::make_unique<ReturnExprAST>(std::move(result));
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
    default:
        return LogError("unknown token when expecting an expression");
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
            std::unique_ptr<ExprAST> LLHS = exprArray.pop_back();
            std::unique_ptr<ExprAST> LHS = exprArray.pop_back();
            exprArray.push_back(std::move(llvm::make_unique<OperatorExprAST>(valueArray[currentIterator], LLHS, LHS)));
        }

        if(result = ParsePrimary()) {
            exprArray.push_back(std::move(result));
            GetNextToken();
        }
    }
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

    return llvm::make_unique<PrototypeAST>(functionName, argNames);
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

    while (auto V = ParseExpression(modeDot)) {
        if(typeid(V) == typeid(std::unique_ptr<ReturnExprAST>))   {
            returns = std::move(V);
        }
        E.push_back(V);
        return llvm::make_unique<FunctionAST>(std::move(proto), std::move(E), std::move(returns));
    }

    GetNextToken();

    return nullptr;
}