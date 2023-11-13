#include "mainllvm.h"
#include <memory>
#include <system_error>

MainLLVM::MainLLVM(){
    moduleInit();
}

void MainLLVM::saveModuleToFile(const std::string& fileName){
    std::error_code errorCode;
    llvm::raw_fd_stream outLL(fileName, errorCode);
    module -> print(outLL, nullptr);
}
void MainLLVM::moduleInit(){
    context = std::make_unique<llvm:LLVMContext>();
    module = std::make_unique<llvm::Module>("MyExample",*context);
    builder = std::make_unique<llvm::IRBuilder>(*context);
}

void MainLLVM::exec(const std::string& program){
    saveModuleToFile("./out.ll");
}

int main(int argc, char const *arg[]){
    std::string program = R"(42)";
    MainLLVM vm;
    vm.exec(program);
    return 0;
}