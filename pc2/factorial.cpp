#include "llvm/IR/Verifier.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

using namespace llvm;

static Function *CreateFactorialFunction(Module *M, LLVMContext &Context) {
  // Create the factorial function and insert it into module M.
  FunctionType *FactorialFTy = FunctionType::get(Type::getInt32Ty(Context), {Type::getInt32Ty(Context)}, false);
  Function *FactorialF = Function::Create(FactorialFTy, Function::ExternalLinkage, "factorial", M);

  BasicBlock *BB = BasicBlock::Create(Context, "EntryBlock", FactorialF);
  Argument *ArgN = &*FactorialF->arg_begin();
  ArgN->setName("n");

  BasicBlock *RetBB = BasicBlock::Create(Context, "return", FactorialF);
  BasicBlock *RecurseBB = BasicBlock::Create(Context, "recurse", FactorialF);
  
  Value *One = ConstantInt::get(Type::getInt32Ty(Context), 1);

  // Create the "if(n > 1)" condition
  Value *CondInst = new ICmpInst(*BB, ICmpInst::ICMP_SGT, ArgN, One, "cond");
  BranchInst::Create(RecurseBB, RetBB, CondInst, BB);

  // Implementing the "else" condition
  ReturnInst::Create(Context, One, RetBB);// Base case: return 1 if n <= 1

  // Creating the recursive call for factorial(n - 1)
  Value *Sub = BinaryOperator::CreateSub(ArgN, One, "n-1", RecurseBB); // n - 1
  CallInst *CallFact = CallInst::Create(FactorialF, Sub, "fact", RecurseBB); // factorial(n - 1)
  CallFact->setTailCall();

  // Multiplying the current n with factorial(n - 1)
  Value *Mult = BinaryOperator::CreateMul(ArgN, CallFact, "n*fact", RecurseBB);
  
  //return the result
  ReturnInst::Create(Context, Mult, RecurseBB);

  return FactorialF;
}

int main(int argc, char **argv) {
  int n = argc > 1 ? std::atoi(argv[1]) : 5; // Cambié el valor predeterminado a 5 para evitar cálculos pesados.

  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();
  LLVMContext Context;

  std::unique_ptr<Module> Owner(new Module("test", Context));
  Module *M = Owner.get();

  Function *FactorialF = CreateFactorialFunction(M, Context);

  std::string errStr;
  ExecutionEngine *EE = 
    EngineBuilder(std::move(Owner))
    .setErrorStr(&errStr)
    .create();

  if (!EE) {
    llvm::errs() << argv[0] << ": Failed to construct ExecutionEngine: " << errStr << "\n";
    return 1;
  }

  llvm::errs() << "verifying... ";
  if (verifyModule(*M)) {
    llvm::errs() << argv[0] << ": Error constructing function!\n";
    return 1;
  }

  llvm::errs() << "OK\n";
  llvm::errs() << "We just constructed this LLVM module:\n\n---------\n" << *M;
  llvm::errs() << "---------\nstarting factorial(" << n << ") with JIT...\n";

  std::vector<GenericValue> Args(1);
  Args[0].IntVal = APInt(32, n);
  GenericValue GV = EE->runFunction(FactorialF, Args);

  llvm::outs() << "Result: " << GV.IntVal << "\n";

  return 0;
}
