#include "pcdosVisitorImpl.h"
#include "pcdosParser.h"
#include <any>
#include <llvm/ADT/APFloat.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <memory>
#include <string>

std::any pcdosVisitorImpl::visitProg(pcdosParser::ProgContext *ctx)
{
	std::cout << "visitProg\n";
	// Llama a visitChildren(ctx), que visitará todos los hijos del contexto actual
	return visitChildren(ctx);
}

std::any pcdosVisitorImpl::visitPrintExpr(pcdosParser::PrintExprContext *ctx)
{
	std::cout << "visitPrintExpr\n";
	// Crea una función LLVM temporal con un bloque básico que contiene la expresión a imprimir
	std::vector<double> Args;
	std::vector<llvm::Type *> Doubles(Args.size(),
									  llvm::Type::getDoubleTy(*context));
	llvm::FunctionType *FT = llvm::FunctionType::get(
		llvm::Type::getDoubleTy(*context), Doubles, false);

	F = llvm::Function::Create(
		FT, llvm::Function::ExternalLinkage, "_anon_", module.get());

	llvm::BasicBlock *BB = llvm::BasicBlock::Create(*context, "entry", F);
	builder->SetInsertPoint(BB);
	// La función retorna un valor de la expresión visitando el contexto expr()
	//  y luego crea una instrucción ret con ese valor
	llvm::Value *val = std::any_cast<llvm::Value *>(visit(ctx->expr()));
	builder->CreateRet(val);

	return std::any();
	// return visitChildren(ctx);
}

std::any pcdosVisitorImpl::visitAssign(pcdosParser::AssignContext *ctx)
{
	std::cout << "visitAssign\n";
	return visitChildren(ctx);
}

std::any pcdosVisitorImpl::visitStatdef(pcdosParser::StatdefContext *ctx)
{
	std::cout << "visitStatdef\n";
	return visitChildren(ctx);
}

std::any
pcdosVisitorImpl::visitStatextern(pcdosParser::StatexternContext *ctx)
{
	std::cout << "visitStatextern\n";
	return visitChildren(ctx);
}

std::any pcdosVisitorImpl::visitBlank(pcdosParser::BlankContext *ctx)
{
	std::cout << "visitBlank\n";
	return visitChildren(ctx);
}

std::any pcdosVisitorImpl::visitCall(pcdosParser::CallContext *ctx)
{
	std::cout << "visitCall\n";
	return visitChildren(ctx);
}

std::any pcdosVisitorImpl::visitNumber(pcdosParser::NumberContext *ctx)
{
	std::cout << "visitNumber\n";
	// Obtiene el valor numérico del número y crea un objeto llvm::ConstantFP con ese valor
	auto numVal = std::stod(ctx->NUMBER()->getText());
	llvm::Value *val = llvm::ConstantFP::get(*context, llvm::APFloat(numVal));
	return std::any(val);
	// return visitChildren(ctx);
}

std::any pcdosVisitorImpl::visitMulDiv(pcdosParser::MulDivContext *ctx)
{
	std::cout << "visitMulDiv\n";
	return visitChildren(ctx);
}

std::any pcdosVisitorImpl::visitAddSub(pcdosParser::AddSubContext *ctx)
{
	std::cout << "visitAddSub\n";
	// Obtiene los valores de las expresiones izquierda (L) y derecha (R)
	llvm::Value *L = std::any_cast<llvm::Value *>(visit(ctx->expr(0)));
	llvm::Value *R = std::any_cast<llvm::Value *>(visit(ctx->expr(1)));

	// crea la operación correspondiente (CreateFAdd o CreateFSub) con esos valores.
	if (ctx->op->getType() == pcdosParser::ADD)
	{
		return std::any(builder->CreateFAdd(L, R, "addTemp"));
	}
	else
	{
		return std::any(builder->CreateFSub(L, R, "subTemp"));
	}
	// return visitChildren(ctx);
}

std::any pcdosVisitorImpl::visitParens(pcdosParser::ParensContext *ctx)
{
	std::cout << "visitParens\n";
	return visitChildren(ctx);
}

std::any pcdosVisitorImpl::visitId(pcdosParser::IdContext *ctx)
{
	std::cout << "visitId\n";

	// Obtiene el nombre del identificador (idName)
	std::string idName = ctx->ID()->getText();
	// crea una instrucción AllocaInst para reservar memoria
	llvm::AllocaInst *Alloca = CreateEntryBlockAlloca(idName);
	// lo almacena en el mapa memory
	memory[idName] = Alloca;
	return std::any(Alloca);
	// return visitChildren(ctx);
}

std::any pcdosVisitorImpl::visitProto(pcdosParser::ProtoContext *ctx)
{
	std::cout << "visitProto\n";
	return visitChildren(ctx);
}

std::any pcdosVisitorImpl::visitDef(pcdosParser::DefContext *ctx)
{
	std::cout << "visitDef\n";
	return visitChildren(ctx);
}

std::any pcdosVisitorImpl::visitExtern(pcdosParser::ExternContext *ctx)
{
	std::cout << "visitExtern\n";
	return visitChildren(ctx);
}

// vim: set ts=2:sw=2:et:sts=2:F