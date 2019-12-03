/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     FUNC = 258,
     SYMBOL = 259,
     INT_LITERAL = 260,
     DOUBLE_LITERAL = 261,
     LPAREN = 262,
     RPAREN = 263,
     EOL = 264,
     LET = 265,
     QUIT = 266,
     INT = 267,
     DOUBLE = 268
   };
#endif
/* Tokens.  */
#define FUNC 258
#define SYMBOL 259
#define INT_LITERAL 260
#define DOUBLE_LITERAL 261
#define LPAREN 262
#define RPAREN 263
#define EOL 264
#define LET 265
#define QUIT 266
#define INT 267
#define DOUBLE 268




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 5 "src/ciLisp.y"
{
    double dval;
    char *sval;
    struct ast_node *astNode;
    struct symbol_table_node *symTabNode;
}
/* Line 1529 of yacc.c.  */
#line 82 "/Users/edgar.ramirez136/CLionProjects/CI_LISP/cmake-build-debug/ciLispParser.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

