#include <cstdio>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>

#include "pcdosLexer.h"
#include "pcdosParser.h"
#include "antlr4-runtime.h"
#include "pcdosVisitorImpl.h"

int main(int argc, char **argv)
{
	// Si no se proporciona un argumento, el programa devuelve -1 y termina
	if (argc <= 1)
		return -1;
	std::ifstream is;
	is.open(argv[1]);
	antlr4::ANTLRInputStream input(is);

	// crea un lexer y un parser para el lenguaje "pcdos" a partir del ANTLRInputStream
	pcdosLexer lexer(&input);
	antlr4::CommonTokenStream tokens(&lexer);
	pcdosParser parser(&tokens);
	// El parser analiza el contenido del archivo de entrada y genera un árbol de análisis sintáctico
	auto tree = parser.prog();
	// Se crea un objeto pcdosVisitorImpl y llama al método visitProg del visitor,
	//  Se pasa el árbol de análisis sintáctico generado por el parser
	pcdosVisitorImpl eval;
	eval.visitProg(tree);
	// el visitor llama al método test para imprimir el módulo LLVM generado en un archivo llamado "test.ll".
	eval.test();
	// std::cout << tree->toStringTree(true) << std::endl;
}