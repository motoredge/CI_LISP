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
%token <dval> INT_LITERAL DOUBLE_LITERAL
%token LPAREN RPAREN EOL LET QUIT INT DOUBLE

%type <astNode> s_expr f_expr number symbol
%type <symTabNode> let_section let_list let_element

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
        $$ = createSymbolTableNode($2,$3, DOUBLE_TYPE);
    }
    | LPAREN INT SYMBOL s_expr RPAREN {
        fprintf(stderr, "yacc: let_element ::= LPAREN SYMBOL s_expr RPAREN\n");
        $$ = createSymbolTableNode($3,$4,INT_TYPE);
    }
    | LPAREN DOUBLE SYMBOL s_expr RPAREN {
        fprintf(stderr, "yacc: let_element ::= LPAREN SYMBOL s_expr RPAREN\n");
        $$ = createSymbolTableNode($3,$4,DOUBLE_TYPE);
    };

number:
    INT_LITERAL {
        fprintf(stderr, "yacc: number ::= INT\n");
        $$ = createNumberNode($1, INT_TYPE);
    }
    | DOUBLE_LITERAL {
        fprintf(stderr, "yacc: number ::= DOUBLE\n");
        $$ = createNumberNode($1, DOUBLE_TYPE);
    };

symbol:
    SYMBOL {
        fprintf(stderr, "yacc: symbol ::= SYMBOL\n");
        $$ = createSymbolNode($1);
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

