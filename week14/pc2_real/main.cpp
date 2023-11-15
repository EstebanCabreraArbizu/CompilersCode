#include <cstdio>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>

//#include "pcdosVisitorImpl.h"
#include "pcdosLexer.h"
#include "pcdosParser.h"
#include "antlr4-runtime.h"


int main(int argc, char** argv) {
    std::ifstream inputFile;
    if (argc > 1) {
        inputFile.open(argv[1]);
    }
    antlr4::ANTLRInputStream input(inputFile.is_open() ? inputFile : std::cin);
    pcdosLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    pcdosParser parser(&tokens);
    auto tree = parser.prog();
    //pcdosVisitorImpl eval;
    //eval.visitProg(tree);
    std::cout<< tree->toStringTree(true) <<std::endl;
    return 0;
}