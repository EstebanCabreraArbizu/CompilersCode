#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Intrinsics.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>
#include <memory>
#include <vector>
using namespace llvm;

int main() {
	auto TheContext = std::make_unique<llvm::LLVMContext>();
	auto TheModule = std::make_unique<Module>("Mi cool jit", *TheContext);
	auto TheBuilder = std::make_unique<IRBuilder<>>(*TheContext);

	Type *Tys[] = { Type::getInt8PtrTy(*TheContext), Type::getInt32Ty(*TheContext) };
	//función intrínseca de LLVM, que limpia la memoria del modulo con Intrinsic::getDeclaration
	Function *memset_func = Intrinsic::getDeclaration(TheModule.get(), Intrinsic::memset, Tys);

	
	auto getchar_func =
		TheModule->getOrInsertFunction("getchar", IntegerType::getInt32Ty(*TheContext));

	std::vector<Type*> Doubles(0, Type::getDoubleTy(*TheContext));
	FunctionType *FT = FunctionType::get(Type::getDoubleTy(*TheContext), Doubles, false);
	Function *Func = Function::Create(FT, Function::ExternalLinkage, "func", TheModule.get());

	BasicBlock *BB = BasicBlock::Create(*TheContext, "entry", Func);
	TheBuilder->SetInsertPoint(BB);

	TheModule->print(errs(), nullptr);

	return 0;
/*
 * para compilar
 *
 * arch:
 *   clang++ -lLLVM toy.cpp
 *
 * ubuntu/debian:
 *   clang++ toy.cpp $(llvm-config-15 --cxxflags) -lLLVM-15
 *
 * */
}
