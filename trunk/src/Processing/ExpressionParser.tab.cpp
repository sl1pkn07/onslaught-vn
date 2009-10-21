
/* A Bison parser, made by GNU Bison 2.4.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse         expressionParser_yyparse
#define yylex           expressionParser_yylex
#define yyerror         expressionParser_yyerror
#define yylval          expressionParser_yylval
#define yychar          expressionParser_yychar
#define yydebug         expressionParser_yydebug
#define yynerrs         expressionParser_yynerrs


/* Copy the first part of user declarations.  */


#include "VariableStore.h"
#include "ScriptInterpreter.h"
#include "../IO_System/IOFunctions.h"

void freeVM(NONS_VariableMember *p);



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

/* "%code requires" blocks.  */


#include <set>
#include <vector>
#include "../IO_System/SAR/FileLog.h"
class NONS_VariableMember;
struct NONS_VariableStore;
struct wstrCmp;
#undef ERROR




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




/* Copy the second part of user declarations.  */



#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  43
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   245

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  33
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  11
/* YYNRULES -- Number of rules.  */
#define YYNRULES  48
/* YYNRULES -- Number of states.  */
#define YYNSTATES  99

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   276

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,    29,    28,     2,     2,
      31,    32,    17,    15,     2,    16,     2,    18,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    30,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    21,     2,    22,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      19,    20,    23,    24,    25,    26,    27
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    11,    13,    15,    17,
      19,    24,    29,    34,    39,    41,    43,    45,    50,    54,
      58,    63,    66,    71,    75,    79,    83,    87,    91,    93,
      97,   101,   104,   107,   111,   114,   119,   122,   127,   131,
     135,   139,   143,   147,   151,   154,   159,   162,   166
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      34,     0,    -1,    35,    -1,    37,    -1,    38,    -1,     6,
      -1,    36,    -1,    39,    -1,    40,    -1,     3,    -1,    27,
      31,    37,    32,    -1,    27,    31,    38,    32,    -1,    24,
      31,    37,    32,    -1,    24,    31,    38,    32,    -1,    41,
      -1,     4,    -1,    42,    -1,    23,    31,    35,    32,    -1,
      37,    15,    37,    -1,    38,    15,    37,    -1,    39,    21,
      35,    22,    -1,    30,    36,    -1,    30,    31,    35,    32,
      -1,    30,     1,    21,    -1,    35,    15,    35,    -1,    35,
      16,    35,    -1,    35,    17,    35,    -1,    35,    18,    35,
      -1,    43,    -1,    35,     7,    35,    -1,    35,     8,    35,
      -1,    15,    35,    -1,    16,    35,    -1,    31,    35,    32,
      -1,    28,    36,    -1,    28,    31,    35,    32,    -1,    29,
      36,    -1,    29,    31,    35,    32,    -1,    35,    14,    35,
      -1,    35,    13,    35,    -1,    35,    12,    35,    -1,    35,
       9,    35,    -1,    35,    10,    35,    -1,    35,    11,    35,
      -1,    26,    37,    -1,    26,    31,    38,    32,    -1,    25,
      37,    -1,    25,    17,     3,    -1,    25,    31,    38,    32,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   113,   113,   124,   135,   146,   151,   154,   170,   175,
     178,   189,   200,   212,   224,   231,   234,   237,   246,   251,
     258,   276,   288,   300,   308,   314,   320,   326,   338,   347,
     363,   379,   384,   389,   394,   403,   414,   423,   434,   441,
     448,   455,   462,   469,   476,   482,   488,   494,   501
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "INTEGER", "STRING", "INTEGER_ARRAY",
  "ERROR", "OR", "AND", "GREATEREQ", "GREATER", "LOWEREQ", "LOWER", "NEQ",
  "EQUALS", "'+'", "'-'", "'*'", "'/'", "POS", "NEG", "'['", "']'", "ITOA",
  "ATOI", "LCHK", "FCHK", "EVAL", "'%'", "'$'", "'?'", "'('", "')'",
  "$accept", "eval", "expr", "integer", "string", "string_concatenation",
  "integer_array", "operation", "integer_dereference",
  "string_dereference", "boolean_operation", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,    43,    45,    42,    47,   270,
     271,    91,    93,   272,   273,   274,   275,   276,    37,    36,
      63,    40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    33,    34,    34,    34,    34,    35,    35,    35,    36,
      36,    36,    36,    36,    36,    37,    37,    37,    38,    38,
      39,    39,    39,    39,    40,    40,    40,    40,    40,    40,
      40,    40,    40,    40,    41,    41,    42,    42,    43,    43,
      43,    43,    43,    43,    43,    43,    43,    43,    43
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       4,     4,     4,     4,     1,     1,     1,     4,     3,     3,
       4,     2,     4,     3,     3,     3,     3,     3,     1,     3,
       3,     2,     2,     3,     2,     4,     2,     4,     3,     3,
       3,     3,     3,     3,     2,     4,     2,     3,     4
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     9,    15,     5,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     2,     6,     3,     4,
       7,     8,    14,    16,    28,    31,    32,     0,     0,     0,
       0,    46,     0,    44,     0,     0,    34,     0,    36,     0,
       0,    21,     0,     1,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    47,     0,     0,     0,     0,     0,     0,     0,
      23,     0,    33,    29,    30,    41,    42,    43,    40,    39,
      38,    24,    25,    26,    27,    18,    19,     0,    17,    12,
      13,    48,    45,    10,    11,    35,    37,    22,    20
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,    15,    16,    17,    63,    19,    20,    21,    22,    23,
      24
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -29
static const yytype_int16 yypact[] =
{
      68,   -29,   -29,   -29,    85,    85,   -28,   -26,    18,    98,
     -15,    -1,    51,     9,    85,    14,   227,   -29,    17,    33,
      30,   -29,   -29,   -29,   -29,   -29,   -29,    85,     2,    50,
       2,   -29,     2,   -29,     2,    85,   -29,    85,   -29,    37,
      85,   -29,   123,   -29,    85,    85,    85,    85,    85,    85,
      85,    85,    85,    85,    85,    85,     2,     2,    85,   135,
     -14,   -11,   -29,    17,    -8,    71,    72,    74,   161,   173,
     -29,   199,   -29,   148,   185,    28,    28,    28,    28,    28,
      28,    21,    21,   -29,   -29,   -29,   -29,   211,   -29,   -29,
     -29,   -29,   -29,   -29,   -29,   -29,   -29,   -29,   -29
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -29,   -29,    15,   106,     0,   -17,   -29,   -29,   -29,   -29,
     -29
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      18,    56,     1,    27,    57,    28,     2,    57,    31,    33,
      39,    61,     1,    64,    43,    65,    34,    67,    89,    25,
      26,    90,     2,     7,    91,     6,    10,    11,    60,    42,
      35,    12,    56,     7,    66,    29,    10,    11,    54,    55,
      40,     6,    59,    52,    53,    54,    55,    12,    57,    30,
      68,    58,    69,    62,     1,    71,    85,    86,    70,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,     1,     2,    87,     3,     7,     0,     0,    10,    11,
       0,     0,    37,     4,     5,     0,    57,    56,     1,    57,
       0,     6,     7,     8,     9,    10,    11,    12,    13,    14,
       4,     5,     2,    92,    93,     0,    94,     0,     0,     7,
       8,     9,    10,    11,     0,    13,    14,    36,    38,    41,
       0,     6,     0,     0,     0,     0,     0,    12,     0,    32,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,     0,    72,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    88,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,     0,    95,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,     0,    96,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
       0,    97,     0,    98,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55
};

static const yytype_int8 yycheck[] =
{
       0,    15,     3,    31,    15,    31,     4,    15,     8,     9,
       1,    28,     3,    30,     0,    32,    31,    34,    32,     4,
       5,    32,     4,    24,    32,    23,    27,    28,    28,    14,
      31,    29,    15,    24,    34,    17,    27,    28,    17,    18,
      31,    23,    27,    15,    16,    17,    18,    29,    15,    31,
      35,    21,    37,     3,     3,    40,    56,    57,    21,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,     3,     4,    58,     6,    24,    -1,    -1,    27,    28,
      -1,    -1,    31,    15,    16,    -1,    15,    15,     3,    15,
      -1,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      15,    16,     4,    32,    32,    -1,    32,    -1,    -1,    24,
      25,    26,    27,    28,    -1,    30,    31,    11,    12,    13,
      -1,    23,    -1,    -1,    -1,    -1,    -1,    29,    -1,    31,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    -1,    32,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    32,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    -1,    32,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    -1,    32,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      -1,    32,    -1,    22,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     6,    15,    16,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    35,    35,    31,    31,    17,
      31,    37,    31,    37,    31,    31,    36,    31,    36,     1,
      31,    36,    35,     0,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    15,    15,    21,    35,
      37,    38,     3,    37,    38,    38,    37,    38,    35,    35,
      21,    35,    32,    35,    35,    35,    35,    35,    35,    35,
      35,    35,    35,    35,    35,    37,    37,    35,    32,    32,
      32,    32,    32,    32,    32,    32,    32,    32,    22
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (stream, store, filelog, result, invert_terms, array_decl, retrievedVar, string, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, stream, store, retrievedVar)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, stream, store, filelog, result, invert_terms, array_decl, retrievedVar, string); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, std::wstringstream *stream, NONS_VariableStore *store, NONS_FileLog *filelog, long *result, bool invert_terms, std::vector<long> *array_decl, NONS_VariableMember **retrievedVar, std::wstring *string)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, stream, store, filelog, result, invert_terms, array_decl, retrievedVar, string)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    std::wstringstream *stream;
    NONS_VariableStore *store;
    NONS_FileLog *filelog;
    long *result;
    bool invert_terms;
    std::vector<long> *array_decl;
    NONS_VariableMember **retrievedVar;
    std::wstring *string;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (stream);
  YYUSE (store);
  YYUSE (filelog);
  YYUSE (result);
  YYUSE (invert_terms);
  YYUSE (array_decl);
  YYUSE (retrievedVar);
  YYUSE (string);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, std::wstringstream *stream, NONS_VariableStore *store, NONS_FileLog *filelog, long *result, bool invert_terms, std::vector<long> *array_decl, NONS_VariableMember **retrievedVar, std::wstring *string)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, stream, store, filelog, result, invert_terms, array_decl, retrievedVar, string)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    std::wstringstream *stream;
    NONS_VariableStore *store;
    NONS_FileLog *filelog;
    long *result;
    bool invert_terms;
    std::vector<long> *array_decl;
    NONS_VariableMember **retrievedVar;
    std::wstring *string;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, stream, store, filelog, result, invert_terms, array_decl, retrievedVar, string);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule, std::wstringstream *stream, NONS_VariableStore *store, NONS_FileLog *filelog, long *result, bool invert_terms, std::vector<long> *array_decl, NONS_VariableMember **retrievedVar, std::wstring *string)
#else
static void
yy_reduce_print (yyvsp, yyrule, stream, store, filelog, result, invert_terms, array_decl, retrievedVar, string)
    YYSTYPE *yyvsp;
    int yyrule;
    std::wstringstream *stream;
    NONS_VariableStore *store;
    NONS_FileLog *filelog;
    long *result;
    bool invert_terms;
    std::vector<long> *array_decl;
    NONS_VariableMember **retrievedVar;
    std::wstring *string;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       , stream, store, filelog, result, invert_terms, array_decl, retrievedVar, string);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule, stream, store, filelog, result, invert_terms, array_decl, retrievedVar, string); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, std::wstringstream *stream, NONS_VariableStore *store, NONS_FileLog *filelog, long *result, bool invert_terms, std::vector<long> *array_decl, NONS_VariableMember **retrievedVar, std::wstring *string)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, stream, store, filelog, result, invert_terms, array_decl, retrievedVar, string)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    std::wstringstream *stream;
    NONS_VariableStore *store;
    NONS_FileLog *filelog;
    long *result;
    bool invert_terms;
    std::vector<long> *array_decl;
    NONS_VariableMember **retrievedVar;
    std::wstring *string;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (stream);
  YYUSE (store);
  YYUSE (filelog);
  YYUSE (result);
  YYUSE (invert_terms);
  YYUSE (array_decl);
  YYUSE (retrievedVar);
  YYUSE (string);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {
      case 3: /* "INTEGER" */

	{
	freeVM((yyvaluep->obj));
};

	break;
      case 4: /* "STRING" */

	{
	freeVM((yyvaluep->obj));
};

	break;
      case 5: /* "INTEGER_ARRAY" */

	{
	freeVM((yyvaluep->obj));
};

	break;
      case 35: /* "expr" */

	{
	freeVM((yyvaluep->obj));
};

	break;
      case 36: /* "integer" */

	{
	freeVM((yyvaluep->obj));
};

	break;
      case 37: /* "string" */

	{
	freeVM((yyvaluep->obj));
};

	break;
      case 39: /* "integer_array" */

	{
	freeVM((yyvaluep->obj));
};

	break;
      case 40: /* "operation" */

	{
	freeVM((yyvaluep->obj));
};

	break;
      case 41: /* "integer_dereference" */

	{
	freeVM((yyvaluep->obj));
};

	break;
      case 42: /* "string_dereference" */

	{
	freeVM((yyvaluep->obj));
};

	break;
      case 43: /* "boolean_operation" */

	{
	freeVM((yyvaluep->obj));
};

	break;

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (std::wstringstream *stream, NONS_VariableStore *store, NONS_FileLog *filelog, long *result, bool invert_terms, std::vector<long> *array_decl, NONS_VariableMember **retrievedVar, std::wstring *string);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */





/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (std::wstringstream *stream, NONS_VariableStore *store, NONS_FileLog *filelog, long *result, bool invert_terms, std::vector<long> *array_decl, NONS_VariableMember **retrievedVar, std::wstring *string)
#else
int
yyparse (stream, store, filelog, result, invert_terms, array_decl, retrievedVar, string)
    std::wstringstream *stream;
    NONS_VariableStore *store;
    NONS_FileLog *filelog;
    long *result;
    bool invert_terms;
    std::vector<long> *array_decl;
    NONS_VariableMember **retrievedVar;
    std::wstring *string;
#endif
#endif
{
/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

    {
		(yyvsp[(1) - (1)].obj)->negate(invert_terms);
		if (!!result)
			*result=(yyvsp[(1) - (1)].obj)->getInt();
		if (!!retrievedVar && !(yyvsp[(1) - (1)].obj)->temporary)
			*retrievedVar=(yyvsp[(1) - (1)].obj);
		if (!!string){
			*string=(yyvsp[(1) - (1)].obj)->getWcs();
		}
		freeVM((yyvsp[(1) - (1)].obj));
	}
    break;

  case 3:

    {
		if (!!string){
			*string=(yyvsp[(1) - (1)].obj)->getWcs();
			freeVM((yyvsp[(1) - (1)].obj));
		}else if (!!retrievedVar && !(yyvsp[(1) - (1)].obj)->temporary)
			*retrievedVar=(yyvsp[(1) - (1)].obj);
		else{
			freeVM((yyvsp[(1) - (1)].obj));
			YYABORT;
		}
	}
    break;

  case 4:

    {
		if (!!string){
			*string=(yyvsp[(1) - (1)].obj)->getWcs();
			freeVM((yyvsp[(1) - (1)].obj));
		}else if (!!retrievedVar && !(yyvsp[(1) - (1)].obj)->temporary)
			*retrievedVar=(yyvsp[(1) - (1)].obj);
		else{
			freeVM((yyvsp[(1) - (1)].obj));
			YYABORT;
		}
	}
    break;

  case 5:

    {
		YYABORT;
	}
    break;

  case 6:

    {
		(yyval.obj)=(yyvsp[(1) - (1)].obj);
	}
    break;

  case 7:

    {
		if (!(yyvsp[(1) - (1)].obj)){
			if (!array_decl){
				handleErrors(NONS_UNDEFINED_ARRAY,0,"yyparse",1);
				YYABORT;
			}
			(yyval.obj)=new NONS_VariableMember((long)0);
			(yyval.obj)->temporary=1;
		}else{
			if ((yyvsp[(1) - (1)].obj)->getType()==INTEGER_ARRAY && !retrievedVar){
				handleErrors(NONS_INSUFFICIENT_DIMENSIONS,0,"yyparse",1);
				YYABORT;
			}else
				(yyval.obj)=(yyvsp[(1) - (1)].obj);
		}
	}
    break;

  case 8:

    {
		(yyval.obj)=(yyvsp[(1) - (1)].obj);
	}
    break;

  case 9:

    {
		(yyval.obj)=(yyvsp[(1) - (1)].obj);
	}
    break;

  case 10:

    {
		long temp;
		ErrorCode error=store->evaluate((yyvsp[(3) - (4)].obj)->getWcs(),&temp,0,0,0,0);
		freeVM((yyvsp[(3) - (4)].obj));
		if (!CHECK_FLAG(error,NONS_NO_ERROR_FLAG)){
			handleErrors(error,0,"yyparse",1);
			YYABORT;
		}
		(yyval.obj)=new NONS_VariableMember(temp);
		(yyval.obj)->temporary=1;
	}
    break;

  case 11:

    {
		long temp;
		ErrorCode error=store->evaluate((yyvsp[(3) - (4)].obj)->getWcs(),&temp,0,0,0,0);
		freeVM((yyvsp[(3) - (4)].obj));
		if (!CHECK_FLAG(error,NONS_NO_ERROR_FLAG)){
			handleErrors(error,0,"yyparse",1);
			YYABORT;
		}
		(yyval.obj)=new NONS_VariableMember(temp);
		(yyval.obj)->temporary=1;
	}
    break;

  case 12:

    {
		long temp;
		std::wstringstream stream;
		stream <<(yyvsp[(3) - (4)].obj)->getWcs();
		freeVM((yyvsp[(3) - (4)].obj));
		if (!(stream >>temp)){
			handleErrors(NONS_LEXICALLY_UNCASTABLE,0,"yyparse",1);
			YYABORT;
		}
		(yyval.obj)=new NONS_VariableMember(temp);
		(yyval.obj)->temporary=1;
	}
    break;

  case 13:

    {
		long temp;
		std::wstringstream stream;
		stream <<(yyvsp[(3) - (4)].obj)->getWcs();
		freeVM((yyvsp[(3) - (4)].obj));
		if (!(stream >>temp)){
			handleErrors(NONS_LEXICALLY_UNCASTABLE,0,"yyparse",1);
			YYABORT;
		}
		(yyval.obj)=new NONS_VariableMember(temp);
		(yyval.obj)->temporary=1;
	}
    break;

  case 14:

    {
		(yyval.obj)=(yyvsp[(1) - (1)].obj);
		if (!(yyval.obj))
			YYABORT;
	}
    break;

  case 15:

    {
		(yyval.obj)=(yyvsp[(1) - (1)].obj);
	}
    break;

  case 16:

    {
		(yyval.obj)=(yyvsp[(1) - (1)].obj);
	}
    break;

  case 17:

    {
		std::wstringstream stream;
		stream <<(yyvsp[(3) - (4)].obj)->getInt();
		freeVM((yyvsp[(3) - (4)].obj));
		(yyval.obj)=new NONS_VariableMember(stream.str());
		(yyval.obj)->temporary=1;
	}
    break;

  case 18:

    {
		(yyval.obj)=new NONS_VariableMember((yyvsp[(1) - (3)].obj)->getWcs()+(yyvsp[(3) - (3)].obj)->getWcs());
		freeVM((yyvsp[(1) - (3)].obj));
		freeVM((yyvsp[(3) - (3)].obj));
	}
    break;

  case 19:

    {
		(yyval.obj)=new NONS_VariableMember((yyvsp[(1) - (3)].obj)->getWcs()+(yyvsp[(3) - (3)].obj)->getWcs());
		freeVM((yyvsp[(1) - (3)].obj));
		freeVM((yyvsp[(3) - (3)].obj));
	}
    break;

  case 20:

    {
		long a=(yyvsp[(3) - (4)].obj)->getInt();
		if (!!(yyvsp[(1) - (4)].obj) && (yyvsp[(1) - (4)].obj)->getType()==INTEGER_ARRAY){
			if (!((yyval.obj)=(yyvsp[(1) - (4)].obj)->getIndex(a))){
				std::wstringstream stream;
				stream <<L"The index is: "<<a;
				handleErrors(NONS_ARRAY_INDEX_OUT_OF_BOUNDS,0,"yyparse",1,stream.str());
				(yyval.obj)=(yyvsp[(1) - (4)].obj)->getIndex(0);
			}
		}else{
			if (!array_decl){
				handleErrors(NONS_TOO_MANY_DIMENSIONS,0,"yyparse",1);
			}else
				array_decl->push_back(a);
			(yyval.obj)=0;
		}
		freeVM((yyvsp[(3) - (4)].obj));
	}
    break;

  case 21:

    {
		long a=(yyvsp[(2) - (2)].obj)->getInt();
		freeVM((yyvsp[(2) - (2)].obj));
		(yyval.obj)=store->getArray(a);
		if (!!array_decl && !(yyval.obj)){
			if (!array_decl->size())
				array_decl->push_back(1);
			else
				(*array_decl)[0]++;
			array_decl->push_back(a);
		}
	}
    break;

  case 22:

    {
		long a=(yyvsp[(3) - (4)].obj)->getInt();
		freeVM((yyvsp[(3) - (4)].obj));
		(yyval.obj)=store->getArray(a);
		if (!!array_decl && !(yyval.obj)){
			if (!array_decl->size())
				array_decl->push_back(1);
			else
				(*array_decl)[0]++;
			array_decl->push_back(a);
		}
	}
    break;

  case 23:

    {
		handleErrors(NONS_ILLEGAL_ARRAY_SPECIFICATION,0,"yyparse",1);
		if (!!array_decl)
			array_decl->clear();
		(yyval.obj)=0;
	}
    break;

  case 24:

    {
		(yyval.obj)=new NONS_VariableMember((yyvsp[(1) - (3)].obj)->getInt()+(yyvsp[(3) - (3)].obj)->getInt());
		(yyval.obj)->temporary=1;
		freeVM((yyvsp[(1) - (3)].obj));
		freeVM((yyvsp[(3) - (3)].obj));
	}
    break;

  case 25:

    {
		(yyval.obj)=new NONS_VariableMember((yyvsp[(1) - (3)].obj)->getInt()-(yyvsp[(3) - (3)].obj)->getInt());
		(yyval.obj)->temporary=1;
		freeVM((yyvsp[(1) - (3)].obj));
		freeVM((yyvsp[(3) - (3)].obj));
	}
    break;

  case 26:

    {
		(yyval.obj)=new NONS_VariableMember((yyvsp[(1) - (3)].obj)->getInt()*(yyvsp[(3) - (3)].obj)->getInt());
		(yyval.obj)->temporary=1;
		freeVM((yyvsp[(1) - (3)].obj));
		freeVM((yyvsp[(3) - (3)].obj));
	}
    break;

  case 27:

    {
		long a=(yyvsp[(1) - (3)].obj)->getInt(),
			b=(yyvsp[(3) - (3)].obj)->getInt();
		if (!b){
			handleErrors(NONS_DIVISION_BY_ZERO,0,"yyparse",1);
			YYABORT;
		}
		(yyval.obj)=new NONS_VariableMember(a+b);
		(yyval.obj)->temporary=1;
		freeVM((yyvsp[(1) - (3)].obj));
		freeVM((yyvsp[(3) - (3)].obj));
	}
    break;

  case 28:

    {
		if (!(yyvsp[(1) - (1)].obj)->temporary){
			(yyval.obj)=new NONS_VariableMember((yyvsp[(1) - (1)].obj)->getInt());
			(yyval.obj)->temporary=1;
			(yyval.obj)->negated=0;
		}else
			(yyval.obj)=(yyvsp[(1) - (1)].obj);
		(yyval.obj)->negate(invert_terms);
	}
    break;

  case 29:

    {
		if (!(yyvsp[(1) - (3)].obj)->temporary){
			(yyvsp[(1) - (3)].obj)=new NONS_VariableMember((yyvsp[(1) - (3)].obj)->getInt());
			(yyvsp[(1) - (3)].obj)->temporary=1;
		}
		if (!(yyvsp[(3) - (3)].obj)->temporary){
			(yyvsp[(3) - (3)].obj)=new NONS_VariableMember((yyvsp[(3) - (3)].obj)->getInt());
			(yyvsp[(3) - (3)].obj)->temporary=1;
		}
		(yyvsp[(1) - (3)].obj)->negate(invert_terms);
		(yyvsp[(3) - (3)].obj)->negate(invert_terms);
		(yyval.obj)=new NONS_VariableMember((yyvsp[(1) - (3)].obj)->getInt() || (yyvsp[(3) - (3)].obj)->getInt());
		(yyval.obj)->temporary=1;
		freeVM((yyvsp[(1) - (3)].obj));
		freeVM((yyvsp[(3) - (3)].obj));
	}
    break;

  case 30:

    {
		if (!(yyvsp[(1) - (3)].obj)->temporary){
			(yyvsp[(1) - (3)].obj)=new NONS_VariableMember((yyvsp[(1) - (3)].obj)->getInt());
			(yyvsp[(1) - (3)].obj)->temporary=1;
		}
		if (!(yyvsp[(3) - (3)].obj)->temporary){
			(yyvsp[(3) - (3)].obj)=new NONS_VariableMember((yyvsp[(3) - (3)].obj)->getInt());
			(yyvsp[(3) - (3)].obj)->temporary=1;
		}
		(yyvsp[(1) - (3)].obj)->negate(invert_terms);
		(yyvsp[(3) - (3)].obj)->negate(invert_terms);
		(yyval.obj)=new NONS_VariableMember((yyvsp[(1) - (3)].obj)->getInt() && (yyvsp[(3) - (3)].obj)->getInt());
		(yyval.obj)->temporary=1;
		freeVM((yyvsp[(1) - (3)].obj));
		freeVM((yyvsp[(3) - (3)].obj));
	}
    break;

  case 31:

    {
		(yyval.obj)=new NONS_VariableMember((yyvsp[(2) - (2)].obj)->getInt());
		(yyval.obj)->temporary=1;
		freeVM((yyvsp[(2) - (2)].obj));
	}
    break;

  case 32:

    {
		(yyval.obj)=new NONS_VariableMember(-((yyvsp[(2) - (2)].obj)->getInt()));
		(yyval.obj)->temporary=1;
		freeVM((yyvsp[(2) - (2)].obj));
	}
    break;

  case 33:

    {
		(yyval.obj)=(yyvsp[(2) - (3)].obj);
	}
    break;

  case 34:

    {
		NONS_Variable *v=store->retrieve((yyvsp[(2) - (2)].obj)->getInt(),0);
		freeVM((yyvsp[(2) - (2)].obj));
		if (!v){
			handleErrors(NONS_VARIABLE_OUT_OF_RANGE,0,"yyparse",1);
			(yyval.obj)=0;
		}else
			(yyval.obj)=v->intValue;
	}
    break;

  case 35:

    {
		NONS_Variable *v=store->retrieve((yyvsp[(3) - (4)].obj)->getInt(),0);
		freeVM((yyvsp[(3) - (4)].obj));
		if (!v){
			handleErrors(NONS_VARIABLE_OUT_OF_RANGE,0,"yyparse",1);
			(yyval.obj)=0;
		}else
			(yyval.obj)=v->intValue;
	}
    break;

  case 36:

    {
		NONS_Variable *v=store->retrieve((yyvsp[(2) - (2)].obj)->getInt(),0);
		freeVM((yyvsp[(2) - (2)].obj));
		if (!v){
			handleErrors(NONS_VARIABLE_OUT_OF_RANGE,0,"yyparse",1);
			(yyval.obj)=0;
		}else
			(yyval.obj)=v->wcsValue;
	}
    break;

  case 37:

    {
		NONS_Variable *v=store->retrieve((yyvsp[(3) - (4)].obj)->getInt(),0);
		freeVM((yyvsp[(3) - (4)].obj));
		if (!v){
			handleErrors(NONS_VARIABLE_OUT_OF_RANGE,0,"yyparse",1);
			(yyval.obj)=0;
		}else
			(yyval.obj)=v->wcsValue;
	}
    break;

  case 38:

    {
		(yyval.obj)=new NONS_VariableMember((yyvsp[(1) - (3)].obj)->getInt()==(yyvsp[(3) - (3)].obj)->getInt());
		(yyval.obj)->temporary=1;
		(yyval.obj)->negated=0;
		freeVM((yyvsp[(1) - (3)].obj));
		freeVM((yyvsp[(3) - (3)].obj));
	}
    break;

  case 39:

    {
		(yyval.obj)=new NONS_VariableMember((yyvsp[(1) - (3)].obj)->getInt()!=(yyvsp[(3) - (3)].obj)->getInt());
		(yyval.obj)->temporary=1;
		(yyval.obj)->negated=0;
		freeVM((yyvsp[(1) - (3)].obj));
		freeVM((yyvsp[(3) - (3)].obj));
	}
    break;

  case 40:

    {
		(yyval.obj)=new NONS_VariableMember((yyvsp[(1) - (3)].obj)->getInt()<(yyvsp[(3) - (3)].obj)->getInt());
		(yyval.obj)->temporary=1;
		(yyval.obj)->negated=0;
		freeVM((yyvsp[(1) - (3)].obj));
		freeVM((yyvsp[(3) - (3)].obj));
	}
    break;

  case 41:

    {
		(yyval.obj)=new NONS_VariableMember((yyvsp[(1) - (3)].obj)->getInt()>=(yyvsp[(3) - (3)].obj)->getInt());
		(yyval.obj)->temporary=1;
		(yyval.obj)->negated=0;
		freeVM((yyvsp[(1) - (3)].obj));
		freeVM((yyvsp[(3) - (3)].obj));
	}
    break;

  case 42:

    {
		(yyval.obj)=new NONS_VariableMember((yyvsp[(1) - (3)].obj)->getInt()>(yyvsp[(3) - (3)].obj)->getInt());
		(yyval.obj)->temporary=1;
		(yyval.obj)->negated=0;
		freeVM((yyvsp[(1) - (3)].obj));
		freeVM((yyvsp[(3) - (3)].obj));
	}
    break;

  case 43:

    {
		(yyval.obj)=new NONS_VariableMember((yyvsp[(1) - (3)].obj)->getInt()<=(yyvsp[(3) - (3)].obj)->getInt());
		(yyval.obj)->temporary=1;
		(yyval.obj)->negated=0;
		freeVM((yyvsp[(1) - (3)].obj));
		freeVM((yyvsp[(3) - (3)].obj));
	}
    break;

  case 44:

    {
		(yyval.obj)=new NONS_VariableMember(filelog->check((yyvsp[(2) - (2)].obj)->getWcs()));
		(yyval.obj)->temporary=1;
		(yyval.obj)->negated=0;
		freeVM((yyvsp[(2) - (2)].obj));
	}
    break;

  case 45:

    {
		(yyval.obj)=new NONS_VariableMember(filelog->check((yyvsp[(3) - (4)].obj)->getWcs()));
		(yyval.obj)->temporary=1;
		(yyval.obj)->negated=0;
		freeVM((yyvsp[(3) - (4)].obj));
	}
    break;

  case 46:

    {
		(yyval.obj)=new NONS_VariableMember(labellog.check((yyvsp[(2) - (2)].obj)->getWcs()));
		(yyval.obj)->temporary=1;
		freeVM((yyvsp[(2) - (2)].obj));
	}
    break;

  case 47:

    {
		std::wstringstream stream;
		stream <<(yyvsp[(3) - (3)].obj)->getInt();
		(yyval.obj)=new NONS_VariableMember(labellog.check(stream.str()));
		(yyval.obj)->temporary=1;
		freeVM((yyvsp[(3) - (3)].obj));
	}
    break;

  case 48:

    {
		(yyval.obj)=new NONS_VariableMember(labellog.check((yyvsp[(3) - (4)].obj)->getWcs()));
		(yyval.obj)->temporary=1;
		freeVM((yyvsp[(3) - (4)].obj));
	}
    break;



      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (stream, store, filelog, result, invert_terms, array_decl, retrievedVar, string, YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (stream, store, filelog, result, invert_terms, array_decl, retrievedVar, string, yymsg);
	  }
	else
	  {
	    yyerror (stream, store, filelog, result, invert_terms, array_decl, retrievedVar, string, YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, stream, store, filelog, result, invert_terms, array_decl, retrievedVar, string);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, stream, store, filelog, result, invert_terms, array_decl, retrievedVar, string);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (stream, store, filelog, result, invert_terms, array_decl, retrievedVar, string, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, stream, store, filelog, result, invert_terms, array_decl, retrievedVar, string);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, stream, store, filelog, result, invert_terms, array_decl, retrievedVar, string);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}





void freeVM(NONS_VariableMember *p){
	if (!!p && p->temporary)
		delete p;
}

#define DOUBLEOP(character,ret_value) if (c==(character)){\
	stream->get();\
	if (stream->peek()==(character)){\
		stream->get();\
		return (ret_value);\
	}\
	return (ret_value);\
}

int expressionParser_yylex(YYSTYPE *yylval,std::wstringstream *stream,NONS_VariableStore *store,NONS_VariableMember **retrievedVar){
	int c;
	while (!stream->eof() && iswhitespace((wchar_t)stream->peek()))
		stream->get();
	if (stream->eof())
		return 0;
	c=stream->peek();
	if (NONS_isdigit(c)){
		std::wstring temp;
		while (NONS_isdigit(c=stream->peek()))
			temp.push_back(stream->get());
		//Handles the case "* 134label". This is the kind of thing that gives me
		//omnicidal rages. Note: under no other circumstance makes sense that an
		//integer be followed immediately by an alphabetic character or an
		//underscore.
		if (NONS_isid1char(c)){
			while (NONS_isidnchar(stream->peek()))
				temp.push_back(stream->get());
			yylval->obj=new NONS_VariableMember(temp);
			yylval->obj->temporary=1;
			return STRING;
		}
		yylval->obj=new NONS_VariableMember(atoi(temp));
		yylval->obj->temporary=1;
		return INTEGER;
	}
	if (c=='#'){
		stream->get();
		std::wstring temp;
		while (NONS_ishexa(stream->peek()))
			temp.push_back(stream->get());
		if (temp.size()<6)
			return ERROR;
		long a=0;
		for (size_t b=0;b<temp.size();b++)
			a=(a<<4)+HEX2DEC(temp[b]);
		yylval->obj=new NONS_VariableMember(a);
		yylval->obj->temporary=1;
		return INTEGER;
	}
	if (c=='\"' || c=='`' || NONS_tolower(c)=='e'){
		c=stream->get();
		bool cont=0,
			useEscapes=0;
		if (NONS_tolower(c)=='e'){
			if (stream->peek()!='\"'){
				stream->putback(c);
				cont=1;
			}else{
				c=stream->get();
				useEscapes=1;
			}
		}
		if (!cont){
			std::wstring temp;
			while ((wchar_t)stream->peek()!=c && !stream->eof()){
				wchar_t character=stream->get();
				if (character=='\\' && useEscapes){
					character=stream->get();
					switch (character){
						case '\\':
						case '\"':
							temp.push_back(character);
							break;
						case 'n':
						case 'r':
							temp.push_back('\n');
							break;
						case 'x':
							{
								std::wstring temp2;
								for (ulong a=0;NONS_ishexa(stream->peek()) && a<4;a++)
									temp2.push_back(stream->get());
								if (temp2.size()<4)
									return ERROR;
								wchar_t a=0;
								for (size_t b=0;b<temp2.size();b++)
									a=(a<<4)+HEX2DEC(temp2[b]);
								temp.push_back(a?a:32);
							}
							break;
						default:
							return ERROR;
					}
				}else
					temp.push_back(character);
			}
			if ((wchar_t)stream->peek()!=c)
				handleErrors(NONS_UNMATCHED_QUOTES,0,"yylex",1);
			else
				stream->get();
			yylval->obj=new NONS_VariableMember(temp);
			yylval->obj->temporary=1;
			return STRING;
		}
	}
	if (c=='*'){
		std::wstring backup;
		backup.push_back(stream->get());
		while (iswhitespace((wchar_t)stream->peek()))
			stream->get();
		std::wstring identifier;
		c=stream->peek();
		if (NONS_isid1char(c)){
			identifier.push_back(c);
			backup.push_back(stream->get());
			while (NONS_isidnchar(c=stream->peek())){
				identifier.push_back(c);
				backup.push_back(stream->get());
			}
			if (!!gScriptInterpreter->script->blockFromLabel(identifier)){
				yylval->obj=new NONS_VariableMember(identifier);
				yylval->obj->temporary=1;
				return STRING;
			}
		}
		for (std::wstring::reverse_iterator i=backup.rbegin();i!=backup.rend();i++)
			stream->putback(*i);
	}
	if (NONS_isid1char(c)){
		std::wstring temp;
		temp.push_back(stream->get());
		while (NONS_isidnchar(stream->peek()))
			temp.push_back(stream->get());
		if (!stdStrCmpCI(temp,L"fchk"))
			return FCHK;
		if (!stdStrCmpCI(temp,L"lchk"))
			return LCHK;
		if (!stdStrCmpCI(temp,L"_eval"))
			return EVAL;
		if (!stdStrCmpCI(temp,L"_itoa"))
			return ITOA;
		if (!stdStrCmpCI(temp,L"_atoi"))
			return ATOI;
		
		if (!(yylval->obj=store->getConstant(temp.c_str()))){
			yylval->obj=new NONS_VariableMember((long)0);
			yylval->obj->temporary=1;
			if (!retrievedVar)
				handleErrors(NONS_UNDEFINED_CONSTANT,0,"yylex",1);
		}else{
			yylval->obj=new NONS_VariableMember(*yylval->obj);
			yylval->obj->temporary=1;
		}
		return yylval->obj->getType();
	}
	DOUBLEOP('=',EQUALS)
	DOUBLEOP('&',AND)
	DOUBLEOP('|',OR)
	if (c=='!'){
		stream->get();
		if (stream->peek()=='='){
			stream->get();
			return NEQ;
		}
		return c;
	}
	if (c=='<'){
		stream->get();
		if (stream->peek()=='='){
			stream->get();
			return LOWEREQ;
		}
		if (stream->peek()=='>'){
			stream->get();
			return NEQ;
		}
		return LOWER;
	}
	if (c=='>'){
		stream->get();
		if (stream->peek()=='='){
			stream->get();
			return GREATEREQ;
		}
		return GREATER;
	}
	if (!multicomparison((wchar_t)c,"+-*/[]%$?()")){
		wchar_t temp[]={c,0};
		handleErrors(NONS_UNRECOGNIZED_OPERATOR,0,"yylex",1,temp);
	}
	return stream->get();
}

void expressionParser_yyerror(
		std::wstringstream *,
		NONS_VariableStore *,
		NONS_FileLog *,
		long *,
		bool,
		std::vector<long> *,
		NONS_VariableMember **retrievedVar,
		std::wstring *string,
		char const *s){
	if (!retrievedVar)
		handleErrors(NONS_UNDEFINED_ERROR,0,"yyparse",1,UniFromISO88591(s));
}

