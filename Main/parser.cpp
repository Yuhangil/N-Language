#include "parser.hpp"
#include "lexer.hpp"

std::unique_ptr<llvm::legacy::FunctionPassManager> TheFPM;
llvm::LLVMContext TheContext;
llvm::IRBuilder<> Builder(TheContext);
std::unique_ptr<llvm::Module> TheModule;
std::map<std::string, llvm::Value *> NamedValues;
std::map<std::string, std::unique_ptr<PrototypeAST>> FunctionProtos;

unsigned int currentIterator = 0;

std::unique_ptr<ExprAST> LogError(const char *string) {
    std::cout << "LogError: " << string << "\n";
    return nullptr;
}
std::unique_ptr<PrototypeAST> LogErrorP(const char *string) {
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
    return std::make_unique<DeclareExprAST>(type, name);
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
        if(tokenArray[currentIterator] == tokenExist)   {
            GetNextToken();
        }
        else if(tokenArray[currentIterator] == tokenOperator)    {
            std::unique_ptr<ExprAST> LLHS = std::move(exprArray.back());
            exprArray.erase(exprArray.end() - 1);
            std::unique_ptr<ExprAST> LHS = std::move(exprArray.back());
            exprArray.erase(exprArray.end() - 1);
            exprArray.push_back(std::make_unique<OpExprAST>(valueArray[currentIterator], std::move(LLHS), std::move(LHS)));
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

    if(tokenArray[currentIterator] == tokenPunctuation) {
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

static std::unique_ptr<FunctionAST> ParseDefinition() {
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

static void HandleDefinition() {
    if (auto result = ParseDefinition()) {
        auto functionCode = result->codegen();
        functionCode->print(llvm::errs());
    } else {
        exit(0);
    }
}

int main(int argc, char** argv)	{
    if(argc < 2)        {
        std::cout << "error : no such input file" << "\n";
        exit(1);
    }

    TheModule = llvm::make_unique<llvm::Module>("code sibal", TheContext);

    // Create a new pass manager attached to it.
    TheFPM = llvm::make_unique<llvm::legacy::FunctionPassManager>(TheModule.get());

    // Promote allocas to registers.
    TheFPM->add(llvm::createPromoteMemoryToRegisterPass());
    // Do simple "peephole" optimizations and bit-twiddling optzns.
    TheFPM->add(llvm::createInstructionCombiningPass());
    // Reassociate expressions.
    TheFPM->add(llvm::createReassociatePass());
    // Eliminate Common SubExpressions.
    TheFPM->add(llvm::createGVNPass());
    // Simplify the control flow graph (deleting unreachable blocks, etc).
    TheFPM->add(llvm::createCFGSimplificationPass());

    TheFPM->doInitialization();

    std::ifstream fileStream(argv[1]);
    StoreToken(fileStream);
    
    while(true) {
        HandleDefinition();
    }
}