
/* A Bison parser, made by GNU Bison 2.4.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* "%code requires" blocks.  */


#include <iostream>
#include <sstream>
#include <string>
#include <climits>
#include "Script.h"
#undef ERROR




/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     IDENTIFIER = 258,
     FOR_TO = 259,
     FOR_STEP = 260,
     EQUALS = 261,
     INTEGER = 262,
     STRING = 263,
     INTEGER_ARRAY = 264,
     ERROR = 265,
     OR = 266,
     AND = 267,
     GREATEREQ = 268,
     GREATER = 269,
     LOWEREQ = 270,
     LOWER = 271,
     NEQ = 272,
     POS = 273,
     NEG = 274,
     ITOA = 275,
     ATOI = 276,
     LCHK = 277,
     FCHK = 278,
     EVAL = 279
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{


	ulong position;



} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif




/* "%code provides" blocks.  */


	int commandPreParser_yyparse(
		std::wstringstream *stream,
		NONS_Script *script,
		ulong *res
	);
	int commandPreParser_yylex(
		YYSTYPE *yylval,
		std::wstringstream *stream,
		NONS_Script *script
	);
	void commandPreParser_yyerror(
		std::wstringstream *stream,
		NONS_Script *script,
		ulong *res,
		char const *
	);
	extern int commandPreParser_yydebug;



