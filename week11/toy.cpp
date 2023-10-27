#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

enum Token {
	tok_eof = -1,
	tok_def = -2,
	tok_extern = -3,

	tok_identifier = -4,
	tok_number = -5,
};

static std::string IdentifierStr;
static double NumVal;

static int gettok() {
	static int LastChar = ' ';

	while (isspace(LastChar)) LastChar = getchar();

	if (isalpha(LastChar)) {
		IdentifierStr = LastChar;
		while (isalnum((LastChar = getchar()))) {
			IdentifierStr += LastChar;
		}
		if (IdentifierStr == "def") return tok_def;
		if (IdentifierStr == "extern") return tok_extern;

		return tok_identifier;
	}

	if (isdigit(LastChar) || LastChar == '.') {
		std::string NumStr;
		do {
			NumStr += LastChar;
			LastChar = getchar();
		} while (isdigit(LastChar) || LastChar == '.');
		NumVal = strtod(NumStr.c_str(), 0);

		return tok_number;
	}

	if (LastChar == '#') {
		do {
			LastChar = getchar();
		} while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');

		if (LastChar != EOF) return gettok();
	}

	if (LastChar == EOF) return tok_eof;

	int ThisChar = LastChar;
	LastChar = getchar();

	return ThisChar;
}
/*Cambios realizado de la segunda sesión*/
namespace{

class ExprAST{
	public:
	  virtual ~ExprAST() = default; //método sobrescrito por una clase hija: virtual -> clase abstracta pura -> interfaz 
};
class NumberExprAST : public ExprAST{ //Se recomienda que las herencia sea con clases abstractas
    double Val;
	public:
	 NumberExprAST(double Val): Val(Val)/*Directamente se le asigna el valor*/{; /*this->Val = Val Se reserva el espacio de memoria para luego asignarla*/}
};
class VariableExprAST: public ExprAST{
	std::string Name;
	public:
	 VariableExprAST(const std::string Name): Name(Name) {}
};
class BinaryExprAST: public ExprAST{
	char Op;
	std::unique_ptr<ExprAST> LHS, RHS;

	public:
	  BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS, std::unique_ptr<ExprAST> RHS): Op(Op), LHS(std::move(LHS), RHS(std::move(RHS)) ){}
};

class CallExprAST: publiC ExprAST{
	std::string Calle;
	std::vector<std::unique_ptr<ExprAST>> Args;

	public:
	  CallExprAST(const std::string &Calle,
	   std::vector<std::unique_ptr<ExprAST>> Args)
	   : Calle(Calle), Args(std::move(Args)){}
};
class PrototypeAST{
	std::string Name;
	std::vector<std::string> Args;
	public:
	  PrototypeAST(const std::string &Name, std::vector<std::string> Args)
	    :Name(Name), Args(std::move(Args)){}
	  const std::string &getName() const{return Name;} //No se pueden modificar los elementos salidos y entrantes del valor al ser constantes
};
class FuncionAST{
	std::unique_ptr<PrototypeAST> Proto;
	std::unique_ptr<ExprAST> Body;

	public:
	 FuncionAST(std::unique_ptr<PrototypeAST> Proto, std::unique_ptr<ExprAST> Body)
	  : Proto(std::move(Proto)), Body(std::move(Body)){}
};
}

static int CurTok;
static int getNextToken(){return CurTok = gettok();}
static std::map<char, int> BinopPrecedence;

static int GetTokPrecedence(){
	if(!isascii(CurTok)) return -1;

	int TokPrec = BinopPrecedence[CurTok];
	if(TokPrec <= 0) return -1;
	return TokPrec;
}
std::unique_ptr<ExprAST> LogError(const char *Str){
	fprintf(stderr, "Error: %s\n",Str);
}
std::unique_ptr<PrototypeAST> LogErrorp(const char *Str){
	LogError(Str);
	return nullptr;
}
static std::unique_ptr<ExprAST> ParseExpression();
//numexpr ::= number
static std::unique_ptr<ExprAST> ParseNumberExpr(){
	auto Result = std::make_unique<NumberExprAST>(NumVal);
	getNextToken();
	return std::move(Result);
}
//pareexpr ::= '('expression')'
static std::unique_ptr<ExprAST> ParseParenExpr(){
	getNextToken();
	auto V = ParseExpression();
	if(!V) return nullptr;
	if(CurTok != ')')
	  return LogError("Expected ')'"); 
	getNextToken();
	return V;
}
int main() {
	int tok;
	while ((tok = gettok()) != tok_eof) {
		switch (tok){
			case tok_number:
			    std::cout << "Number: "<<NumVal<<std::endl;
			break;
		}
		std::cout << tok << " -> \n";
	}
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
