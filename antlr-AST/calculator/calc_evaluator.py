from antlr4 import *
from CalcLexer import CalcLexer
from CalcParser import CalcParser
from CalcListener import CalcListener

class CalcEvaluator(CalcListener):
    def __init__(selg):
        self.stack = []
    def exitExpr(self, ctx):
        if ctx.getChildCount() ==3:
            #Obtiene los elementos de la derecha e izquierda
            right = self.stack.pop() 
            left = self.stack.pop()
            op = ctx.getChild(1).getText()
            #Hace las operaciones de acuerdo al operdor (+|-)
            if op == '+':
                self.stack.append(left + right)
            elif op == '-':
                self.stack.append(left-right)
    def exitTerm(self,ctx):
        if ctx.getChildCount()==3:
            right = self.stack.pop()
            left = self.stack.pop()
            op = ctx.getChild(1).getText()
            #Hace las operaciones de acuerdo al operdor (*|/)
            if op == '*':
                self.satck.append(left * right)
            elif op == '/':
                self.stack.append(left / right)
    def exitFactor(self,ctx):
        #Si el contexto tiene un solo hijo (es decir, ctx.getChildCount() == 1),
        #se asume que este hijo es un número y se agrega al stack convertido a tipo float.
        if ctx.getChildCount() == 1:
            self.stack.append(float(ctx.getText()))
        #Si el contexto tiene dos hijos (ctx.getChildCount() == 2), 
        #se asume que es un número negativo y se cambia el signo del número
        # que está en el tope del stack, ya que en una expresión matemática,
        #  el operador - precediendo a un número lo convierte en negativo
        elif ctx.getChildCount() == 2:
            self.stack.append(-self.stack.pop())
    def getValue(self):
        return self.stack[0]

