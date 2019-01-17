#include "Parser.hpp"

extern std::vector<std::string> valueArray;
extern std::vector<int> tokenArray;

unsigned int currentIterator = 0;

std::unique_ptr<ExprAST> LogError(const char* string) {
    std::cout << "LogError: " << string << "\n";

    return nullptr;
}
std::unique_ptr<PrototypeAST> LogErrorPrototype(const char* string) {
    std::cout << "LogError: " << string << "\n";

    return nullptr;
}

llvm::Value* LogErrorValue(const char* string) {
    std::cout << "LogError: " << string << "\n";
    
    return nullptr;
}

int GetNextToken() {
    return ++currentIterator;
}

static std::unique_ptr<ExprAST> ParseExpression(int mode);

static std::unique_ptr<ExprAST> ParseNumberExpr()   {
    double value;

    value = std::stod(valueArray[currentIterator]);

    GetNextToken();

    return std::make_unique<NumberExprAST>(value);
}

static std::unique_ptr<ExprAST> ParseStringExpr()   {
    std::string value;

    value = valueArray[currentIterator];

    GetNextToken();

    return std::make_unique<StringExprAST>(value);
}

static std::unique_ptr<ExprAST> ParseIdentifierExpr() {
    std::string name = valueArray[currentIterator];
    std::vector<std::unique_ptr<ExprAST>> args;

    GetNextToken();

    if (valueArray[currentIterator] != "(") {
        return std::make_unique<IdentifierExprAST>(name);
    }

    GetNextToken();
    
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

    GetNextToken();

    return std::make_unique<CallExprAST>(name, std::move(args));
}

static std::unique_ptr<ExprAST> ParseDeclareExpr() {
    std::string type = valueArray[currentIterator];
    std::string name = valueArray[GetNextToken()];

    GetNextToken();

    return std::make_unique<DeclareExprAST>(name, type);
}

static std::unique_ptr<ExprAST> ParseIfExpr(std::unique_ptr<ExprAST> Condition) {
    std::vector<std::unique_ptr<ExprAST>> Then;
    std::vector<std::unique_ptr<ExprAST>> Else;

    if (!Condition)  {
        return LogError("no condition");
    }

    if(valueArray[currentIterator] == "{")  {
        GetNextToken();
    } else    {
        return LogError("expected bracket");
    }

    while(std::unique_ptr<ExprAST> E = ParseExpression(modeDot))    {
        Then.push_back(std::move(E));

        if(valueArray[currentIterator] == "}")  {
            GetNextToken();
            break;
        }
    }
    
    if(tokenArray[currentIterator] == tokenElse)    {
        GetNextToken();
    } else  {
        return std::make_unique<IfExprAST>(std::move(Condition), std::move(Then), std::move(Else));
    }

    while(std::unique_ptr<ExprAST> E = ParseExpression(modeDot))    {
        Else.push_back(std::move(E));

        if(valueArray[currentIterator] == "}")  {
            GetNextToken();
            break;
        }
    }

    return std::make_unique<IfExprAST>(std::move(Condition), std::move(Then), std::move(Else));
}

static std::unique_ptr<ExprAST> ParseWhileExpr(std::unique_ptr<ExprAST> Condition) {
    std::vector<std::unique_ptr<ExprAST>> Body;

    if(!Condition)   {
        return nullptr;
    }
    
    if(valueArray[currentIterator] == "{")  {
        GetNextToken();
    } else    {
        return LogError("expected bracket");
    }

    while(std::unique_ptr<ExprAST> E = ParseExpression(modeDot))    {
        Body.push_back(std::move(E));
    }

    GetNextToken();

    return std::make_unique<WhileExprAST>(std::move(Condition), std::move(Body));
}

static std::unique_ptr<PrototypeAST> ParsePrototype() {
    std::string functionName = valueArray[currentIterator];
    std::vector<std::pair<std::string, std::string>> Args;
    
    GetNextToken();

    if (valueArray[currentIterator] != "(") {
        return LogErrorPrototype("Expected '(' in prototype");
    }

    while (tokenArray[GetNextToken()] == tokenType)   {
        std::string type = valueArray[currentIterator];
        
        GetNextToken();
        
        if(tokenArray[currentIterator] != tokenIdentifier)  {
            return LogErrorPrototype("Expected id in prototype");
        }
        
        Args.push_back(std::make_pair(type, valueArray[currentIterator]));
        
        GetNextToken();
        
        if(valueArray[currentIterator] != ",")  {
            if(valueArray[currentIterator] == ")")  {
                break;
            } else    {
                return LogErrorPrototype("Expected ',' in prototype");
            }
        }
    }

    if (valueArray[currentIterator] != ")") {
        return LogErrorPrototype("Expected ')' in prototype");
    }

    GetNextToken();

    return std::make_unique<PrototypeAST>(functionName, std::move(Args));
}

std::unique_ptr<FunctionAST> ParseDefinition() {
    std::string type = valueArray[currentIterator];
    std::unique_ptr<PrototypeAST> Proto;
    std::vector<std::unique_ptr<ExprAST>> Body;

    if(tokenArray[currentIterator] != tokenReturnType)    {
        return nullptr;
    }

    GetNextToken();
    Proto = ParsePrototype();

    if (!Proto) {
        return nullptr;
    }

    GetNextToken();

    while (std::unique_ptr<ExprAST> E = ParseExpression(modeDot)) {
        Body.push_back(std::move(E));
    }

    GetNextToken();

    if(Body.empty())   {
        return nullptr;
    } else  {
        return std::make_unique<FunctionAST>(type, std::move(Proto), std::move(Body));
    }
}

static std::unique_ptr<ExprAST> ParsePrimary() {
    switch (tokenArray[currentIterator]) {
        case tokenType:
            return ParseDeclareExpr();
        case tokenIdentifier:
            return ParseIdentifierExpr();
        case tokenFigure:
            return ParseNumberExpr();
        case tokenString:
            return ParseStringExpr();
        default:
            LogError("unknown token when expecting an expression");
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
        if(valueArray[currentIterator] == "," || valueArray[currentIterator] == "}")  {
            break;
        }

        switch(tokenArray[currentIterator]) {
            case tokenWhile:    {
                GetNextToken();

                return ParseWhileExpr(std::move(exprArray.back()));
            }
            case tokenIf:   {
                GetNextToken();

                return ParseIfExpr(std::move(exprArray.back()));
            }
            case tokenExist:    {
                GetNextToken(); break;
            }
            case tokenOperator: {
                std::unique_ptr<ExprAST> LLHS = std::move(exprArray.back());
                exprArray.erase(exprArray.end() - 1);
                std::unique_ptr<ExprAST> LHS = std::move(exprArray.back());
                exprArray.erase(exprArray.end() - 1);
                exprArray.push_back(std::make_unique<OpExprAST>(valueArray[currentIterator], std::move(LHS), std::move(LLHS)));
                
                GetNextToken(); break;
            }
            case tokenReturn:   {
                std::unique_ptr<ExprAST> E = std::move(exprArray.back());

                GetNextToken();
                return std::make_unique<ReturnExprAST>(std::move(E));
            }
            default:    {
                if((result = ParsePrimary()))    {
                        exprArray.push_back(std::move(result));
                }
                break;
            }
        }
    }

    if(valueArray[currentIterator] == ".") {
        GetNextToken();
    }

    if(exprArray.empty())   {
        return nullptr;
    }

    result = std::move(exprArray.back());

    return result;
}
