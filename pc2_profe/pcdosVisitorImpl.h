#ifndef __PCDOS_VISITOR_IMPL__
#define __PCDOS_VISITOR_IMPL__

// Cabeceras para el parser y visitor de Antlr4
#include "pcdosBaseVisitor.h"
#include "pcdosParser.h"

// Cabeceras para generar código LLVM
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"

#include <algorithm>
#include <any>
#include <iostream>
#include <llvm/ADT/StringRef.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Type.h>
#include <llvm/Support/raw_ostream.h>
#include <map>
#include <memory>
#include <string>
#include <system_error>

/* added the Impl at the end of the class to avoid it being .gitignored sorry */
// Clase que hereda pcdosBaseVisitor para sobreescribir sus métodos
class pcdosVisitorImpl : pcdosBaseVisitor
{
public:
	pcdosVisitorImpl()
		: // Creación de contexto, módulo y un constructor IR
		  context(std::make_unique<llvm::LLVMContext>()),
		  module(std::make_unique<llvm::Module>("LaPC2", *context)),
		  builder(std::make_unique<llvm::IRBuilder<>>(*context))
	{
	}
	// Sobrescribe los métodos de visita generados por ANTLR4 para cada regla definida en la gramática
	virtual std::any visitProg(pcdosParser::ProgContext *ctx) override;
	virtual std::any visitPrintExpr(pcdosParser::PrintExprContext *ctx) override;
	virtual std::any visitAssign(pcdosParser::AssignContext *ctx) override;
	virtual std::any visitStatdef(pcdosParser::StatdefContext *ctx) override;
	virtual std::any
	visitStatextern(pcdosParser::StatexternContext *ctx) override;
	virtual std::any visitBlank(pcdosParser::BlankContext *ctx) override;
	virtual std::any visitCall(pcdosParser::CallContext *ctx) override;
	virtual std::any visitNumber(pcdosParser::NumberContext *ctx) override;
	virtual std::any visitMulDiv(pcdosParser::MulDivContext *ctx) override;
	virtual std::any visitAddSub(pcdosParser::AddSubContext *ctx) override;
	virtual std::any visitParens(pcdosParser::ParensContext *ctx) override;
	virtual std::any visitId(pcdosParser::IdContext *ctx) override;
	virtual std::any visitProto(pcdosParser::ProtoContext *ctx) override;
	virtual std::any visitDef(pcdosParser::DefContext *ctx) override;
	virtual std::any visitExtern(pcdosParser::ExternContext *ctx) override;
	void test()
	{
		// Generación de un test.ll para guardar le código LLVM generado del modulo
		std::error_code error;
		llvm::raw_fd_stream outLL("test.ll", error);
		module->print(outLL, nullptr);
	}

private:
	std::map<std::string, std::any> memory;
	std::unique_ptr<llvm::LLVMContext> context;
	std::unique_ptr<llvm::Module> module;
	std::unique_ptr<llvm::IRBuilder<>> builder;

	// aux
	llvm::Function *F;

	// aux methods
	// Método auxiliar que crea una instrucción de asignación de memoria
	// para una variable local en el bloque de entrada de una función LLVM

	//devuelve un puntero a una instrucción AllocaInst de LLVM. 
	//llvm::StringRef -> nombre de la variable que se va a asignar en la pila
	llvm::AllocaInst *CreateEntryBlockAlloca(llvm::StringRef varName)
	{
		//Crea un objeto IRBuilder llamado TmpB.
		//construye un instrucción IR dentro de un bloque específico (EntryBlock) de una función LLVM (F)
		//Se coloca al principio del bloque de entrada (begin()).
		llvm::IRBuilder<> TmpB(&F->getEntryBlock(), F->getEntryBlock().begin());
		return TmpB.CreateAlloca(llvm::Type::getDoubleTy(*context), nullptr,
								 varName);
		// Crea una instrucción que reserva espacio en la pila para una variable local de tipo double
		// en el bloque de entrada de una función LLVM
	}
};

#endif

// vim: set ts=2:sw=2:et:sts=2: