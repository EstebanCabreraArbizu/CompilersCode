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

int main(int argc, char **argv) {
  if (argc <= 1)
    return -1;
  std::ifstream is;
  is.open(argv[1]);
	antlr4::ANTLRInputStream input(is);
  LabeledExprLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);
  LabeledExprParser parser(&tokens);
  auto tree = parser.prog();
  EvalVisitorImpl eval;
  eval.visitProg(tree);
}
