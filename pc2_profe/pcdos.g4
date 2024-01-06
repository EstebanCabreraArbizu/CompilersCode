grammar pcdos;

/* Syntax analizer */

//Puede tener uno o más estados
prog: stat+ ;


stat
	: expr NEWLINE          # printExpr //Expresión seguido de un salto de línea
	| ID '=' expr NEWLINE   # assign //Una asignación
	| definition NEWLINE    # statdef //Una función
	| external NEWLINE      # statextern //Una declaración externa
	| NEWLINE               # blank // Un salto de líena o espacio en blanco
	;

expr
	: expr op=('*'|'/') expr   # MulDiv //Operación de multiplicación o división
	| expr op=('+'|'-') expr   # AddSub //Operación de suma o resta
	| NUMBER                   # Number //Un número
	| ID '(' expr* ')'         # Call // Una llamada a una función
	| ID                       # Id // Una varible
	| '(' expr ')'             # Parens //Una expresión entre paréntesis
	;

prototype //Prototipo de función
	: ID '(' ID* ')'           # Proto //Un identificador seguido de varios identificadores entre paréntesis
	;

definition // Definición de una función
	: 'def' prototype expr     # Def // def + un prototipo y una expresión
	;

external // Declaración externa
	: 'extern' prototype       # Extern // extern + pototipo
	;

/* Lexical analizer */

MUL: '*' ;
DIV: '/' ;
ADD: '+' ;
SUB: '-' ;

DEF    : 'def' ;
EXTERN : 'extern' ;
ID     : [a-zA-Z]+ ;
NUMBER : [+-]?([0-9]*[.])?[0-9]+ ; //Representa un número, puede ser positivo o negativo, con o sin parte decimal
NEWLINE: '\r'? '\n' ; //Define un salto de línea
COMMENT: [#].*? NEWLINE -> skip ; //Ignora los comentarios
WS     : [ \t]+ -> skip ; //Ignora los espacios en blanco