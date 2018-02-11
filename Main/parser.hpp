#ifndef __PARSER_H__
#define __PARSER_H__

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
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

enum Token    {
    tokenInteger = 1,
    tokenDouble = 2,
	tokenReturnType = 3,
    tokenType = 4,
    tokenIf = 5,
	tokenElse = 6,
	tokenOperator = 7,
	tokenReturn = 8,
	tokenBreak = 9,
    tokenWhile = 10,
    tokenExist = 11,
	tokenIdentifier = 12,
	tokenPunctuation = 13
};

enum Mode   {
    modeDot = 1,
    modeBracket = 2
};

class ExprAST {
public:
    virtual llvm::Value *codegen() = 0;
    virtual ~ExprAST() = default;
};

class VariableExprAST : public ExprAST  {
    std::string name;
public:

    VariableExprAST(const std::string &name) : name(name) {}

    virtual llvm::Value *codegen() = 0;
    const std::string &getName() const { return name; }
};

class NumberExprAST : public ExprAST    {
    std::string type;
    double value;

public:
    NumberExprAST(const std::string &type, double value) : type(type), value(value) {}

    llvm::Value* codegen() override;
};

class IdentifierExprAST : public VariableExprAST    {
public:
    IdentifierExprAST(const std::string &name) : VariableExprAST(name) {}

    llvm::Value *codegen() override;
};

/// VariableExprAST - Expression class for referencing a variable, like "a".
class DeclareExprAST : public VariableExprAST {
    std::string type;

public:
    DeclareExprAST(const std::string &name, const std::string &type) : VariableExprAST(name), type(type) {}

    llvm::Value *codegen() override;
};

/// IfExprAST - Expression class for if/then/else.
// class IfExprAST : public ExprAST {
//     std::unique_ptr<ExprAST> Cond, Then, Else;

// public:
//     IfExprAST(std::unique_ptr<ExprAST> Cond, std::unique_ptr<ExprAST> Then,
//                         std::unique_ptr<ExprAST> Else)
//             : Cond(std::move(Cond)), Then(std::move(Then)), Else(std::move(Else)) {}

//     llvm::Value *codegen() override;
// };

/// ForExprAST - Expression class for for/in.
// class ForExprAST : public ExprAST {
//     std::string VarName;
//     std::unique_ptr<ExprAST> Start, End, Step, Body;

// public:
//     ForExprAST(const std::string &VarName, std::unique_ptr<ExprAST> Start,
//                          std::unique_ptr<ExprAST> End, std::unique_ptr<ExprAST> Step,
//                          std::unique_ptr<ExprAST> Body)
//             : VarName(VarName), Start(std::move(Start)), End(std::move(End)),
//                 Step(std::move(Step)), Body(std::move(Body)) {}

//     llvm::Value *codegen() override;
// };

class OpExprAST : public ExprAST	{
	std::string Op;
	std::unique_ptr<ExprAST> LLHS, LHS;

public:
	OpExprAST(std::string Op, std::unique_ptr<ExprAST> LLHS, std::unique_ptr<ExprAST> LHS) : Op(Op), LLHS(std::move(LLHS)), LHS(std::move(LHS)) {}
    llvm::Value *codegen() override;
};

class CallExprAST : public ExprAST { 
	std::string callee; 
	std::vector<std::unique_ptr<ExprAST>> args; 
public: 
	CallExprAST(const std::string &callee, std::vector<std::unique_ptr<ExprAST>> args) : callee(callee), args(std::move(args)) {} 
    llvm::Value *codegen() override;
};

class ReturnExprAST : public ExprAST	{
	std::unique_ptr<ExprAST> returns;
public:
	ReturnExprAST(std::unique_ptr<ExprAST> returns) : returns(std::move(returns))	{}
    llvm::Value *codegen() override;
};

class PrototypeAST { 
	std::string name; 
	std::vector<std::pair<std::string, std::string>> args; 
public: 
	PrototypeAST(const std::string &name, std::vector<std::pair<std::string, std::string>> args) : name(name), args(std::move(args)) {}
    llvm::Function* codegen();
    const std::string &getName() const { return name; }
}; 

class FunctionAST { 
public: 
	std::unique_ptr<PrototypeAST> proto;
	std::vector<std::unique_ptr<ExprAST>> body;
	FunctionAST(std::unique_ptr<PrototypeAST> proto, std::vector<std::unique_ptr<ExprAST>> body) : proto(std::move(proto)), body(std::move(body)) {}
    llvm::Function* codegen();
};

#endif
