#include <iostream>
#include <vector>
#include <map>

enum Token  {
    tokenInteger = 1,
    tokenDouble = 2,
    tokenType = 3,
	tokenReturnType = 4,
	tokenIf = 5,
	tokenElse = 6,
	tokenReturn = 7,
	tokenWhile = 8,
	tokenBreak = 9,
	tokenOperator = 10,
	tokenIdentifier = 11,
	tokenPunctuation = 12
};

class ExprAST	{
public:
	virtual ~ExprAST()	{}
};

class IntegerExprAST : public ExprAST	{
	int value;

public:
	IntegerExprAST(int value) : value(value) {}
};

class DoubleExprAST : public ExprAST	{
	double value;

public:
	DoubleExprAST(double value) : value(value) {}
};

class VarExprAST : public ExprAST	{
	std::string type;
	std::string name;

public:
	VarExprAST(const std::string &type, const std::string &name) : type(type), name(name) {}
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

extern int StoreToken();