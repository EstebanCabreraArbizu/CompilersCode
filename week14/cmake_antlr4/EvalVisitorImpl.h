#pragma once

#include "LabeledExprBaseVisitor.h"
#include "LabeledExprParser.h"
#include <any>
#include <iostream>
#include <map>
#include <string>

/* added the Impl at the end of the class to avoid it being .gitignored sorry */
class EvalVisitorImpl: LabeledExprBaseVisitor {
	std::map<std::string, std::any> memory;

public:
	/* ID '=' expr NEWLINE */
	std::any visitAssign(LabeledExprParser::AssignContext* ctx) {
		std::string id = ctx->ID()->getText();
		auto value = visit(ctx->expr());
		memory[id] = value;
		return value;
	}

	/* expr NEWLINE */
	std::any visitPrintExpr(LabeledExprParser::PrintExprContext* ctx) {
		auto value = visit(ctx->expr());
		std::cout << std::any_cast<int>(value) << '\n';
		return std::any();
	}

	/* INT */
	std::any visitInt(LabeledExprParser::IntContext* ctx) {
		return std::any(std::stoi(ctx->INT()->getText()));
	}

	/* ID */
	std::any visitId(LabeledExprParser::IdContext* ctx) {
		std::string id = ctx->ID()->getText();
		if (memory.count(id)) return memory[id];
		return std::any();
	}

	/* expr op=('*'|'/') expr */
	std::any visitMulDiv(LabeledExprParser::MulDivContext* ctx) {
		int  left = std::any_cast<int>(visit(ctx->expr(0)));
		int right = std::any_cast<int>(visit(ctx->expr(1)));
		if (ctx->op->getType() == LabeledExprParser::MUL) {
			return left * right;
		} else {
			return  left / right;
		}
		//return std::any(resp);
	}

	/* expr op=('+'|'-') expr */
	std::any visitAddSub(LabeledExprParser::AddSubContext* ctx) {
		auto left = std::any_cast<int>(visit(ctx->expr(0)));
		auto right = std::any_cast<int>(visit(ctx->expr(1)));
		int resp;
		if (ctx->op->getType() == LabeledExprParser::ADD) {
			return left + right;
		} else {
			return left - right;
		}
		//return std::any(resp);
	}


	/* '(' expr ')' */
	std::any visitParens(LabeledExprParser::ParensContext* ctx) {
		return visit(ctx->expr());
	}

	std::any visitProg(LabeledExprParser::ProgContext* ctx) {
		return visitChildren(ctx);
	}
};