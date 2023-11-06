#include <llvm/IR/LLVMContext.h>
#include <memory>

using namespace llvm;

int main(){
    auto TheContext = std::make_unique<llvm::LLVMContext>();
    auto TheModule = std::make_unique<Module>("Mi cool jit", *TheContext);
    auto TheBuildel = std::make_unique<IRBuilder<>>(*TheContext);

    Type *Tys[] = { Type::getInt8PtrTy(*TheContext), Type::getInt32Ty(*TheContext)};
    Function *memset_func = Intrinsic::getDeclaration(TheModule.get(), Intrinsic::memset, Tys);
    auto getchar_func = TheModule-> getOrInsertFunction("getchar", IntegerType::getInt32Ty(*TheContext));

    std::vector<Type¨*> Doubles(0, Type::getDoubleTy(*TheContext));
    FunctionType *FT = FunctionType::geDoubleTy¨(*TheContext);
    return 0;
}
