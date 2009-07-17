
/* A Bison parser, made by GNU Bison 2.4.1.  */

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


	#include <set>
	#include <vector>
	#include "../IO_System/SAR/FileLog.h"
	class NONS_VariableMember;
	struct NONS_VariableStore;
	struct wstrCmp;




/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INTEGER = 258,
     STRING = 259,
     INTEGER_ARRAY = 260,
     ERROR = 261,
     OR = 262,
     AND = 263,
     GREATEREQ = 264,
     GREATER = 265,
     LOWEREQ = 266,
     LOWER = 267,
     NEQ = 268,
     EQUALS = 269,
     POS = 270,
     NEG = 271,
     ITOA = 272,
     ATOI = 273,
     LCHK = 274,
     FCHK = 275,
     EVAL = 276
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{


	NONS_VariableMember *obj;



} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif




/* "%code provides" blocks.  */


	#include <sstream>
	int expressionParser_yyparse(
		std::wstringstream *stream,
		NONS_VariableStore *store,
		NONS_FileLog *filelog,
		long *result,
		bool invert_terms,
		std::vector<long> *array_decl,
		NONS_VariableMember **retrievedVar,
		std::wstring *string
	);
	int expressionParser_yylex(
		YYSTYPE *yylval,
		std::wstringstream *stream,
		NONS_VariableStore *store,
		NONS_VariableMember **retrievedVar
	);
	void expressionParser_yyerror(
		std::wstringstream *,
		NONS_VariableStore *,
		NONS_FileLog *,
		long *,
		bool,
		std::vector<long> *,
		NONS_VariableMember **,
		std::wstring *string,
		char const *
	);
	long atol(std::wstring &str);



