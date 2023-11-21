grammar calc;
//Parser
expr: term (('+'|'-')term)*; //acepta term y luego (+|-)term   
term: factor (('*'|'/')factor)*;
factor: NUMBER |
        '('expr')'|
        ('-'|'+') factor;

//lexer-tokens
NUMBER: ('-')?[0-9]+([.][0-9]+)?;
WS: [\t\r\n]+ -> skip;