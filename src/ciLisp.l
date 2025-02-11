%option noyywrap
%option nounput
%option noinput

%{
    #include "ciLisp.h"
%}

digit [0-9]
letter [a-zA-Z]
int_literal [+-]?{digit}+
double_literal [+-]?{digit}+\.{digit}*
symbol {letter}+
func "neg"|"abs"|"exp"|"sqrt"|"add"|"sub"|"mult"|"div"|"remainder"|"log"|"pow"|"max"|"min"|"exp2"|"cbrt"|"hypot"|"print"

%%

{int_literal} {
    yylval.dval = strtod(yytext, NULL);
    fprintf(stderr, "lex: INT_LITERAL dval = %lf\n", yylval.dval);
    return INT_LITERAL;
    }

{double_literal} {
    yylval.dval = strtod(yytext, NULL);
    fprintf(stderr, "lex: DOUBLE_LITERAL dval = %lf\n", yylval.dval);
    return DOUBLE_LITERAL;
    }

"let" {
    fprintf(stderr, "lex: LET\n");
    return LET;
    }

"quit" {
    fprintf(stderr, "lex: QUIT\n");
    return QUIT;
    }

"int" {
      fprintf(stderr, "lex: INT\n");
      return INT;
    }

"double" {
     fprintf(stderr, "lex: DOUBLE\n");
     return DOUBLE;
    }

{func} {
    yylval.sval = (char *) malloc(strlen(yytext)*sizeof(char)+1);
    strcpy(yylval.sval, yytext);
    fprintf(stderr, "lex: FUNC sval = %s\n", yylval.sval);
    return FUNC;
    }

{symbol} {
    yylval.sval = (char *) malloc(strlen(yytext)*sizeof(char)+1);
    strcpy(yylval.sval, yytext);
    fprintf(stderr, "lex: SYMBOL sval = %s\n", yylval.sval);
    return SYMBOL;
}

"(" {
    fprintf(stderr, "lex: LPAREN\n");
    return LPAREN;
    }

")" {
    fprintf(stderr, "lex: RPAREN\n");
    return RPAREN;
    }

[\n] {
    fprintf(stderr, "lex: EOL\n");
    YY_FLUSH_BUFFER;
    return EOL;
    }

[ |\t] ; /* skip whitespace */

. { // anything else
    printf("ERROR: invalid character: >>%s<<\n", yytext);
    }

%%

/*
 * DO NOT CHANGE THE FOLLOWING CODE!
 */
int main(void) {

    freopen("/dev/null", "w", stderr); // except for this line that can be uncommented to throw away debug printouts

    char *s_expr_str = NULL;
    size_t s_expr_str_len = 0;
    YY_BUFFER_STATE buffer;
    while (true) {
        printf("\n> ");
        getline(&s_expr_str, &s_expr_str_len, stdin);
        s_expr_str[s_expr_str_len++] = '\0';
        s_expr_str[s_expr_str_len++] = '\0';
        buffer = yy_scan_buffer(s_expr_str, s_expr_str_len);
        yyparse();
        yy_delete_buffer(buffer);
    }
}