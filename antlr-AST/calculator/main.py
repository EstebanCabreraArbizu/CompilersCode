from antlr4 import *
from CalcLexer import CalcLexer
from CalcParser import CalcParser
from calc_evaluator import CalcEvaluator

def main():
    expression = input("Enter an expression: ")
    input_stream = InputStream(expression)
    lexer = CalcLexer(input_stream)
    token_stream = CommonTokenStream(lexer)
    parser = CalcParser(token_stream)
    tree = parser.expr()

    calc_evaluator = CalcEvaluator()