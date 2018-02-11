#include "parser.hpp"

extern std::unique_ptr<llvm::legacy::FunctionPassManager> TheFPM;
extern llvm::LLVMContext TheContext;
extern llvm::IRBuilder<> Builder;
extern std::unique_ptr<llvm::Module> TheModule;
extern std::map<std::string, llvm::AllocaInst *> NamedValues;
extern std::map<std::string, std::unique_ptr<PrototypeAST>> FunctionProtos;

std::unique_ptr<ExprAST> LogError_(const char *Str) {
    fprintf(stderr, "LogError: %s\n", Str);
    return nullptr;
}

llvm::Value *LogErrorV(const char *Str) {
  LogError_(Str);
  return nullptr;
}
static llvm::AllocaInst *CreateEntryBlockAlloca(llvm::Function *TheFunction, const std::string &name) {
    llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());
    return TmpB.CreateAlloca(llvm::Type::getDoubleTy(TheContext), nullptr, name);
}

llvm::Function *getFunction(std::string Name) {
    if (auto * F = TheModule->getFunction(Name))    {
        return F;
    }
    auto FI = FunctionProtos.find(Name);
    if (FI != FunctionProtos.end())
        return FI->second->codegen();

    return nullptr;
}

llvm::Value *NumberExprAST::codegen() {
    return llvm::ConstantFP::get(TheContext, llvm::APFloat(value));
}

llvm::Value * CallExprAST::codegen() {
    llvm::Function * CalleeF = getFunction(callee);
    if(!CalleeF)
        return LogErrorV("-1");
    if(CalleeF->arg_size() != args.size())
        return LogErrorV("-1");
    
    std::vector<llvm::Value *> ArgsV;

    for(int i = 0, e = args.size(); i != e; ++i){
        ArgsV.push_back(args[i]->codegen());
        if(!ArgsV.back())
            return nullptr;
    }
    return Builder.CreateCall(CalleeF, ArgsV, "calltmp");
}

llvm::Function * PrototypeAST::codegen() {
    std::vector<llvm::Type *> Doubles(args.size(), llvm::Type::getDoubleTy(TheContext));
    llvm::FunctionType * FT = llvm::FunctionType::get(llvm::Type::getDoubleTy(TheContext), Doubles, false);
    llvm::Function * F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, name, TheModule.get());

    unsigned Idx = 0;
    for (auto &Arg : F->args())
        Arg.setName(args[Idx++].second);
    
    return F;
}

llvm::Function * FunctionAST::codegen() {
    auto &P = *proto;
    FunctionProtos[proto->getName()] = std::move(proto);
    llvm::Function * TheFunction = getFunction(P.getName());

    if(!TheFunction)
        return nullptr;
    
    if(!TheFunction->empty())
        return (llvm::Function *)LogErrorV("Function cannot be redefined.");
    
    llvm::BasicBlock * BB = llvm::BasicBlock::Create(TheContext, "entry", TheFunction);
    Builder.SetInsertPoint(BB); //end

    NamedValues.clear();

    for(auto & Arg : TheFunction->args()){ 
        llvm::AllocaInst * Alloca = CreateEntryBlockAlloca(TheFunction, Arg.getName());
        Builder.CreateStore(&Arg, Alloca);
        NamedValues[Arg.getName()] = Alloca;
    }
    
    for(auto & Body : body){
        Body->codegen();
    }

    if(body.empty())    {
        TheFunction->eraseFromParent();
        return nullptr;
    }

    verifyFunction(*TheFunction);
    TheFPM->run(*TheFunction);
    return TheFunction;
}

llvm::Value * DeclareExprAST::codegen(){
    std::vector<llvm::AllocaInst *> OldBindings;
    llvm::Function *TheFunction = Builder.GetInsertBlock()->getParent();
    llvm::Value *InitVal = llvm::ConstantFP::get(TheContext, llvm::APFloat(0.0));
    llvm::AllocaInst *Alloca = CreateEntryBlockAlloca(TheFunction, getName());

    // Remember the old variable binding so that we can restore the binding when
    // we unrecurse.
    OldBindings.push_back(NamedValues[getName()]);
    Builder.CreateStore(InitVal, Alloca);

    // Remember this binding.
    NamedValues[getName()] = Alloca;

    return nullptr;
}

llvm::Value * ReturnExprAST::codegen(){
    llvm::Value* value;

    value = returns->codegen();
    Builder.CreateRet(value);

    return value;
}

llvm::Value * IdentifierExprAST::codegen(){
    llvm::Value *V = NamedValues[getName()];
    if (!V)
        return LogErrorV("Unknown variable name");

  // Load the value.
    return Builder.CreateLoad(V, getName().c_str());
}

llvm::Value * OpExprAST::codegen(){
    if(Op == "이다"){
        VariableExprAST* LLHSE = static_cast<VariableExprAST *>(LLHS.get());
        if(typeid(*LLHS) == typeid(DeclareExprAST))  {
            LLHS->codegen();
        }
        if(!LLHSE)
            return LogErrorV("destination of '=' must be a variable");
        llvm::Value * Val = LHS->codegen();
        if(!Val)
            return nullptr;
        llvm::Value *Variable = NamedValues[LLHSE->getName()];
        if (!Variable)  {
            return LogErrorV("Unknown variable name");
        }
        Builder.CreateStore(Val, Variable);
        return Val;
    }

    llvm::Value *LL = LLHS->codegen();
    llvm::Value *L = LHS->codegen();
    if(!LL || !L)  {
        return nullptr;
    }

    llvm::Value *val;
    
    if(Op == "더한다"){
        val = Builder.CreateFAdd(LL, L, "addstore");
    } else if(Op == "뺀다"){
        val = Builder.CreateFSub(LL, L, "substore");
    } else if(Op == "곱한다"){
        val = Builder.CreateFMul(LL, L, "mulstore");
    } else if(Op == "나눈다"){
        val = Builder.CreateFDiv(LL, L, "divstore");
    } else if(Op == "더한") {
        val = Builder.CreateFAdd(LL, L, "add");
    } else if(Op == "뺀")  {
        val = Builder.CreateFSub(LL, L, "sub");
    } else if(Op == "곱한") {
        val = Builder.CreateFMul(LL, L, "mul");
    } else if(Op == "나눈") {
        val = Builder.CreateFDiv(LL, L, "div");
    } else  {
        return LogErrorV("invalid binary operator");
    }

    if(typeid(*LLHS) == typeid(DeclareExprAST) || typeid(*LLHS) == typeid(IdentifierExprAST))   {
        VariableExprAST* LLHSE = static_cast<VariableExprAST *>(LLHS.get());
        Builder.CreateStore(val, NamedValues[LLHSE->getName()]);
    }

    return nullptr;
}