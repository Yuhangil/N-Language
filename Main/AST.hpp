#ifndef __AST_H__
#define __AST_H__

class ExprAST {
public:
    virtual llvm::Value *codegen() = 0;
    virtual ~ExprAST() = default;
};

class NumberExprAST : public ExprAST    {
    double value;

public:
    NumberExprAST(double value) : value(value) {}

    llvm::Value* codegen() override;
};

class StringExprAST : public ExprAST    {
    std::string value;

public:
    StringExprAST(std::string value) : value(value) {}

    llvm::Value* codegen() override;
};

class VariableExprAST : public ExprAST  {
    std::string name;

public:
    VariableExprAST(const std::string &name) : name(name) {}

    virtual llvm::Value* codegen() = 0;
    const std::string &getName() const { return name; }
};

class IdentifierExprAST : public VariableExprAST    {
public:
    IdentifierExprAST(const std::string &name) : VariableExprAST(name) {}

    llvm::Value* codegen() override;
};

class DeclareExprAST : public VariableExprAST {
    std::string type;

public:
    DeclareExprAST(const std::string &name, const std::string &type) : VariableExprAST(name), type(type) {}

    llvm::Value* codegen() override;
};

class OpExprAST : public ExprAST	{
	std::string Op;
	std::unique_ptr<ExprAST> LLHS, LHS;

public:
	OpExprAST(std::string Op, std::unique_ptr<ExprAST> LLHS, std::unique_ptr<ExprAST> LHS) 
        : Op(Op), LLHS(std::move(LLHS)), LHS(std::move(LHS)) {}

    llvm::Value* codegen() override;
};

class ReturnExprAST : public ExprAST	{
	std::unique_ptr<ExprAST> Return;

public:
	ReturnExprAST(std::unique_ptr<ExprAST> Return) : Return(std::move(Return))	{}

    llvm::Value* codegen() override;
};

class CallExprAST : public ExprAST { 
	std::string callee; 
	std::vector<std::unique_ptr<ExprAST>> args; 

public: 
	CallExprAST(const std::string &callee, std::vector<std::unique_ptr<ExprAST>> args) 
        : callee(callee), args(std::move(args)) {} 

    llvm::Value* codegen() override;
};

class IfExprAST : public ExprAST {
    std::unique_ptr<ExprAST> Condition;
    std::vector<std::unique_ptr<ExprAST>> Then, Else;

public:
    IfExprAST(std::unique_ptr<ExprAST> Condition, std::vector<std::unique_ptr<ExprAST>> Then, std::vector<std::unique_ptr<ExprAST>> Else)
        : Condition(std::move(Condition)), Then(std::move(Then)), Else(std::move(Else)) {}

    llvm::Value* codegen() override;
};

class WhileExprAST : public ExprAST { 
    std::unique_ptr<ExprAST> Condition;
    std::vector<std::unique_ptr<ExprAST>> Body;

public:
    WhileExprAST(std::unique_ptr<ExprAST> Condition, std::vector<std::unique_ptr<ExprAST>> Body)
        : Condition(std::move(Condition)), Body(std::move(Body)) {}

    llvm::Value* codegen() override;
};

class PrototypeAST { 
	std::string name; 
	std::vector<std::pair<std::string, std::string>> Args; 
    
public: 
	PrototypeAST(const std::string &name, std::vector<std::pair<std::string, std::string>> Args) 
        : name(name), Args(std::move(Args)) {}

    llvm::Function* codegen();
    const std::string &getName() const { return name; }
}; 

class FunctionAST {
    std::string type;
	std::unique_ptr<PrototypeAST> Proto;
	std::vector<std::unique_ptr<ExprAST>> Body;

public:
	FunctionAST(std::string type, std::unique_ptr<PrototypeAST> Proto, std::vector<std::unique_ptr<ExprAST>> Body) 
        : type(type), Proto(std::move(Proto)), Body(std::move(Body)) {}

    llvm::Function* codegen();
};

#endif