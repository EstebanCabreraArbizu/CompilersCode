parser grammar ExprParser;
options { tokenVocab=ExprLexer; }

numberexpr : NUM {
    $$ = new NumberNode($NUM.text);
};
parenexpr : LPAREN expression RPAREN;
identifierexpr
    : ID
    | ID LPAREN expression* RPAREN
    ;

primary
    : identifierexpr
    | numberexpr
    | parenexpr
    ;

binoprhs
    : (PLUS primary)*;

expression: primary binoprhs;

prototype
    : ID LPAREN ID* RPAREN
    ;

definition: DEF prototype expression;

toplevelexpr: expression;

external: EXTERN prototype;

top
    : definition
    | external
    | expression
    | SEMICOLON
    ;