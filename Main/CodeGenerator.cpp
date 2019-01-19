#include "CodeGenerator.hpp"

std::unique_ptr<llvm::legacy::FunctionPassManager> TheFPM;
llvm::LLVMContext TheContext;
llvm::IRBuilder<> Builder(TheContext);
std::unique_ptr<llvm::Module> TheModule;
std::map<std::string, llvm::AllocaInst *> NamedValues;
std::map<std::string, std::unique_ptr<PrototypeAST>> FunctionProtos;

static llvm::AllocaInst *CreateEntryBlockAlloca(llvm::Function *TheFunction, const std::string &name) {
    llvm::IRBuilder<> EntryBlock(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());
    return EntryBlock.CreateAlloca(llvm::Type::getDoubleTy(TheContext), nullptr, name);
}

llvm::Function* getFunction(std::string Name) {
    if (auto* F = TheModule->getFunction(Name))    {
        return F;
    }

    auto FI = FunctionProtos.find(Name);

    if (FI != FunctionProtos.end()) {
        return FI->second->codegen();
    }

    return nullptr;
}

llvm::Value* NumberExprAST::codegen() {
    return llvm::ConstantFP::get(TheContext, llvm::APFloat(value));
}

llvm::Value* StringExprAST::codegen() {
    auto str = this->value;
    auto charType = llvm::IntegerType::get(TheContext, 8);

    std::vector<llvm::Constant *> chars(str.length());
    for(unsigned int i = 0; i < str.size(); i++) {
      chars[i] = llvm::ConstantInt::get(charType, str[i]);
    }

    chars.push_back(llvm::ConstantInt::get(charType, 0));

    auto stringType = llvm::ArrayType::get(charType, chars.size());

    auto globalDeclaration = (llvm::GlobalVariable*) TheModule->getOrInsertGlobal(".str", stringType);
    globalDeclaration->setInitializer(llvm::ConstantArray::get(stringType, chars));
    globalDeclaration->setConstant(true);
    globalDeclaration->setLinkage(llvm::GlobalValue::LinkageTypes::PrivateLinkage);
    globalDeclaration->setUnnamedAddr (llvm::GlobalValue::UnnamedAddr::Global);

    return llvm::ConstantExpr::getBitCast(globalDeclaration, charType->getPointerTo());
}

llvm::Value* DeclareExprAST::codegen(){
    llvm::Function *theFunction = Builder.GetInsertBlock()->getParent();
    llvm::Value *initValue = llvm::ConstantFP::get(TheContext, llvm::APFloat(0.0));
    llvm::AllocaInst *alloca = CreateEntryBlockAlloca(theFunction, getName());

    Builder.CreateStore(initValue, alloca);
    NamedValues[getName()] = alloca;

    return nullptr;
}

llvm::Value* IdentifierExprAST::codegen(){
    llvm::Value* value = NamedValues[getName()];
    if (!value) {
        return LogErrorValue("Id) Unknown variable name");
    }

    return Builder.CreateLoad(value, getName().c_str());
}

llvm::Value* ReturnExprAST::codegen(){
    llvm::Value* value;

    value = Return->codegen();
    Builder.CreateRet(value);

    return value;
}

llvm::Value* OpExprAST::codegen(){
    if(Op == "이다"){
        VariableExprAST* LLHSE = static_cast<VariableExprAST *>(LLHS.get());
        if(typeid(*LLHS) == typeid(DeclareExprAST))  {
            LLHS->codegen();
        }
        if(!LLHSE)
            return LogErrorValue("destination of '=' must be a variable");
        llvm::Value* Val = LHS->codegen();
        if(!Val)
            return nullptr;
        llvm::Value* Variable = NamedValues[LLHSE->getName()];
        if (!Variable)  {
            return LogErrorValue("Op) Unknown variable name");
        }
        Builder.CreateStore(Val, Variable);
        return Val;
    }

    llvm::Value* LL = LLHS->codegen();
    llvm::Value* L = LHS->codegen();
    if(!LL || !L)  {
        return nullptr;
    }

    llvm::Value* value;
    
    if(Op == "더한다"){
        value = Builder.CreateFAdd(LL, L, "addstore");
    } else if(Op == "뺀다"){
        value = Builder.CreateFSub(LL, L, "substore");
    } else if(Op == "곱한다"){
        value = Builder.CreateFMul(LL, L, "mulstore");
    } else if(Op == "나눈다"){
        value = Builder.CreateFDiv(LL, L, "divstore");
    } else if(Op == "더한") {
        value = Builder.CreateFAdd(LL, L, "add");
    } else if(Op == "뺀")  {
        value = Builder.CreateFSub(LL, L, "sub");
    } else if(Op == "곱한") {
        value = Builder.CreateFMul(LL, L, "mul");
    } else if(Op == "나눈") {
        value = Builder.CreateFDiv(LL, L, "div");
    } else if(Op == "같다")   {
        value = Builder.CreateFCmpUEQ(LL, L, "equal");
        value = Builder.CreateUIToFP(value, llvm::Type::getDoubleTy(TheContext), "bool");
    } else if(Op == "다르다")  {
        value = Builder.CreateFCmpUNE(LL, L, "notequal");
        value = Builder.CreateUIToFP(value, llvm::Type::getDoubleTy(TheContext), "bool");
    } else if(Op == "보다 크다")    {
        value = Builder.CreateFCmpUGT(LL, L, "greater");
        value = Builder.CreateUIToFP(value, llvm::Type::getDoubleTy(TheContext), "bool");
    } else if(Op == "보다 작다")    {
        value = Builder.CreateFCmpULT(LL, L, "less");
        value = Builder.CreateUIToFP(value, llvm::Type::getDoubleTy(TheContext), "bool");
    } else if(Op == "보다 크거나 같다")    {
        value = Builder.CreateFCmpUGE(LL, L, "greaterequal");
        value = Builder.CreateUIToFP(value, llvm::Type::getDoubleTy(TheContext), "bool");
    } else if(Op == "보다 작거나 같다")    {
        value = Builder.CreateFCmpULE(LL, L, "lessequal");
        value = Builder.CreateUIToFP(value, llvm::Type::getDoubleTy(TheContext), "bool");
    } else  {
        return LogErrorValue("invalid binary operator");
    }

    if(typeid(*LLHS) == typeid(DeclareExprAST) || typeid(*LLHS) == typeid(IdentifierExprAST))   {
        VariableExprAST* LLHSE = static_cast<VariableExprAST *>(LLHS.get());
        if(Op == "더한다" || Op == "뺀다" || Op == "곱한다" || Op == "나눈다")   {
            Builder.CreateStore(value, NamedValues[LLHSE->getName()]);
        }
    }

    return value;
}

llvm::Value* CallExprAST::codegen() {
    llvm::Function * CalleeFunction = getFunction(callee);
    std::vector<llvm::Value *> ArgsV;

    if(!CalleeFunction) {
        return LogErrorValue("-1");
    }
    if(CalleeFunction->arg_size() != args.size())   {
        return LogErrorValue("-1");
    }
    
    for(int i = 0, e = args.size(); i != e; ++i){
        ArgsV.push_back(args[i]->codegen());
        if(!ArgsV.back())   {
            return nullptr;
        }
    }
    return Builder.CreateCall(CalleeFunction, ArgsV, "call");
}

llvm::Value* IfExprAST::codegen() {
    llvm::Value* ConditionValue = Condition->codegen();
    if (!ConditionValue)  {
        return nullptr;
    }

    ConditionValue = Builder.CreateFCmpONE(ConditionValue, llvm::ConstantFP::get(TheContext, llvm::APFloat(0.0)), "ifcondition");

    llvm::Function* theFunction = Builder.GetInsertBlock()->getParent();

    llvm::BasicBlock* ThenBB = llvm::BasicBlock::Create(TheContext, "then", theFunction);
    llvm::BasicBlock* ElseBB = llvm::BasicBlock::Create(TheContext, "else");
    llvm::BasicBlock* MergeBB = llvm::BasicBlock::Create(TheContext, "afterif");

    Builder.CreateCondBr(ConditionValue, ThenBB, ElseBB);

    Builder.SetInsertPoint(ThenBB);

    for(auto& thenValue : Then){
        thenValue->codegen();
    }

    Builder.CreateBr(MergeBB);
    
    ThenBB = Builder.GetInsertBlock();

    theFunction->getBasicBlockList().push_back(ElseBB);
    Builder.SetInsertPoint(ElseBB);

    for(auto& elseValue : Else){
        elseValue->codegen();
    }

    Builder.CreateBr(MergeBB);
    
    ElseBB = Builder.GetInsertBlock();

    theFunction->getBasicBlockList().push_back(MergeBB);
    Builder.SetInsertPoint(MergeBB);
    return nullptr;
}

llvm::Value* WhileExprAST::codegen() {
    llvm::Function *theFunction = Builder.GetInsertBlock()->getParent();
    llvm::BasicBlock* LoopBB = llvm::BasicBlock::Create(TheContext, "loop", theFunction);
    llvm::BasicBlock* AfterBB = llvm::BasicBlock::Create(TheContext, "afterloop", theFunction);
    llvm::Value* ConditionValue = Condition->codegen();

    ConditionValue = Builder.CreateFCmpUNE(ConditionValue, llvm::ConstantFP::get(TheContext, llvm::APFloat(0.0)), "loopcondition");

    Builder.CreateCondBr(ConditionValue, LoopBB, AfterBB);

    Builder.SetInsertPoint(LoopBB);

    if (!ConditionValue)  {
        return nullptr;
    }

    for(auto& bodyValue : Body){
        bodyValue->codegen();
    }

    ConditionValue = Condition->codegen();

    ConditionValue = Builder.CreateFCmpUNE(ConditionValue, llvm::ConstantFP::get(TheContext, llvm::APFloat(0.0)), "loopcondition");

    Builder.CreateCondBr(ConditionValue, LoopBB, AfterBB);

    Builder.SetInsertPoint(AfterBB);

    return llvm::Constant::getNullValue(llvm::Type::getDoubleTy(TheContext));
}

llvm::Function* PrototypeAST::codegen() {
    std::vector<llvm::Type *> Doubles(Args.size(), llvm::Type::getDoubleTy(TheContext));
    llvm::FunctionType* FT = llvm::FunctionType::get(llvm::Type::getDoubleTy(TheContext), Doubles, false);
    llvm::Function* F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, name, TheModule.get());

    unsigned Idx = 0;
    for (auto &Arg : F->args())
        Arg.setName(Args[Idx++].second);
    
    return F;
}

llvm::Function* FunctionAST::codegen() {
    auto &P = *Proto;
    FunctionProtos[Proto->getName()] = std::move(Proto);
    llvm::Function* TheFunction = getFunction(P.getName());

    if(!TheFunction)
        return nullptr;
    
    if(!TheFunction->empty())
        return (llvm::Function *)LogErrorValue("Function cannot be redefined.");
    
    llvm::BasicBlock* BB = llvm::BasicBlock::Create(TheContext, "entry", TheFunction);
    Builder.SetInsertPoint(BB);

    NamedValues.clear();

    for(auto & Arg : TheFunction->args()){ 
        llvm::AllocaInst* Alloca = CreateEntryBlockAlloca(TheFunction, Arg.getName());
        Builder.CreateStore(&Arg, Alloca);
        NamedValues[Arg.getName()] = Alloca;
    }
    
    for(auto& body : Body){
        body->codegen();
    }

    if(Body.empty())    {
        TheFunction->eraseFromParent();
        return nullptr;
    }

    verifyFunction(*TheFunction);
    TheFPM->run(*TheFunction);
    return TheFunction;
}