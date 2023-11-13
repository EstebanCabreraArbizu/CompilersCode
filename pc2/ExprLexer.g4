lexer grammar ExprLexer;

ID : [a-zA-Z][a-zA-Z0-9]*;
NUM: [0-9]+;
WS: [ \t\n\r]+ -> skip ;

LPAREN: '(';
RPAREN: ')';
PLUS: '+';
DEF: 'def';
EXTERN: 'extern';
SEMICOLON: ';';