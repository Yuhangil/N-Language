#include "main.hpp"

extern std::unique_ptr<llvm::legacy::FunctionPassManager> TheFPM;
extern llvm::LLVMContext TheContext;
extern llvm::IRBuilder<> Builder;
extern std::unique_ptr<llvm::Module> TheModule;
extern std::map<std::string, llvm::AllocaInst *> NamedValues;
extern std::map<std::string, std::unique_ptr<PrototypeAST>> FunctionProtos;

static int HandleDefinition() {
    std::error_code EC;
    llvm::raw_fd_ostream output("output.bc", EC, llvm::sys::fs::F_Append);
    if (auto result = ParseDefinition()) {
        auto functionCode = result->codegen();
        functionCode->print(output);
        return 0;
    } else {
        return -1;
    }
}

int main(int argc, char** argv)	{
    if(argc < 2)        {
        std::cout << "error : no such input file" << "\n";
        exit(1);
    }

    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    auto CPU = "generic", Features = "";
    std::string command, Error, Filename = "output.o";
    std::ifstream inputFile(argv[1]);
    std::stringstream inputStream;
    std::string inputString;
    std::fstream f;
    auto TargetTriple = llvm::sys::getDefaultTargetTriple();
    auto Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);
    llvm::TargetOptions opt;
    auto RM = llvm::Optional<llvm::Reloc::Model>();
    auto TheTargetMachine = Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);
    std::error_code EC;
    llvm::raw_fd_ostream dest(Filename, EC, llvm::sys::fs::F_None);
    llvm::legacy::PassManager pass;
    auto FileType = llvm::TargetMachine::CGFT_ObjectFile;

    inputStream << inputFile.rdbuf();
    inputString = inputStream.str();

    LexicalAnalysis(inputString); 

    TheModule = llvm::make_unique<llvm::Module>("N-Language", TheContext);
    TheFPM = llvm::make_unique<llvm::legacy::FunctionPassManager>(TheModule.get());

    TheFPM->add(llvm::createPromoteMemoryToRegisterPass());
    TheFPM->add(llvm::createInstructionCombiningPass());
    TheFPM->add(llvm::createReassociatePass());
    TheFPM->add(llvm::createGVNPass());
    TheFPM->add(llvm::createCFGSimplificationPass());
    TheFPM->doInitialization();

    f.open("output.bc", std::fstream::out | std::fstream::trunc);
    f.close();
    
    while(true) {
        if(HandleDefinition() == -1)    {
            break;
        }
    }

    TheModule->setTargetTriple(TargetTriple);

    if (!Target) {
      llvm::errs() << Error;
      return 1;
    }

    TheModule->setDataLayout(TheTargetMachine->createDataLayout());

    if (EC) {
      llvm::errs() << "Could not open file: " << EC.message();
      return 1;
    }

    if (TheTargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
      llvm::errs() << "TheTargetMachine can't emit a file of this type";
      return 1;
    }

    pass.run(*TheModule);
    dest.flush();

    std::cout << "Wrote " << Filename << "\n";

    command = "gcc " + Filename + " -o output";
    system(command.c_str());

    std::cout << command << "\n";

    return 0;
}