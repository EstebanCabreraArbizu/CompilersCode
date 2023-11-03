/*
  Esta es la manera SIMPLIFICADA de usar el analizador sintáctico
*/

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <llvm/ADT/APFloat.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>
#include <iostream>
#include <string>
#include <map> 
#include <memory>
#include <string>
#include <vector>

using namespace llvm;

//#include "llvm/ADT/APFloat.h"

/*
   ----------
   LEXER
   ----------
*/

enum Token {
	tok_eof = -1,
	//comands
	tok_def = -2,
	tok_extern = -3,

	//primary tokens
	tok_identifier = -4,
	tok_number = -5,
};

static std::string IdentifierStr; //If token is tok_identifier, fill the array
static double NumVal; //If token is tok_number, fill this varible with his value

/* gettok - Return the next token from standard input.*/
static int gettok() { 
	static int LastChar = ' ';
	// skip any whitespace
	while (isspace(LastChar)) LastChar = getchar();

	/*To handle the alphanumeric numbers*/

	if (isalpha(LastChar)) { // identifier: [a-zA-Z][a-zA-Z0-9]*
		IdentifierStr = LastChar;
		while (isalnum((LastChar = getchar()))) {
			IdentifierStr += LastChar;
		}
		if (IdentifierStr == "def") return tok_def;
		if (IdentifierStr == "extern") return tok_extern;

		return tok_identifier; //return the type of token
	}

	/*To handle the digit numbers*/

	if (isdigit(LastChar) || LastChar == '.') {    // Number: [0-9.]+
		std::string NumStr;
		do {
			NumStr += LastChar;
			LastChar = getchar();
		} while (isdigit(LastChar) || LastChar == '.');
		NumVal = strtod(NumStr.c_str(), 0); // convert this string in a numeric value

		return tok_number;  // return the type of token
		/*
		this isn’t doing sufficient error checking: it will incorrectly read “1.23.45.67” and handle it as if you typed in “1.23”
		*/
	}

	/*To handle the commets*/

	if (LastChar == '#') {
		// Comment until end of line.
		do {
			LastChar = getchar();
		} while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');

		if (LastChar != EOF) return gettok();
	}
	// Check for end of file.  Don't eat the EOF.
	if (LastChar == EOF) return tok_eof;

	// Otherwise, just return the character as its ascii value.
	int ThisChar = LastChar;
	LastChar = getchar();

	return ThisChar;
}

/*
   Cambios realizado de la segunda sesión:
   Creación del Parser Abstract Syntax Tree (Parse Tree)
*/
namespace {

// ExprAST - Base class for all expression nodes.
class ExprAST{
	public:
	  virtual ~ExprAST() = default; //método sobrescrito por una clase hija: virtual -> clase abstracta pura -> interfaz 
	  virtual Value *codegen() = 0;
};

// NumberExprAST - Expression class for numeric literals like "1.0".
class NumberExprAST : public ExprAST{ //Se recomienda que las herencia sea con clases abstractas
    double Val;
	public:
	 NumberExprAST(double Val): Val(Val)/*Directamente se le asigna el valor*/{; /*this->Val = Val Se reserva el espacio de memoria para luego asignarla*/}
	 Value *codegen() override;
};

/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST: public ExprAST{
	std::string Name;
	public:
	 VariableExprAST(const std::string Name): Name(Name) {}
	 Value *codegen() override;
};

class BinaryExprAST: public ExprAST{
	char Op;
	std::unique_ptr<ExprAST> LHS, RHS;

	public:
	  BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS,
                std::unique_ptr<ExprAST> RHS)
      : Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
	  Value *codegen() override;
};

class CallExprAST: public ExprAST{
	std::string Calle;
	std::vector<std::unique_ptr<ExprAST>> Args;

	public:
	  CallExprAST(const std::string &Calle,
	   std::vector<std::unique_ptr<ExprAST>> Args)
	   : Calle(Calle), Args(std::move(Args)){}
	   Value *codegen() override;
};
class PrototypeAST{
	std::string Name;
	std::vector<std::string> Args;
	public:
	  PrototypeAST(const std::string &Name, std::vector<std::string> Args)
	    :Name(Name), Args(std::move(Args)){}
	  Function *codegen();//Se coloca así ya que la clase es una función
	  const std::string &getName() const{return Name;} //No se pueden modificar los elementos salidos y entrantes del valor al ser constantes
};
class FunctionAST{
	std::unique_ptr<PrototypeAST> Proto;
	std::unique_ptr<ExprAST> Body;

	public:
	 FunctionAST(std::unique_ptr<PrototypeAST> Proto, std::unique_ptr<ExprAST> Body)
	  : Proto(std::move(Proto)), Body(std::move(Body)){}
	 Function *codegen();
};
}
/*
   Parser
*/

/// CurTok/getNextToken - Provide a simple token buffer.  CurTok is the current
/// token the parser is looking at.  getNextToken reads another token from the
/// lexer and updates CurTok with its results
static int CurTok;
static int getNextToken(){return CurTok = gettok();}
static std::map<char, int> BinopPrecedence;

static int GetTokPrecedence(){
	if(!isascii(CurTok)) return -1;

	// Make sure it's a declared binop.
	int TokPrec = BinopPrecedence[CurTok];
	if(TokPrec <= 0) return -1;
	return TokPrec;
}
/// LogError* - These are little helper functions for error handling.
std::unique_ptr<ExprAST> LogError(const char *Str){
	fprintf(stderr, "Error: %s\n",Str);
	return nullptr;
}
std::unique_ptr<PrototypeAST> LogErrorP(const char *Str){
	LogError(Str);
	return nullptr;
}

/// expression
///   ::= primary binoprhs
///
static std::unique_ptr<ExprAST> ParseExpression();

//numexpr ::= number
static std::unique_ptr<ExprAST> ParseNumberExpr(){
	auto Result = std::make_unique<NumberExprAST>(NumVal);
	getNextToken(); // consume the number
	return std::move(Result);
}
//parenexpr ::= '('expression')'
static std::unique_ptr<ExprAST> ParseParenExpr(){
	getNextToken(); // eat (.
	auto V = ParseExpression();
	if(!V) return nullptr;
	if(CurTok != ')')
	  return LogError("Expected ')'");
	getNextToken(); // eat ).
	return V;
}
/*
   Continuación: Sesión Lunes (Sem12)
*/

/// identifierexpr
///   ::= identifier
///   ::= identifier '(' expression* ')'
static std::unique_ptr<ExprAST> ParseIdentifierExpr(){
	std::string IdName = IdentifierStr;

	getNextToken(); // eat identifier.

	if(CurTok != '(') // Simple variable ref
	  return std::make_unique<VariableExprAST>(IdName);
	// Call.
	getNextToken(); // eat (
	std::vector<std::unique_ptr<ExprAST>> Args;
	if (CurTok != ')') 
	  while (true) {
        if (auto Arg = ParseExpression())
          Args.push_back(std::move(Arg));
        else
          return nullptr;
  
        if (CurTok == ')')
          break;
  
        if (CurTok != ',')
          return LogError("Expected ')' or ',' in argument list");
        getNextToken();
    }
	// Eat the ')'
	getNextToken();
	return std::make_unique<CallExprAST>(IdName, std::move(Args));
}

/// primary
///   ::= identifierexpr
///   ::= numberexpr
///   ::= parenexpr
static std::unique_ptr<ExprAST> ParsePrimary() {
  switch (CurTok) {
    case tok_identifier:
      return ParseIdentifierExpr();
    case tok_number:
      return ParseNumberExpr();
    case '(':
      return ParseParenExpr();
    default:
      return LogError("unknown token when expecting an expression");
  }
}

/*Binary Expression Parsing*/

/// binoprhs
///   ::= ('+' primary)*
static std::unique_ptr<ExprAST> ParseBinOpRHS(int ExprPrec,
                                              std::unique_ptr<ExprAST> LHS) {
    // If this is a binop, find its precedence.
    while (true) {
      int TokPrec = GetTokPrecedence();
  
      // If this is a binop that binds at least as tightly as the current binop,
      // consume it, otherwise we are done.
      if (TokPrec < ExprPrec)
        return LHS;
  
      // Okay, we know this is a binop.
      int BinOp = CurTok; // Sabemos que es un operador
      getNextToken(); // eat binop
  
      // Parse the primary expression after the binary operator.
      auto RHS = ParsePrimary();
      if (!RHS)
        return nullptr;
  
      // If BinOp binds less tightly with RHS than the operator after RHS, let
      // the pending operator take RHS as its LHS.
      int NextPrec = GetTokPrecedence();
      if (TokPrec < NextPrec) {
        RHS = ParseBinOpRHS(TokPrec + 1, std::move(RHS));
        if (!RHS)
          return nullptr;
      }
  
      // Merge LHS/RHS.
      LHS =
          std::make_unique<BinaryExprAST>(BinOp, std::move(LHS), std::move(RHS));
    }
}

/// expression
///   ::= primary binoprhs
///
static std::unique_ptr<ExprAST> ParseExpression() {
    auto LHS = ParsePrimary();
    if (!LHS)
      return nullptr;
    
    return ParseBinOpRHS(0, std::move(LHS));
}

/// prototype
///   ::= id '(' id* ')'
static std::unique_ptr<PrototypeAST> ParsePrototype() {
    if (CurTok != tok_identifier)
      return LogErrorP("Expected function name in prototype");
    
    std::string FnName = IdentifierStr;
    getNextToken();
    
    if (CurTok != '(')
      return LogErrorP("Expected '(' in prototype");
    
    // Read the list of argument names.
    std::vector<std::string> ArgNames;
    while (getNextToken() == tok_identifier)
      ArgNames.push_back(IdentifierStr);
    if (CurTok != ')')
      return LogErrorP("Expected ')' in prototype");
    
    // success.
    getNextToken();  // eat ')'.
    
    return std::make_unique<PrototypeAST>(FnName, std::move(ArgNames));
}
/// definition ::= 'def' prototype expression
static std::unique_ptr<FunctionAST> ParseDefinition() {
    getNextToken();  // eat def.
    auto Proto = ParsePrototype();
    if (!Proto) return nullptr;
  
    if (auto E = ParseExpression())
      return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
    return nullptr;
}



/// toplevelexpr ::= expression
static std::unique_ptr<FunctionAST> ParseTopLevelExpr() {
    if (auto E = ParseExpression()) {
      // Make an anonymous proto.
      auto Proto = std::make_unique<PrototypeAST>("__anon_expr", std::vector<std::string>());
      return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
    }
    return nullptr;
}

/// external ::= 'extern' prototype
static std::unique_ptr<PrototypeAST> ParseExtern() {
    getNextToken();  // eat extern.
    return ParsePrototype();
}

/*
Generación de código
*/
static std::unique_ptr<LLVMContext> theContext;
static std::unique_ptr<Module> theModule;
static std::unique_ptr<IRBuilder<>> Builder;
static std::map<std::string, Value*> NamedValues;

Value *LogErrorV(const char *Str){
	LogError(Str);
	return nullptr;
}

Value *NumberExprAST::codegen(){
	return ConstantFP::get(*theContext, APFloat(Val));
}

Value *VariableExprAST::codegen(){
	Value *V = NamedValues[Name];
	if (!V)
      LogErrorV("Unknown variable name");
    return V;
}

Value *BinaryExprAST::codegen(){
	 Value *L = LHS->codegen();
	 Value *R = RHS->codegen();

	if(!L || !R) return nullptr;

	switch (Op){
		case '+':
		  return Builder->CreateFAdd(L, R, "addtmp");
		case '-':
		  return Builder->CreateFSub(L, R, "subtmp");
		case '*':
		  return Builder->CreateFMul(L, R, "multmp");
		case '<':
		  L = Builder->CreateFCmpULT(L, R, "cmptmp");
		  return Builder->CreateUIToFP(L, Type::getDoubleTy(*theContext),
		                                "bolltmp");
		default:
		  return LogErrorV("invalid binary operator");
	}
}

Value *CallExprAST::codegen(){
	Function *CalleeF = theModule->getFunction(Calle);
	if (!CalleeF) return LogErrorV("Unknown function referenced");

	if(CalleeF->arg_size() != Args.size()) return LogErrorV("Incorrect # of arguments passed");

	std::vector<Value*> ArgsV;
	for(unsigned i = 0, e = Args.size(); i !=e; ++i){
		ArgsV.push_back(Args[i]->codegen());
		if(!ArgsV.back())
		  return nullptr;
	} 
	return Builder->CreateCall(CalleeF,ArgsV,"calltmp");
}

Function *PrototypeAST::codegen(){
	std::vector<Type*> Doubles(Args.size(), Type::getDoubleTy(*theContext));
	FunctionType *FT = FunctionType::get(Type::getDoubleTy(*theContext),Doubles,false);

	Function *F = Function::Create(FT, Function::ExternalLinkage,Name,theModule.get());//theModule.get() -> nos retorna el puntero

	unsigned Idx = 0;
	for(auto &Arg : F->args())
	   Arg.setName(Args[Idx++]);
	return F;
}

Function *FunctionAST::codegen(){
	Function *theFunction = theModule->getFunction(Proto->getName());

	if(!theFunction)
	  theFunction = Proto->codegen();
	if(!theFunction)
	  return nullptr;
	BasicBlock *BB = BasicBlock::Create(*theContext, "entry", theFunction);
	Builder->SetInsertPoint(BB);

	NamedValues.clear();
	for(auto &Arg:theFunction->args())
	  NamedValues[std::string(Arg.getName())] = &Arg;
	if(Value *RetVal = Body->codegen()){
		Builder->CreateRet(RetVal);
		verifyFunction(*theFunction);

		return theFunction;
	}
	theFunction->eraseFromParent();
	return nullptr;
}

static void InitializeModule(){
	theContext = std::make_unique<LLVMContext>();
	theModule = std::make_unique<Module>("my cool jit", *theContext);

	Builder = std::make_unique<IRBuilder<>>(*theContext);
}

// initial symbols
static void HandleDefinition(){
	if(auto FnAST = ParseDefinition()){
	   if(auto *FnIR = FnAST->codegen()){
	     fprintf(stderr, "Parsed a function definition.\n");
		 FnIR->print(errs());
		 fprintf(stderr, "\n");
	   }
    }
	else{
		getNextToken();
	}
}
static void HandleExtern(){
	if(auto ProtoAST =ParseExtern()){
		if(auto *FnIR = ProtoAST->codegen()){
	     fprintf(stderr, "Parsed extern.\n");
		 FnIR->print(errs());
		 fprintf(stderr, "\n");
	   }
	}
	else{
		getNextToken();
	}
}

static void HandleTopLevelExpression(){
	if(auto FnAST = ParseTopLevelExpr()){
		if(auto *FnIR = FnAST->codegen()){
	     fprintf(stderr, "Parsed top-level expression.\n");
		 FnIR->print(errs());
		 fprintf(stderr, "\n");
	   }
	}else{
		getNextToken();
	}
}
// top := definition | external | expression | ';'
static void MainLoop() {
    while (true) {
        fprintf(stderr, "ready> ");
        switch (CurTok) {
        case tok_eof:
          return;
        case ';': // ignore top-level semicolons.
          getNextToken();
          break;
        case tok_def:
          HandleDefinition();
          break;
        case tok_extern:
          HandleExtern();
          break;
        default:
          HandleTopLevelExpression();
          break;
        }
    }
}

int main() {
	BinopPrecedence['<'] = 10;
	BinopPrecedence['+'] = 20;
	BinopPrecedence['-'] = 20;
	BinopPrecedence['*'] = 40;
	int tok;

	fprintf(stderr, "ready>");
	getNextToken();

	InitializeModule();
	MainLoop();
	theModule->print(errs(), nullptr);
	
	return 0;
}

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
