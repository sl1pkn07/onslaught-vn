
/* A Bison parser, made by GNU Bison 2.4.1.  */

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
#define YYBISON_VERSION "2.4.1"

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



/* Copy the first part of user declarations.  */


#include "VariableStore.h"
#include "ScriptInterpreter.h"
#include "../UTF.h"
#include "../Globals.h"
#include "../Functions.h"
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
	#include "../IO_System/SAR/Image_Loader/FileLog.h"
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
	int yyparse(
		std::wstringstream *stream,
		NONS_VariableStore *store,
		NONS_FileLog *filelog,
		long *result,
		bool invert_terms,
		std::vector<long> *array_decl,
		NONS_VariableMember **retrievedVar,
		wchar_t **string
	);
	int yylex(YYSTYPE *yylval,
		std::wstringstream *stream,
		NONS_VariableStore *store,
		NONS_VariableMember **retrievedVar
	);
	void yyerror(
		std::wstringstream *,
		NONS_VariableStore *,
		NONS_FileLog *,
		long *,
		bool,
		std::vector<long> *,
		NONS_VariableMember **,
		wchar_t **string,
		char const *
	);
	long atol(std::wstring &str);




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
#define YYFINAL  39
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   224

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  34
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  10
/* YYNRULES -- Number of rules.  */
#define YYNRULES  41
/* YYNRULES -- Number of states.  */
#define YYNSTATES  77

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
       2,     2,     2,    19,     2,     2,    25,    24,     2,     2,
      32,    33,    17,    15,     2,    16,     2,    18,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    26,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    22,     2,    23,     2,     2,     2,     2,     2,     2,
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
      20,    21,    27,    28,    29,    30,    31
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    11,    13,    15,    17,
      20,    23,    25,    27,    29,    33,    38,    41,    46,    50,
      54,    58,    62,    66,    68,    72,    76,    79,    82,    86,
      89,    94,    97,   102,   106,   110,   114,   118,   122,   126,
     129,   132
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      35,     0,    -1,    36,    -1,    38,    -1,     6,    -1,    37,
      -1,    39,    -1,    40,    -1,     3,    -1,    31,    38,    -1,
      28,    38,    -1,    41,    -1,     4,    -1,    42,    -1,    38,
      15,    38,    -1,    39,    22,    36,    23,    -1,    26,    37,
      -1,    26,    32,    36,    33,    -1,    26,     1,    22,    -1,
      36,    15,    36,    -1,    36,    16,    36,    -1,    36,    17,
      36,    -1,    36,    18,    36,    -1,    43,    -1,    36,     7,
      36,    -1,    36,     8,    36,    -1,    15,    36,    -1,    16,
      36,    -1,    32,    36,    33,    -1,    24,    37,    -1,    24,
      32,    36,    33,    -1,    25,    37,    -1,    25,    32,    36,
      33,    -1,    36,    14,    36,    -1,    36,    13,    36,    -1,
      36,    12,    36,    -1,    36,     9,    36,    -1,    36,    10,
      36,    -1,    36,    11,    36,    -1,    19,    36,    -1,    30,
      38,    -1,    29,    38,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   114,   114,   127,   140,   145,   148,   164,   169,   172,
     183,   195,   202,   205,   208,   215,   233,   245,   257,   265,
     271,   277,   283,   295,   304,   320,   336,   341,   346,   351,
     360,   371,   380,   391,   398,   405,   412,   419,   426,   433,
     439,   445
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "INTEGER", "STRING", "INTEGER_ARRAY",
  "ERROR", "OR", "AND", "GREATEREQ", "GREATER", "LOWEREQ", "LOWER", "NEQ",
  "EQUALS", "'+'", "'-'", "'*'", "'/'", "'!'", "POS", "NEG", "'['", "']'",
  "'%'", "'$'", "'?'", "ITOA", "ATOI", "LCHK", "FCHK", "EVAL", "'('",
  "')'", "$accept", "eval", "expr", "integer", "string", "integer_array",
  "operation", "integer_dereference", "string_dereference",
  "boolean_operation", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,    43,    45,    42,    47,    33,
     270,   271,    91,    93,    37,    36,    63,   272,   273,   274,
     275,   276,    40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    34,    35,    35,    35,    36,    36,    36,    37,    37,
      37,    37,    38,    38,    38,    39,    39,    39,    39,    40,
      40,    40,    40,    40,    40,    40,    40,    40,    40,    41,
      41,    42,    42,    43,    43,    43,    43,    43,    43,    43,
      43,    43
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     1,     1,     2,
       2,     1,     1,     1,     3,     4,     2,     4,     3,     3,
       3,     3,     3,     1,     3,     3,     2,     2,     3,     2,
       4,     2,     4,     3,     3,     3,     3,     3,     3,     2,
       2,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     8,    12,     4,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     2,     5,     3,     6,
       7,    11,    13,    23,    26,    27,    39,     0,    29,     0,
      31,     0,     0,    16,    10,    41,    40,     9,     0,     1,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    18,     0,    28,    24,
      25,    36,    37,    38,    35,    34,    33,    19,    20,    21,
      22,    14,     0,    30,    32,    17,    15
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,    15,    16,    17,    18,    19,    20,    21,    22,    23
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -19
static const yytype_int16 yypact[] =
{
      48,   -19,   -19,   -19,    66,    66,    66,    80,    85,     2,
       4,     4,     4,     4,    66,     9,   206,   -19,     7,   -18,
     -19,   -19,   -19,   -19,   -19,   -19,   -19,    66,   -19,    66,
     -19,     5,    66,   -19,   -19,   -19,   -19,   -19,   111,   -19,
      66,    66,    66,    66,    66,    66,    66,    66,    66,    66,
      66,    66,     4,    66,   123,   150,   -19,   162,   -19,   137,
     175,     0,     0,     0,     0,     0,     0,   -11,   -11,   -19,
     -19,   -19,   189,   -19,   -19,   -19,   -19
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -19,   -19,    -4,    12,     1,   -19,   -19,   -19,   -19,   -19
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      24,    25,    26,    31,    53,     1,    50,    51,     2,    39,
      38,    34,    35,    36,    37,    48,    49,    50,    51,    28,
      30,    33,    52,    54,     0,    55,     7,    56,    57,     8,
      10,     0,     0,    13,    32,     0,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,     0,    72,
       0,     1,     2,    71,     3,     0,     0,     0,     0,     0,
       0,     0,     0,     4,     5,     0,     0,     6,     0,     1,
       0,     0,     7,     8,     9,     0,    10,    11,    12,    13,
      14,     4,     5,     1,     0,     6,     0,     0,     1,     0,
       7,     0,     9,     0,    10,    11,    12,    13,    14,     0,
       0,     0,     0,     0,     7,     0,     0,     0,    10,     7,
       0,    13,    27,    10,     0,     0,    13,    29,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,     0,     0,    58,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    73,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,     0,     0,    74,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,     0,    75,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,     0,     0,
       0,     0,    76,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51
};

static const yytype_int8 yycheck[] =
{
       4,     5,     6,     1,    22,     3,    17,    18,     4,     0,
      14,    10,    11,    12,    13,    15,    16,    17,    18,     7,
       8,     9,    15,    27,    -1,    29,    24,    22,    32,    25,
      28,    -1,    -1,    31,    32,    -1,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    -1,    53,
      -1,     3,     4,    52,     6,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    15,    16,    -1,    -1,    19,    -1,     3,
      -1,    -1,    24,    25,    26,    -1,    28,    29,    30,    31,
      32,    15,    16,     3,    -1,    19,    -1,    -1,     3,    -1,
      24,    -1,    26,    -1,    28,    29,    30,    31,    32,    -1,
      -1,    -1,    -1,    -1,    24,    -1,    -1,    -1,    28,    24,
      -1,    31,    32,    28,    -1,    -1,    31,    32,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    -1,    -1,    33,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    33,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    -1,    -1,    33,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    -1,    33,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    -1,    -1,
      -1,    -1,    23,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     6,    15,    16,    19,    24,    25,    26,
      28,    29,    30,    31,    32,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    36,    36,    36,    32,    37,    32,
      37,     1,    32,    37,    38,    38,    38,    38,    36,     0,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    15,    22,    36,    36,    22,    36,    33,    36,
      36,    36,    36,    36,    36,    36,    36,    36,    36,    36,
      36,    38,    36,    33,    33,    33,    23
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
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, std::wstringstream *stream, NONS_VariableStore *store, NONS_FileLog *filelog, long *result, bool invert_terms, std::vector<long> *array_decl, NONS_VariableMember **retrievedVar, wchar_t **string)
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
    wchar_t **string;
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, std::wstringstream *stream, NONS_VariableStore *store, NONS_FileLog *filelog, long *result, bool invert_terms, std::vector<long> *array_decl, NONS_VariableMember **retrievedVar, wchar_t **string)
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
    wchar_t **string;
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
yy_reduce_print (YYSTYPE *yyvsp, int yyrule, std::wstringstream *stream, NONS_VariableStore *store, NONS_FileLog *filelog, long *result, bool invert_terms, std::vector<long> *array_decl, NONS_VariableMember **retrievedVar, wchar_t **string)
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
    wchar_t **string;
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, std::wstringstream *stream, NONS_VariableStore *store, NONS_FileLog *filelog, long *result, bool invert_terms, std::vector<long> *array_decl, NONS_VariableMember **retrievedVar, wchar_t **string)
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
    wchar_t **string;
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
      case 36: /* "expr" */

	{
	freeVM((yyvaluep->obj));
};

	break;
      case 37: /* "integer" */

	{
	freeVM((yyvaluep->obj));
};

	break;
      case 38: /* "string" */

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
int yyparse (std::wstringstream *stream, NONS_VariableStore *store, NONS_FileLog *filelog, long *result, bool invert_terms, std::vector<long> *array_decl, NONS_VariableMember **retrievedVar, wchar_t **string);
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
yyparse (std::wstringstream *stream, NONS_VariableStore *store, NONS_FileLog *filelog, long *result, bool invert_terms, std::vector<long> *array_decl, NONS_VariableMember **retrievedVar, wchar_t **string)
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
    wchar_t **string;
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
			if (!!*string)
				delete[] *string;
			*string=(yyvsp[(1) - (1)].obj)->getWcsCopy();
		}
		freeVM((yyvsp[(1) - (1)].obj));
	;}
    break;

  case 3:

    {
		if (!!string){
			if (!!*string)
				delete[] *string;
			*string=(yyvsp[(1) - (1)].obj)->getWcsCopy();
			freeVM((yyvsp[(1) - (1)].obj));
		}else if (!!retrievedVar && !(yyvsp[(1) - (1)].obj)->temporary)
			*retrievedVar=(yyvsp[(1) - (1)].obj);
		else{
			freeVM((yyvsp[(1) - (1)].obj));
			YYABORT;
		}
	;}
    break;

  case 4:

    {
		YYABORT;
	;}
    break;

  case 5:

    {
		(yyval.obj)=(yyvsp[(1) - (1)].obj);
	;}
    break;

  case 6:

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
	;}
    break;

  case 7:

    {
		(yyval.obj)=(yyvsp[(1) - (1)].obj);
	;}
    break;

  case 8:

    {
		(yyval.obj)=(yyvsp[(1) - (1)].obj);
	;}
    break;

  case 9:

    {
		long temp;
		ErrorCode error=store->evaluate((yyvsp[(2) - (2)].obj)->getWcs(),&temp,0,0,0,0);
		freeVM((yyvsp[(2) - (2)].obj));
		if (!CHECK_FLAG(error,NONS_NO_ERROR_FLAG)){
			handleErrors(error,0,"yyparse",1);
			YYABORT;
		}
		(yyval.obj)=new NONS_VariableMember(temp);
		(yyval.obj)->temporary=1;
	;}
    break;

  case 10:

    {
		long temp;
		std::wstringstream stream;
		stream <<(yyvsp[(2) - (2)].obj)->getWcs();
		freeVM((yyvsp[(2) - (2)].obj));
		if (!(stream >>temp)){
			handleErrors(NONS_LEXICALLY_UNCASTABLE,0,"yyparse",1);
			YYABORT;
		}
		(yyval.obj)=new NONS_VariableMember(temp);
		(yyval.obj)->temporary=1;
	;}
    break;

  case 11:

    {
		(yyval.obj)=(yyvsp[(1) - (1)].obj);
		if (!(yyval.obj))
			YYABORT;
	;}
    break;

  case 12:

    {
		(yyval.obj)=(yyvsp[(1) - (1)].obj);
	;}
    break;

  case 13:

    {
		(yyval.obj)=(yyvsp[(1) - (1)].obj);
	;}
    break;

  case 14:

    {
		(yyval.obj)=new NONS_VariableMember(addStrings((yyvsp[(1) - (3)].obj)->getWcsCopy(),(yyvsp[(3) - (3)].obj)->getWcsCopy()),1);
		freeVM((yyvsp[(1) - (3)].obj));
		freeVM((yyvsp[(3) - (3)].obj));
	;}
    break;

  case 15:

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
	;}
    break;

  case 16:

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
	;}
    break;

  case 17:

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
	;}
    break;

  case 18:

    {
		handleErrors(NONS_ILLEGAL_ARRAY_SPECIFICATION,0,"yyparse",1);
		if (!!array_decl)
			array_decl->clear();
		(yyval.obj)=0;
	;}
    break;

  case 19:

    {
		(yyval.obj)=new NONS_VariableMember((yyvsp[(1) - (3)].obj)->getInt()+(yyvsp[(3) - (3)].obj)->getInt());
		(yyval.obj)->temporary=1;
		freeVM((yyvsp[(1) - (3)].obj));
		freeVM((yyvsp[(3) - (3)].obj));
	;}
    break;

  case 20:

    {
		(yyval.obj)=new NONS_VariableMember((yyvsp[(1) - (3)].obj)->getInt()-(yyvsp[(3) - (3)].obj)->getInt());
		(yyval.obj)->temporary=1;
		freeVM((yyvsp[(1) - (3)].obj));
		freeVM((yyvsp[(3) - (3)].obj));
	;}
    break;

  case 21:

    {
		(yyval.obj)=new NONS_VariableMember((yyvsp[(1) - (3)].obj)->getInt()*(yyvsp[(3) - (3)].obj)->getInt());
		(yyval.obj)->temporary=1;
		freeVM((yyvsp[(1) - (3)].obj));
		freeVM((yyvsp[(3) - (3)].obj));
	;}
    break;

  case 22:

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
	;}
    break;

  case 23:

    {
		if (!(yyvsp[(1) - (1)].obj)->temporary){
			(yyval.obj)=new NONS_VariableMember((yyvsp[(1) - (1)].obj)->getInt());
			(yyval.obj)->temporary=1;
			(yyval.obj)->negated=0;
		}else
			(yyval.obj)=(yyvsp[(1) - (1)].obj);
		(yyval.obj)->negate(invert_terms);
	;}
    break;

  case 24:

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
	;}
    break;

  case 25:

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
	;}
    break;

  case 26:

    {
		(yyval.obj)=new NONS_VariableMember((yyvsp[(2) - (2)].obj)->getInt());
		(yyval.obj)->temporary=1;
		freeVM((yyvsp[(2) - (2)].obj));
	;}
    break;

  case 27:

    {
		(yyval.obj)=new NONS_VariableMember(-((yyvsp[(2) - (2)].obj)->getInt()));
		(yyval.obj)->temporary=1;
		freeVM((yyvsp[(2) - (2)].obj));
	;}
    break;

  case 28:

    {
		(yyval.obj)=(yyvsp[(2) - (3)].obj);
	;}
    break;

  case 29:

    {
		NONS_Variable *v=store->retrieve((yyvsp[(2) - (2)].obj)->getInt(),0);
		freeVM((yyvsp[(2) - (2)].obj));
		if (!v){
			handleErrors(NONS_VARIABLE_OUT_OF_RANGE,0,"yyparse",1);
			(yyval.obj)=0;
		}else
			(yyval.obj)=v->intValue;
	;}
    break;

  case 30:

    {
		NONS_Variable *v=store->retrieve((yyvsp[(3) - (4)].obj)->getInt(),0);
		freeVM((yyvsp[(3) - (4)].obj));
		if (!v){
			handleErrors(NONS_VARIABLE_OUT_OF_RANGE,0,"yyparse",1);
			(yyval.obj)=0;
		}else
			(yyval.obj)=v->intValue;
	;}
    break;

  case 31:

    {
		NONS_Variable *v=store->retrieve((yyvsp[(2) - (2)].obj)->getInt(),0);
		freeVM((yyvsp[(2) - (2)].obj));
		if (!v){
			handleErrors(NONS_VARIABLE_OUT_OF_RANGE,0,"yyparse",1);
			(yyval.obj)=0;
		}else
			(yyval.obj)=v->wcsValue;
	;}
    break;

  case 32:

    {
		NONS_Variable *v=store->retrieve((yyvsp[(3) - (4)].obj)->getInt(),0);
		freeVM((yyvsp[(3) - (4)].obj));
		if (!v){
			handleErrors(NONS_VARIABLE_OUT_OF_RANGE,0,"yyparse",1);
			(yyval.obj)=0;
		}else
			(yyval.obj)=v->wcsValue;
	;}
    break;

  case 33:

    {
		(yyval.obj)=new NONS_VariableMember((yyvsp[(1) - (3)].obj)->getInt()==(yyvsp[(3) - (3)].obj)->getInt());
		(yyval.obj)->temporary=1;
		(yyval.obj)->negated=0;
		freeVM((yyvsp[(1) - (3)].obj));
		freeVM((yyvsp[(3) - (3)].obj));
	;}
    break;

  case 34:

    {
		(yyval.obj)=new NONS_VariableMember((yyvsp[(1) - (3)].obj)->getInt()!=(yyvsp[(3) - (3)].obj)->getInt());
		(yyval.obj)->temporary=1;
		(yyval.obj)->negated=0;
		freeVM((yyvsp[(1) - (3)].obj));
		freeVM((yyvsp[(3) - (3)].obj));
	;}
    break;

  case 35:

    {
		(yyval.obj)=new NONS_VariableMember((yyvsp[(1) - (3)].obj)->getInt()<(yyvsp[(3) - (3)].obj)->getInt());
		(yyval.obj)->temporary=1;
		(yyval.obj)->negated=0;
		freeVM((yyvsp[(1) - (3)].obj));
		freeVM((yyvsp[(3) - (3)].obj));
	;}
    break;

  case 36:

    {
		(yyval.obj)=new NONS_VariableMember((yyvsp[(1) - (3)].obj)->getInt()>=(yyvsp[(3) - (3)].obj)->getInt());
		(yyval.obj)->temporary=1;
		(yyval.obj)->negated=0;
		freeVM((yyvsp[(1) - (3)].obj));
		freeVM((yyvsp[(3) - (3)].obj));
	;}
    break;

  case 37:

    {
		(yyval.obj)=new NONS_VariableMember((yyvsp[(1) - (3)].obj)->getInt()>(yyvsp[(3) - (3)].obj)->getInt());
		(yyval.obj)->temporary=1;
		(yyval.obj)->negated=0;
		freeVM((yyvsp[(1) - (3)].obj));
		freeVM((yyvsp[(3) - (3)].obj));
	;}
    break;

  case 38:

    {
		(yyval.obj)=new NONS_VariableMember((yyvsp[(1) - (3)].obj)->getInt()<=(yyvsp[(3) - (3)].obj)->getInt());
		(yyval.obj)->temporary=1;
		(yyval.obj)->negated=0;
		freeVM((yyvsp[(1) - (3)].obj));
		freeVM((yyvsp[(3) - (3)].obj));
	;}
    break;

  case 39:

    {
		(yyval.obj)=new NONS_VariableMember(!(yyvsp[(2) - (2)].obj)->getInt());
		(yyval.obj)->temporary=1;
		(yyval.obj)->negated=0;
		freeVM((yyvsp[(2) - (2)].obj));
	;}
    break;

  case 40:

    {
		(yyval.obj)=new NONS_VariableMember(filelog->check((yyvsp[(2) - (2)].obj)->getWcs()));
		(yyval.obj)->temporary=1;
		(yyval.obj)->negated=0;
		freeVM((yyvsp[(2) - (2)].obj));
	;}
    break;

  case 41:

    {
		(yyval.obj)=new NONS_VariableMember(labellog.check((yyvsp[(2) - (2)].obj)->getWcs()));
		(yyval.obj)->temporary=1;
		freeVM((yyvsp[(2) - (2)].obj));
	;}
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

/*std::string WChar_to_UTF8(std::wstring &string){
	std::string res;
	for (std::wstring::iterator i=string.begin();i!=string.end();i++){
		if (*i<0x80)
			res.push_back(*i);
		else if (*i<0x800){
			res.push_back((*i>>6)|192);
			res.push_back((*i&63)|128);
		}else{
			res.push_back((*i>>12)|224);
			res.push_back(((*i&4095)>>6)|128);
			res.push_back((*i&63)|128);
		}
	}
	return res;
}*/

long atol(std::wstring &str){
	long res=0;
	size_t a=0;
	for (;a<str.size() && iswhitespace(str[a]);a++);
	int sign;
	if (a<str.size() && str[a]=='-'){
		sign=-1;
		a++;
	}else
		sign=1;
	for (;a<str.size();a++){
		if (!NONS_isdigit(str[a]))
			return 0;
		res=res*10+(str[a]-'0')*sign;
	}
	return res;
}

template <typename T>
int strcmpCI(T *a,T *b){
    for (;*a || *b;a++,b++){
		char c=(*a>='A' && *a<='Z')?*a|32:*a,
			d=(*b>='A' && *b<='Z')?*b|32:*b;
		if (c<d)
            return -1;
        if (c>d)
            return 1;
    }
	return 0;
}

#define DOUBLEOP(character,ret_value) if (c==(character)){\
	stream->get();\
	if (stream->peek()==(character)){\
		stream->get();\
		return (ret_value);\
	}\
	return (ret_value);\
}

template <typename T>
T HEX2DEC(T x){
	return x<='9'?x-'0':(x<='F'?x-'A'+10:x-'a'+10);
}

int yylex(YYSTYPE *yylval,std::wstringstream *stream,NONS_VariableStore *store,NONS_VariableMember **retrievedVar){
	int c;
	for (;(c=stream->peek())!=EOF && iswhitespace((wchar_t)c);stream->get());
	if (c==EOF || c==0xFFFF)
		return 0;
	if (NONS_isdigit(c)){
		std::wstring temp;
		while (NONS_isdigit(stream->peek()))
			temp.push_back(stream->get());
		yylval->obj=new NONS_VariableMember(atol(temp));
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
			a=a*16+HEX2DEC(temp[b]);
		yylval->obj=new NONS_VariableMember(a);
		yylval->obj->temporary=1;
		return INTEGER;
	}
	if (c=='\"' || c=='`'){
		c=stream->get();
		std::wstring temp;
		while (stream->peek()!=c && stream->peek()!=EOF && stream->peek()!=0xFFFF)
			temp.push_back(stream->get());
		if (stream->peek()!=c)
			handleErrors(NONS_UNMATCHED_QUOTES,0,"yylex",1);
		else
			stream->get();
		yylval->obj=new NONS_VariableMember(temp.c_str(),0);
		yylval->obj->temporary=1;
		return STRING;
	}
	if (c=='*'){
		std::wstring backup;
		backup.push_back(stream->get());
		while (iswhitespace((wchar_t)stream->peek()))
			backup.push_back(stream->get());
		std::wstring identifier;
		c=stream->peek();
		if (c=='_' || NONS_isalpha(c)){
			identifier.push_back(c);
			backup.push_back(stream->get());
			while ((c=stream->peek())=='_' || NONS_isalnum(c)){
				identifier.push_back(c);
				backup.push_back(stream->get());
			}
			if (((NONS_ScriptInterpreter *)gScriptInterpreter)->script->offsetFromBlock(identifier.c_str())>=0){
				yylval->obj=new NONS_VariableMember(identifier.c_str(),0);
				yylval->obj->temporary=1;
				return STRING;
			}
		}
		for (std::wstring::reverse_iterator i=backup.rbegin();i!=backup.rend();i++)
			stream->putback(*i);
	}
	if (c=='_' || NONS_isalpha(c)){
		std::wstring temp;
		temp.push_back(stream->get());
		while (NONS_isalnum(c=stream->peek()) || c=='_')
			temp.push_back(stream->get());
		if (!strcmpCI(temp.c_str(),L"fchk"))
			return FCHK;
		if (!strcmpCI(temp.c_str(),L"lchk"))
			return LCHK;
		if (!strcmpCI(temp.c_str(),L"_eval"))
			return LCHK;
		if (!strcmpCI(temp.c_str(),L"_itoa"))
			return ITOA;
		if (!strcmpCI(temp.c_str(),L"_atoi"))
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
			//yylloc.last_column=stream.getPosition();
			return NEQ;
		}
		//yylloc.last_column=stream.getPosition();
		return c;
	}
	if (c=='<'){
		stream->get();
		if (stream->peek()=='='){
			stream->get();
			//yylloc.last_column=stream.getPosition();
			return LOWEREQ;
		}
		if (stream->peek()=='>'){
			stream->get();
			//yylloc.last_column=stream.getPosition();
			return NEQ;
		}
		//yylloc.last_column=stream.getPosition();
		return LOWER;
	}
	if (c=='>'){
		stream->get();
		if (stream->peek()=='='){
			stream->get();
			//yylloc.last_column=stream.getPosition();
			return GREATEREQ;
		}
		//yylloc.last_column=stream.getPosition();
		return GREATER;
	}
	//yylloc.last_column=stream.getPosition();
	if (!multicomparison((wchar_t)c,"+-*/![]%$?()")){
		wchar_t temp[]={c,0};
		handleErrors(NONS_UNRECOGNIZED_OPERATOR,0,"yylex",1,temp);
	}
	return stream->get();
}

void yyerror(
		std::wstringstream *,
		NONS_VariableStore *,
		NONS_FileLog *,
		long *,
		bool,
		std::vector<long> *,
		NONS_VariableMember **retrievedVar,
		wchar_t **string,
		char const *s){
	wchar_t *temp=copyWString(s);
	if (!retrievedVar)
		handleErrors(NONS_UNDEFINED_ERROR,0,"yyparse",1,temp);
	delete[] temp;
}

