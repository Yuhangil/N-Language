#include <iostream>
#include <vector>

std::stringstream GetCode(std::ifstream fileStream);
int getToken(std::string buffer);

class ExprAST	{
public:
	virtual ~ExprAST()	{}
};

class NumExprAST : public ExprAST	{
	double value;

public:
	NumExprAST(double value) : value(value) {}
};

class VarExprAST : public ExprAST	{
	std::string name;

public:
	VarExprAST(const std::string &name) : name(name) {}
};

class OpExprAST : public ExprAST	{
	char Op;
	std::unique_ptr<ExprAST> LLHS, LHS;

public:
	OpExprAST(char Op, std::unique_ptr<ExprAST> LLHS, std::unique_ptr<ExprAST> LHS) : Op(Op), LLHS(std::move(LLHS)), LHS(std::move(LHS)) {}
};

class CallExprAST : public ExprAST { 
	std::string Callee; 
	std::vector<std::unique_ptr<ExprAST>> Args; 
public: 
	CallExprAST(const std::string &Callee, std::vector<std::unique_ptr<ExprAST>> Args) : Callee(Callee), Args(std::move(Args)) {} 
};

class PrototypeAST { 
	std::string Name; 
	std::vector<std::string> Args; 
public: 
	PrototypeAST(const std::string &name, std::vector<std::string> Args) : Name(name), Args(std::move(Args)) {}
}; 

class FunctionAST { 
	std::unique_ptr<PrototypeAST> Proto; 
	std::unique_ptr<ExprAST> Body; 
public: 
	FunctionAST(std::unique_ptr<PrototypeAST> Proto, std::unique_ptr<ExprAST> Body) : Proto(std::move(Proto)), Body(std::move(Body)) {}
};