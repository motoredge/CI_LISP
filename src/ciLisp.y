%{
    #include "ciLisp.h"
%}

%union {
    double dval;
    char *sval;
    struct ast_node *astNode;
    struct symbol_table_node *symTabNode;
};


%token <sval> FUNC SYMBOL
%token <dval> INT DOUBLE
%token LPAREN RPAREN EOL LET QUIT

%type <astNode> s_expr f_expr number symbol
%type <symTabNode> let_section let_list let_element
%type <type> int double

%%

program:
    s_expr EOL {
        fprintf(stderr, "yacc: program ::= s_expr EOL\n");
        if ($1) {
            printRetVal(eval($1));
            freeNode($1);
        }
    };

s_expr:
    number {
        fprintf(stderr, "yacc: s_expr ::= number\n");
        $$ = $1;
    }
    | f_expr {
        $$ = $1;
    }
    | QUIT {
        fprintf(stderr, "yacc: s_expr ::= QUIT\n");
        exit(EXIT_SUCCESS);
    }
    | error {
        fprintf(stderr, "yacc: s_expr ::= error\n");
        yyerror("unexpected token");
        $$ = NULL;
    }
    | symbol {
        fprintf(stderr, "yacc: s_expr ::= symbol\n");
        yyerror("unexpected token");
        $$ = $1;
    }
    | LPAREN let_section s_expr RPAREN {
        fprintf(stderr, "yacc: s_expr ::= LPAREN let_section s_expr RPAREN\n");
        $$ = setSymbolTable($2, $3);
    };

let_section :
     LPAREN let_list RPAREN {
        fprintf(stderr, "yacc: s_expr ::= LPAREN let_list RPAREN\n");
        $$=$2;
    };

let_list :
    LET let_element {
        fprintf(stderr, "yacc: s_expr ::= LPAREN FUNC expr RPAREN\n");
        $$=$2;
    }
    | let_list let_element {
        fprintf(stderr, "yacc: s_expr ::= let_list let_element\n");
        $$ = addSymbolToList($1,$2);
    };

let_element :
    LPAREN SYMBOL s_expr RPAREN {
        fprintf(stderr, "yacc: let_element ::= LPAREN SYMBOL s_expr RPAREN\n");
        $$ = createSymbolTableNode($2,$3);
    };
    | LPAREN type SYMBOL s_expr RPAREN {
        fprintf(stderr, "yacc: let_element ::= LPAREN SYMBOL s_expr RPAREN\n");
        $$ = createSymbolTableNode($2,$3,$4);
    };

number:
    INT_LITERAL {
        fprintf(stderr, "yacc: number ::= INT\n");
        $$ = createNumberNode($2, INT_TYPE);
    }
    | DOUBLE_LITERAL {
        fprintf(stderr, "yacc: number ::= DOUBLE\n");
        $$ = createNumberNode($2, DOUBLE_TYPE);
    | type INT_LITERAL {
        fprintf(stderr, "yacc: number ::= INT\n");
        $$ = createNumberNode($2, $1);
    }
    | type DOUBLE_LITERAL {
        fprintf(stderr, "yacc: number ::= DOUBLE\n");
        $$ = createNumberNode($2, $1);
    };

symbol:
    SYMBOL {
        fprintf(stderr, "yacc: symbol ::= SYMBOL\n");
        $$ = createSymbolNode($1);
    };

type:
    INT {
        fprintf(stderr, "yacc: type ::= INT\n");
        $$ = $1;
    };
    | DOUBLE
    {
        fprintf(stderr, "yacc: type ::= DOUBLE\n");
        $$ = $1;
    };

f_expr:
    LPAREN FUNC s_expr RPAREN {
        fprintf(stderr, "yacc: s_expr ::= LPAREN FUNC expr RPAREN\n");
        $$ = createFunctionNode($2, $3, NULL);
    }
    | LPAREN FUNC s_expr s_expr RPAREN {
        fprintf(stderr, "yacc: s_expr ::= LPAREN FUNC expr expr RPAREN\n");
        $$ = createFunctionNode($2, $3, $4);
    };
%%

