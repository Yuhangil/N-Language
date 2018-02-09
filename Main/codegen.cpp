#include "parser.hpp"

static std::unique_ptr<llvm::legacy::FunctionPassManager> TheFPM;
extern llvm::LLVMContext TheContext;
extern llvm::IRBuilder<> Builder;
extern std::unique_ptr<llvm::Module> TheModule;
extern std::map<std::string, llvm::Value *> NamedValues;
extern std::map<std::string, std::unique_ptr<PrototypeAST>> FunctionProtos;

std::unique_ptr<ExprAST> LogError_(const char *Str) {
    fprintf(stderr, "LogError: %s\n", Str);
    return nullptr;
}

llvm::Value *LogErrorV(const char *Str) {
  LogError_(Str);
  return nullptr;
}

llvm::Value *NumberExprAST::codegen() {
    return llvm::ConstantFP::get(TheContext, llvm::APFloat(value));
}
static llvm::AllocaInst *CreateEntryBlockAlloca(llvm::Function *TheFunction, const std::string &varName) {
    llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());
    return TmpB.CreateAlloca(llvm::Type::getDoubleTy(TheContext), nullptr, varName);
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

    // Register all variables and emit their initializer.
    for (unsigned i = 0, e = varNames.size(); i != e; ++i) {
      const std::string &varName = varNames[i].first;
      ExprAST *Init = varNames[i].second.get();

      // Emit the initializer before adding the variable to scope, this prevents
      // the initializer from referencing the variable itself, and permits stuff
      // like this:
      //  var a = 1 in
      //    var a = a in ...   # refers to outer 'a'.
      llvm::Value *InitVal;
      if (Init) {
        InitVal = Init->codegen();
        if (!InitVal)
          return nullptr;
      } else { // If not specified, use 0.0.
        InitVal = llvm::ConstantFP::get(TheContext, llvm::APFloat(0.0));
      }

      llvm::AllocaInst *Alloca = CreateEntryBlockAlloca(TheFunction, varName);
      Builder.CreateStore(InitVal, Alloca);

      // Remember the old variable binding so that we can restore the binding when
      // we unrecurse.
      OldBindings.push_back(NamedValues[varName]);

      // Remember this binding.
      NamedValues[varName] = Alloca;
    }

    return nullptr;

    // // Codegen the body, now that all vars are in scope.
    // Value *BodyVal = Body->codegen();
    // if (!BodyVal)
    //   return nullptr;

    // // Pop all our variables from scope.
    // for (unsigned i = 0, e = varNames.size(); i != e; ++i)
    //   NamedValues[varNames[i].first] = OldBindings[i];

    // // Return the body computation.
    // return BodyVal;
}

llvm::Value * ReturnExprAST::codegen(){
    llvm::Value* value;

    value = returns->codegen();
    Builder.CreateRet(value);

    return value;
}

llvm::Value * IdentifierExprAST::codegen(){
    llvm::Value *V = NamedValues[name];
    if (!V)
        return LogErrorV("Unknown variable name");

  // Load the value.
    return Builder.CreateLoad(V, name.c_str());
}

llvm::Value * OpExprAST::codegen(){
    if(Op.compare("이다")){
        IdentifierExprAST * LHSE = static_cast<IdentifierExprAST *>(LLHS.get());
        if(!LHSE)
            return LogErrorV("destination of '=' must be a variable");
        llvm::Value * Val = LHS->codegen();
        if(!Val)
            return nullptr;
        llvm::Value * Variable = NamedValues[LHSE->getName()];
        if (!Variable)
            return LogErrorV("Unknown variable name");
        Builder.CreateStore(Val, Variable);
        return Val;
    }

    llvm::Value *LL = LLHS->codegen();
    llvm::Value *L = LHS->codegen();
    if (!L || !LL)
        return nullptr;
    
    if(Op.compare("더한다")){
        return Builder.CreateFAdd(LL, L, "addtmp");
    } else if(Op.compare("뺀다")){
        return Builder.CreateFSub(LL, L, "subtmp");
    } else if(Op.compare("곱한다")){
        return Builder.CreateFMul(LL, L, "multmp");
    } else if(Op.compare("나눈다")){
        return Builder.CreateFDiv(LL, L, "divtmp");
    }

    return LogErrorV("invalid binary operator");
}