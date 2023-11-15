#include <cstdio>
#include <fstream>
#include <string>

#include "EvalVisitorImpl.h"
#include "antlr4-runtime.h"
#include "LabeledExprLexer.h"
#include "LabeledExprParser.h"

/*import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.tree.*;
import java.io.FileInputStream;
import java.io.InputStream;
*/

int main(int argc, char** argv) {
    std::ifstream inputFile;
    if (argc > 1) {
        inputFile.open(argv[1]);
    }
    antlr4::ANTLRInputStream input(inputFile.is_open() ? inputFile : std::cin);
    LabeledExprLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    LabeledExprParser parser(&tokens);
    auto tree = parser.prog();
    EvalVisitorImpl eval;
    eval.visitProg(tree);
    return 0;
}
//clang++ -std=c++17  -I $ANTLR4I -L $ANTLR4L -lantlr4-runtime  *.cpp
//g++ *.cpp -I $ANTLR4I -L $ANTLR4L -lantlr4-runtime
//cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=1
//cmake --build build -j8
