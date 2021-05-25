/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

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
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     SEMICOLON = 258,
     CREATE = 259,
     DROP = 260,
     TABLE = 261,
     TABLES = 262,
     INDEX = 263,
     SELECT = 264,
     DESC = 265,
     SHOW = 266,
     SYNC = 267,
     INSERT = 268,
     DELETE = 269,
     UPDATE = 270,
     LBRACE = 271,
     RBRACE = 272,
     COMMA = 273,
     TRX_BEGIN = 274,
     TRX_COMMIT = 275,
     TRX_ROLLBACK = 276,
     INT_T = 277,
     STRING_T = 278,
     FLOAT_T = 279,
     HELP = 280,
     EXIT = 281,
     DOT = 282,
     INTO = 283,
     VALUES = 284,
     FROM = 285,
     WHERE = 286,
     AND = 287,
     SET = 288,
     ON = 289,
     EQ = 290,
     LT = 291,
     GT = 292,
     LE = 293,
     GE = 294,
     NE = 295,
     NUMBER = 296,
     FLOAT = 297,
     ID = 298,
     SSS = 299,
     STAR = 300,
     STRING_V = 301
   };
#endif
/* Tokens.  */
#define SEMICOLON 258
#define CREATE 259
#define DROP 260
#define TABLE 261
#define TABLES 262
#define INDEX 263
#define SELECT 264
#define DESC 265
#define SHOW 266
#define SYNC 267
#define INSERT 268
#define DELETE 269
#define UPDATE 270
#define LBRACE 271
#define RBRACE 272
#define COMMA 273
#define TRX_BEGIN 274
#define TRX_COMMIT 275
#define TRX_ROLLBACK 276
#define INT_T 277
#define STRING_T 278
#define FLOAT_T 279
#define HELP 280
#define EXIT 281
#define DOT 282
#define INTO 283
#define VALUES 284
#define FROM 285
#define WHERE 286
#define AND 287
#define SET 288
#define ON 289
#define EQ 290
#define LT 291
#define GT 292
#define LE 293
#define GE 294
#define NE 295
#define NUMBER 296
#define FLOAT 297
#define ID 298
#define SSS 299
#define STAR 300
#define STRING_V 301




/* Copy the first part of user declarations.  */
#line 2 "yacc_sql.y"


#include "handler/handler_defs.h"
// #include "common/log/log.h" // TODO 包含C++中的头文件

extern char * position;
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
extern int yylex(void);
//获取子串
char *substr(const char *s,int n1,int n2)/*从s中提取下标为n1~n2的字符组成一个新字符串，然后返回这个新串的首地址*/
{
	char *sp=malloc(sizeof(char)*(n2-n1+2));
	int i,j=0;
	for (i=n1; i<=n2; i++) {
		sp[j++]=s[i];
	}
	sp[j]=0;
	return sp;
}
  sqlstr *ssql;
  struct _Condition wherecondi[MAX_NUM];

	/*临时中间变量*/
int whereleng=0; 
int fromleng=0;
int selectleng=0;
int valueleng=0;
int i;
char get_id[MAX_NUM];
char *temp;

Value valueT[MAX_NUM];

int compOpT;
void yyerror(const char *str) {

	ssql->flag=0;
	whereleng=0; 
  fromleng=0;
  selectleng=0;
	valueleng=0;
  ssql->sstr.ins.nValues=0;
	ssql->sstr.errors=position;
	printf("parse sql failed. error=%s", str);
}



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

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 91 "yacc_sql.y"
{
    struct _Attr *attr;
	  struct _Condition *condition1;
  	struct _Value *value1;
    char *string;
  //  char *ssss;
    int number;
    float floats;
}
/* Line 193 of yacc.c.  */
#line 248 "yacc_sql.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 261 "yacc_sql.tab.c"

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
# if defined YYENABLE_NLS && YYENABLE_NLS
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
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
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
  yytype_int16 yyss;
  YYSTYPE yyvs;
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
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   139

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  47
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  33
/* YYNRULES -- Number of rules.  */
#define YYNRULES  74
/* YYNRULES -- Number of states.  */
#define YYNSTATES  153

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   301

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     4,     7,     9,    11,    13,    15,    17,
      19,    21,    23,    25,    27,    29,    31,    33,    35,    37,
      39,    42,    45,    48,    51,    54,    57,    62,    66,    70,
      80,    85,    94,    95,    99,   105,   108,   110,   112,   114,
     116,   118,   128,   129,   133,   135,   137,   139,   145,   154,
     162,   164,   167,   172,   173,   177,   183,   184,   188,   189,
     193,   194,   198,   202,   206,   210,   214,   220,   226,   234,
     236,   238,   240,   242,   244
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      48,     0,    -1,    -1,    48,    49,    -1,    72,    -1,    67,
      -1,    71,    -1,    70,    -1,    61,    -1,    56,    -1,    57,
      -1,    58,    -1,    59,    -1,    60,    -1,    52,    -1,    53,
      -1,    54,    -1,    55,    -1,    51,    -1,    50,    -1,    26,
       3,    -1,    25,     3,    -1,    12,     3,    -1,    19,     3,
      -1,    20,     3,    -1,    21,     3,    -1,     5,     6,    43,
       3,    -1,    11,     7,     3,    -1,    10,    43,     3,    -1,
       4,     8,    43,    34,    43,    16,    43,    17,     3,    -1,
       5,     8,    43,     3,    -1,     4,     6,    43,    16,    63,
      62,    17,     3,    -1,    -1,    18,    63,    62,    -1,    66,
      65,    16,    64,    17,    -1,    66,    65,    -1,    41,    -1,
      22,    -1,    23,    -1,    24,    -1,    43,    -1,    13,    28,
      43,    29,    16,    69,    68,    17,     3,    -1,    -1,    18,
      69,    68,    -1,    41,    -1,    42,    -1,    44,    -1,    14,
      30,    43,    76,     3,    -1,    15,    43,    33,    43,    35,
      69,    76,     3,    -1,     9,    73,    30,    43,    75,    76,
       3,    -1,    45,    -1,    43,    74,    -1,    43,    27,    43,
      74,    -1,    -1,    18,    43,    74,    -1,    18,    43,    27,
      43,    74,    -1,    -1,    18,    43,    75,    -1,    -1,    31,
      78,    77,    -1,    -1,    32,    78,    77,    -1,    43,    79,
      69,    -1,    69,    79,    69,    -1,    43,    79,    43,    -1,
      69,    79,    43,    -1,    43,    27,    43,    79,    69,    -1,
      69,    79,    43,    27,    43,    -1,    43,    27,    43,    79,
      43,    27,    43,    -1,    35,    -1,    36,    -1,    37,    -1,
      38,    -1,    39,    -1,    40,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   116,   116,   118,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     155,   160,   165,   171,   177,   183,   189,   195,   201,   208,
     217,   223,   233,   235,   239,   246,   255,   258,   259,   260,
     263,   273,   288,   290,   296,   302,   308,   318,   330,   344,
     365,   369,   373,   378,   380,   384,   390,   392,   396,   398,
     402,   404,   409,   421,   434,   445,   458,   470,   482,   495,
     496,   497,   498,   499,   500
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "SEMICOLON", "CREATE", "DROP", "TABLE",
  "TABLES", "INDEX", "SELECT", "DESC", "SHOW", "SYNC", "INSERT", "DELETE",
  "UPDATE", "LBRACE", "RBRACE", "COMMA", "TRX_BEGIN", "TRX_COMMIT",
  "TRX_ROLLBACK", "INT_T", "STRING_T", "FLOAT_T", "HELP", "EXIT", "DOT",
  "INTO", "VALUES", "FROM", "WHERE", "AND", "SET", "ON", "EQ", "LT", "GT",
  "LE", "GE", "NE", "NUMBER", "FLOAT", "ID", "SSS", "STAR", "STRING_V",
  "$accept", "commands", "command", "exit", "help", "sync", "begin",
  "commit", "rollback", "drop_table", "show_tables", "desc_table",
  "create_index", "drop_index", "create_table", "attr_def_list",
  "attr_def", "number", "type", "ID_get", "insert", "value_list", "value",
  "delete", "update", "select", "select_attr", "attr_list", "rel_list",
  "where", "condition_list", "condition", "comOp", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    47,    48,    48,    49,    49,    49,    49,    49,    49,
      49,    49,    49,    49,    49,    49,    49,    49,    49,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    62,    63,    63,    64,    65,    65,    65,
      66,    67,    68,    68,    69,    69,    69,    70,    71,    72,
      73,    73,    73,    74,    74,    74,    75,    75,    76,    76,
      77,    77,    78,    78,    78,    78,    78,    78,    78,    79,
      79,    79,    79,    79,    79
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     2,     2,     2,     2,     2,     4,     3,     3,     9,
       4,     8,     0,     3,     5,     2,     1,     1,     1,     1,
       1,     9,     0,     3,     1,     1,     1,     5,     8,     7,
       1,     2,     4,     0,     3,     5,     0,     3,     0,     3,
       0,     3,     3,     3,     3,     3,     5,     5,     7,     1,
       1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     3,    19,    18,
      14,    15,    16,    17,     9,    10,    11,    12,    13,     8,
       5,     7,     6,     4,     0,     0,     0,     0,    53,    50,
       0,     0,     0,    22,     0,     0,     0,    23,    24,    25,
      21,    20,     0,     0,     0,     0,     0,     0,    51,     0,
      28,    27,     0,    58,     0,     0,     0,    26,    30,    53,
      53,    56,     0,     0,     0,     0,    40,    32,     0,     0,
       0,    54,    52,     0,    58,     0,    44,    45,     0,    46,
       0,    60,    47,     0,     0,     0,    37,    38,    39,    35,
       0,    53,    56,     0,    42,     0,    69,    70,    71,    72,
      73,    74,     0,     0,     0,    59,    58,    32,     0,     0,
       0,    55,    57,    49,     0,     0,     0,    64,    62,    65,
      63,    60,     0,    33,    31,    36,     0,     0,    42,     0,
       0,     0,    61,    48,    34,    29,    43,    41,     0,    66,
      67,     0,    68
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    95,    77,   136,    99,    78,
      30,   125,    90,    31,    32,    33,    40,    58,    84,    74,
     115,    91,   112
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -90
static const yytype_int8 yypact[] =
{
     -90,     5,   -90,    32,    73,    37,   -37,     0,     8,     1,
      -7,   -11,    50,    51,    71,    80,    81,   -90,   -90,   -90,
     -90,   -90,   -90,   -90,   -90,   -90,   -90,   -90,   -90,   -90,
     -90,   -90,   -90,   -90,    -8,    42,    43,    44,    -6,   -90,
      58,    86,    87,   -90,    48,    49,    60,   -90,   -90,   -90,
     -90,   -90,    78,    61,    93,    94,    55,    56,   -90,    57,
     -90,   -90,    72,    74,    59,    63,    64,   -90,   -90,    -5,
      85,    90,    88,    15,   106,    75,   -90,    95,    54,    96,
      68,   -90,   -90,    76,    74,    31,   -90,   -90,     6,   -90,
      12,    82,   -90,    31,    63,    98,   -90,   -90,   -90,   100,
      77,    85,    90,   114,   103,    79,   -90,   -90,   -90,   -90,
     -90,   -90,    19,    23,    15,   -90,    74,    95,   115,    83,
     108,   -90,   -90,   -90,    31,   109,    12,   -90,   -90,   101,
     -90,    82,   120,   -90,   -90,   -90,   110,   126,   103,   127,
      27,    89,   -90,   -90,   -90,   -90,   -90,   -90,   104,   -90,
     -90,    91,   -90
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -90,   -90,   -90,   -90,   -90,   -90,   -90,   -90,   -90,   -90,
     -90,   -90,   -90,   -90,   -90,    16,    41,   -90,   -90,   -90,
     -90,    -2,   -85,   -90,   -90,   -90,   -90,   -67,    35,   -80,
       7,    25,   -89
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
     104,   113,    81,    82,   103,     2,    41,    42,   116,     3,
       4,    43,    56,    56,     5,     6,     7,     8,     9,    10,
      11,    57,    80,    45,    12,    13,    14,   128,   130,    44,
      15,    16,    46,   105,   121,    52,   132,   140,    34,   138,
      35,   106,   107,   108,   109,   110,   111,   106,   107,   108,
     109,   110,   111,    47,    48,   149,    86,    87,    88,    89,
      86,    87,   127,    89,    86,    87,   129,    89,    86,    87,
     148,    89,    86,    87,    49,    89,    96,    97,    98,    36,
      38,    37,    39,    50,    51,    53,    54,    55,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    75,    56,    85,    73,    76,    79,    83,    92,
      93,   101,   100,    94,   114,   118,   119,   123,   134,   102,
     120,   124,   126,   143,   135,   137,   139,   144,   141,   145,
     147,   151,   150,   133,   152,   117,   146,   122,   142,   131
};

static const yytype_uint8 yycheck[] =
{
      85,    90,    69,    70,    84,     0,    43,     7,    93,     4,
       5,     3,    18,    18,     9,    10,    11,    12,    13,    14,
      15,    27,    27,    30,    19,    20,    21,   112,   113,    28,
      25,    26,    43,    27,   101,    43,   116,   126,     6,   124,
       8,    35,    36,    37,    38,    39,    40,    35,    36,    37,
      38,    39,    40,     3,     3,   140,    41,    42,    43,    44,
      41,    42,    43,    44,    41,    42,    43,    44,    41,    42,
      43,    44,    41,    42,     3,    44,    22,    23,    24,     6,
      43,     8,    45,     3,     3,    43,    43,    43,    30,     3,
       3,    43,    43,    33,    16,    34,     3,     3,    43,    43,
      43,    29,    43,    18,    16,    31,    43,    43,    18,     3,
      35,    43,    16,    18,    32,    17,    16,     3,     3,    43,
      43,    18,    43,     3,    41,    17,    17,    17,    27,     3,
       3,    27,    43,   117,    43,    94,   138,   102,   131,   114
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    48,     0,     4,     5,     9,    10,    11,    12,    13,
      14,    15,    19,    20,    21,    25,    26,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      67,    70,    71,    72,     6,     8,     6,     8,    43,    45,
      73,    43,     7,     3,    28,    30,    43,     3,     3,     3,
       3,     3,    43,    43,    43,    43,    18,    27,    74,    30,
       3,     3,    43,    43,    33,    16,    34,     3,     3,    43,
      43,    43,    29,    31,    76,    43,    43,    63,    66,    43,
      27,    74,    74,    18,    75,    16,    41,    42,    43,    44,
      69,    78,     3,    35,    18,    62,    22,    23,    24,    65,
      16,    43,    43,    76,    69,    27,    35,    36,    37,    38,
      39,    40,    79,    79,    32,    77,    69,    63,    17,    16,
      43,    74,    75,     3,    18,    68,    43,    43,    69,    43,
      69,    78,    76,    62,     3,    41,    64,    17,    69,    17,
      79,    27,    77,     3,    17,     3,    68,     3,    43,    69,
      43,    27,    43
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
      yyerror (YY_("syntax error: cannot back up")); \
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
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
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
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
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
		  Type, Value); \
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
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
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
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
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
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

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
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

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
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

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
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

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

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
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

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
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
        case 20:
#line 155 "yacc_sql.y"
    {
        ssql->flag=SCF_EXIT;//"exit";
    }
    break;

  case 21:
#line 160 "yacc_sql.y"
    {
        ssql->flag=SCF_HELP;//"help";
    }
    break;

  case 22:
#line 165 "yacc_sql.y"
    {
      ssql->flag = SCF_SYNC;
    }
    break;

  case 23:
#line 171 "yacc_sql.y"
    {
      ssql->flag = SCF_BEGIN;
    }
    break;

  case 24:
#line 177 "yacc_sql.y"
    {
      ssql->flag = SCF_COMMIT;
    }
    break;

  case 25:
#line 183 "yacc_sql.y"
    {
      ssql->flag = SCF_ROLLBACK;
    }
    break;

  case 26:
#line 189 "yacc_sql.y"
    {
        ssql->flag=SCF_DROP_TABLE;//"drop_table";
        ssql->sstr.drt.relName=(yyvsp[(3) - (4)].string);
    }
    break;

  case 27:
#line 195 "yacc_sql.y"
    {
      ssql->flag=SCF_SHOW_TABLES;
    }
    break;

  case 28:
#line 201 "yacc_sql.y"
    {
      ssql->flag = SCF_DESC_TABLE;
      ssql->sstr.desc_table.table_name = (yyvsp[(2) - (3)].string);
    }
    break;

  case 29:
#line 208 "yacc_sql.y"
    {
																							        ssql->flag=SCF_CREATE_INDEX;//"create_index";
																							        ssql->sstr.crei.indexName=(yyvsp[(3) - (9)].string);
																							        ssql->sstr.crei.relName=(yyvsp[(5) - (9)].string);
																							        ssql->sstr.crei.attrName=(yyvsp[(7) - (9)].string);
																							    }
    break;

  case 30:
#line 217 "yacc_sql.y"
    {
											        ssql->flag=SCF_DROP_INDEX;//"drop_index";
											        ssql->sstr.dri.indexName=(yyvsp[(3) - (4)].string);
											    }
    break;

  case 31:
#line 223 "yacc_sql.y"
    {
	
																																		ssql->flag=SCF_CREATE_TABLE;//"create_table";
																																		ssql->sstr.cret.relName=(yyvsp[(3) - (8)].string);
																																		ssql->sstr.cret.attrCount=valueleng;
																																		//临时变量清零	
																																		valueleng=0;
																																		
																																	    }
    break;

  case 33:
#line 235 "yacc_sql.y"
    {    }
    break;

  case 34:
#line 239 "yacc_sql.y"
    {
																ssql->sstr.cret.attributes[valueleng].attrName=(char*)malloc(sizeof(char)); // TODO FATAL
																strcpy(ssql->sstr.cret.attributes[valueleng].attrName,get_id); 
																ssql->sstr.cret.attributes[valueleng].attrType=(yyvsp[(2) - (5)].number);  
																ssql->sstr.cret.attributes[valueleng].attrLength=(yyvsp[(4) - (5)].number);
																valueleng++;
															    }
    break;

  case 35:
#line 246 "yacc_sql.y"
    {
															ssql->sstr.cret.attributes[valueleng].attrName=(char*)malloc(sizeof(char));
															strcpy(ssql->sstr.cret.attributes[valueleng].attrName,get_id); 
															ssql->sstr.cret.attributes[valueleng].attrType=(yyvsp[(2) - (2)].number);  
															ssql->sstr.cret.attributes[valueleng].attrLength=4;
															valueleng++;
														    }
    break;

  case 36:
#line 255 "yacc_sql.y"
    {(yyval.number) = (yyvsp[(1) - (1)].number);}
    break;

  case 37:
#line 258 "yacc_sql.y"
    { (yyval.number)=2; }
    break;

  case 38:
#line 259 "yacc_sql.y"
    { (yyval.number)=1; }
    break;

  case 39:
#line 260 "yacc_sql.y"
    { (yyval.number)=3; }
    break;

  case 40:
#line 263 "yacc_sql.y"
    {
				 temp=(yyvsp[(1) - (1)].string); 
				for(i=0;i<MAX_NUM;i++){
				get_id[i]=*(temp+i);
				}
			}
    break;

  case 41:
#line 273 "yacc_sql.y"
    {
        valueT[valueleng++] = *(yyvsp[(6) - (9)].value1);

        ssql->flag=SCF_INSERT;//"insert";
        ssql->sstr.ins.relName=(yyvsp[(3) - (9)].string);
        ssql->sstr.ins.nValues=valueleng;
        for(i=0;i<valueleng;i++){
        ssql->sstr.ins.values[i] = valueT[i];
      }

      //临时变量清零
      valueleng=0;
      //	free(valueT);
    }
    break;

  case 43:
#line 290 "yacc_sql.y"
    { 
  			 valueT[valueleng++] = *(yyvsp[(2) - (3)].value1);
	}
    break;

  case 44:
#line 296 "yacc_sql.y"
    {	
					(yyval.value1) = (Value*)malloc(sizeof(Value));
					(yyval.value1)->data=(int*)malloc((sizeof(int)));
					(yyval.value1)->type =ints; 					                      // "ints";
					*((int*)((yyval.value1)->data)) = (yyvsp[(1) - (1)].number);
			}
    break;

  case 45:
#line 302 "yacc_sql.y"
    {								
					(yyval.value1)=(Value*)malloc(sizeof(Value));
					(yyval.value1)->data=malloc(MAX_NUM*(sizeof(char)));
					(yyval.value1)->type =floats; 					                     	//"floats"
					*((float*)((yyval.value1)->data)) = (yyvsp[(1) - (1)].floats);
			}
    break;

  case 46:
#line 308 "yacc_sql.y"
    {																		
					(yyval.value1)=(Value*)malloc(sizeof(Value));				
					(yyval.value1)->data=malloc(MAX_NUM*(sizeof(char)));		
    			(yyval.value1)->type = chars;					// "chars";
    			(yyvsp[(1) - (1)].string) = substr((yyvsp[(1) - (1)].string),1,strlen((yyvsp[(1) - (1)].string))-2); // TODO snprintf
    			sprintf((char*)((yyval.value1)->data), "%s", (yyvsp[(1) - (1)].string)); 			
    	}
    break;

  case 47:
#line 318 "yacc_sql.y"
    {
	
	ssql->flag=SCF_DELETE;//"delete";
	ssql->sstr.del.relName=(yyvsp[(3) - (5)].string);
	for(i=0;i<whereleng;i++){
	ssql->sstr.del.conditions[i]=wherecondi[i];
	}
	ssql->sstr.del.nConditions=whereleng;
	whereleng=0;	
    }
    break;

  case 48:
#line 330 "yacc_sql.y"
    {
	
											ssql->flag=SCF_UPDATE;//"update";
											ssql->sstr.upd.relName=(yyvsp[(2) - (8)].string);
											ssql->sstr.upd.value=*(yyvsp[(6) - (8)].value1);
											ssql->sstr.upd.attrName=(yyvsp[(4) - (8)].string);
											for(i=0;i<whereleng;i++){
											  ssql->sstr.upd.conditions[i]=wherecondi[i];
											}
											ssql->sstr.upd.nConditions=whereleng;
											whereleng=0;
										    }
    break;

  case 49:
#line 344 "yacc_sql.y"
    {
      
            ssql->sstr.sel.relations[fromleng++]=(yyvsp[(4) - (7)].string);

            for(i=0;i<whereleng;i++){
              ssql->sstr.sel.conditions[i]=wherecondi[i];
            }

            ssql->flag=SCF_SELECT;//"select";
            ssql->sstr.sel.nSelAttrs=selectleng;
            ssql->sstr.sel.nRelations=fromleng;
            ssql->sstr.sel.nConditions=whereleng;

            //临时变量清零
            whereleng=0;
            fromleng=0;
            selectleng=0;
        }
    break;

  case 50:
#line 365 "yacc_sql.y"
    {  // TODO 处理这里的内存泄露问题
         ssql->sstr.sel.selAttrs[selectleng].attrName="*";
         ssql->sstr.sel.selAttrs[selectleng++].relName=NULL;
       }
    break;

  case 51:
#line 369 "yacc_sql.y"
    {
        ssql->sstr.sel.selAttrs[selectleng].attrName=(yyvsp[(1) - (2)].string);
        ssql->sstr.sel.selAttrs[selectleng++].relName=NULL;
      }
    break;

  case 52:
#line 373 "yacc_sql.y"
    {
        ssql->sstr.sel.selAttrs[selectleng].attrName=(yyvsp[(3) - (4)].string);
        ssql->sstr.sel.selAttrs[selectleng++].relName=(yyvsp[(1) - (4)].string);
      }
    break;

  case 54:
#line 380 "yacc_sql.y"
    {
     	  ssql->sstr.sel.selAttrs[selectleng].relName = NULL;
        ssql->sstr.sel.selAttrs[selectleng++].attrName=(yyvsp[(2) - (3)].string);
      }
    break;

  case 55:
#line 384 "yacc_sql.y"
    {
        ssql->sstr.sel.selAttrs[selectleng].attrName=(yyvsp[(4) - (5)].string);
        ssql->sstr.sel.selAttrs[selectleng++].relName=(yyvsp[(2) - (5)].string);
  	  }
    break;

  case 57:
#line 392 "yacc_sql.y"
    {	
				ssql->sstr.sel.relations[fromleng++]=(yyvsp[(2) - (3)].string);
		  }
    break;

  case 59:
#line 398 "yacc_sql.y"
    {	
				wherecondi[whereleng++]=*(yyvsp[(2) - (3)].condition1);
			}
    break;

  case 61:
#line 404 "yacc_sql.y"
    {
				wherecondi[whereleng++]=*(yyvsp[(2) - (3)].condition1);
			}
    break;

  case 62:
#line 409 "yacc_sql.y"
    {
									(yyval.condition1) = ( Condition *)malloc(sizeof( Condition));
									(yyval.condition1)->bLhsIsAttr = 1;
									(yyval.condition1)->lhsAttr.relName = NULL;
									(yyval.condition1)->lhsAttr.attrName = (yyvsp[(1) - (3)].string);
									(yyval.condition1)->op = compOpT;
									(yyval.condition1)->bRhsIsAttr = 0;
									(yyval.condition1)->rhsAttr.relName = NULL;
									(yyval.condition1)->rhsAttr.attrName = NULL;
									(yyval.condition1)->rhsValue = *(yyvsp[(3) - (3)].value1);

								    }
    break;

  case 63:
#line 421 "yacc_sql.y"
    {
									(yyval.condition1) = ( Condition *)malloc(sizeof( Condition));
									(yyval.condition1)->bLhsIsAttr = 0;
									(yyval.condition1)->lhsAttr.relName=NULL;
									(yyval.condition1)->lhsAttr.attrName=NULL;
									(yyval.condition1)->lhsValue = *(yyvsp[(1) - (3)].value1);
									(yyval.condition1)->op = compOpT;
									(yyval.condition1)->bRhsIsAttr = 0;
									(yyval.condition1)->rhsAttr.relName = NULL;
									(yyval.condition1)->rhsAttr.attrName = NULL;
									(yyval.condition1)->rhsValue = *(yyvsp[(3) - (3)].value1);

								    }
    break;

  case 64:
#line 434 "yacc_sql.y"
    {
									(yyval.condition1)=( Condition *)malloc(sizeof( Condition));
									(yyval.condition1)->bLhsIsAttr = 1;
									(yyval.condition1)->lhsAttr.relName=NULL;
									(yyval.condition1)->lhsAttr.attrName=(yyvsp[(1) - (3)].string);
									(yyval.condition1)->op=compOpT;
									(yyval.condition1)->bRhsIsAttr = 1;
									(yyval.condition1)->rhsAttr.relName=NULL;
									(yyval.condition1)->rhsAttr.attrName=(yyvsp[(3) - (3)].string);

								    }
    break;

  case 65:
#line 445 "yacc_sql.y"
    {
									(yyval.condition1)=( Condition *)malloc(sizeof( Condition));
									(yyval.condition1)->bLhsIsAttr = 0;
									(yyval.condition1)->lhsAttr.relName=NULL;
									(yyval.condition1)->lhsAttr.attrName=NULL;
									(yyval.condition1)->lhsValue = *(yyvsp[(1) - (3)].value1);
									(yyval.condition1)->op=compOpT;
									
									(yyval.condition1)->bRhsIsAttr = 1;
									(yyval.condition1)->rhsAttr.relName=NULL;
									(yyval.condition1)->rhsAttr.attrName=(yyvsp[(3) - (3)].string);
								
								    }
    break;

  case 66:
#line 458 "yacc_sql.y"
    {
									(yyval.condition1)=( Condition *)malloc(sizeof( Condition));
									(yyval.condition1)->bLhsIsAttr = 1;
									(yyval.condition1)->lhsAttr.relName=(yyvsp[(1) - (5)].string);
									(yyval.condition1)->lhsAttr.attrName=(yyvsp[(3) - (5)].string);
									(yyval.condition1)->op=compOpT;
									(yyval.condition1)->bRhsIsAttr = 0;   //属性值
									(yyval.condition1)->rhsAttr.relName=NULL;
									(yyval.condition1)->rhsAttr.attrName=NULL;
									(yyval.condition1)->rhsValue=*(yyvsp[(5) - (5)].value1);			
							
    											}
    break;

  case 67:
#line 470 "yacc_sql.y"
    {
									(yyval.condition1)=( Condition *)malloc(sizeof( Condition));
									(yyval.condition1)->bLhsIsAttr = 0;//属性值
									(yyval.condition1)->lhsAttr.relName=NULL;
									(yyval.condition1)->lhsAttr.attrName=NULL;
									(yyval.condition1)->lhsValue = *(yyvsp[(1) - (5)].value1);
									(yyval.condition1)->op=compOpT;
									(yyval.condition1)->bRhsIsAttr = 1;//属性
									(yyval.condition1)->rhsAttr.relName = (yyvsp[(3) - (5)].string);
									(yyval.condition1)->rhsAttr.attrName = (yyvsp[(5) - (5)].string);
									
    						}
    break;

  case 68:
#line 482 "yacc_sql.y"
    {
									(yyval.condition1)=( Condition *)malloc(sizeof( Condition));
									(yyval.condition1)->bLhsIsAttr = 1;		//属性
									(yyval.condition1)->lhsAttr.relName=(yyvsp[(1) - (7)].string);
									(yyval.condition1)->lhsAttr.attrName=(yyvsp[(3) - (7)].string);
									(yyval.condition1)->op=compOpT;
									(yyval.condition1)->bRhsIsAttr = 1;		//属性
									(yyval.condition1)->rhsAttr.relName=(yyvsp[(5) - (7)].string);
									(yyval.condition1)->rhsAttr.attrName=(yyvsp[(7) - (7)].string);								
    											}
    break;

  case 69:
#line 495 "yacc_sql.y"
    { compOpT = EQual; }
    break;

  case 70:
#line 496 "yacc_sql.y"
    { compOpT = LEqual; }
    break;

  case 71:
#line 497 "yacc_sql.y"
    { compOpT = GreatT; }
    break;

  case 72:
#line 498 "yacc_sql.y"
    { compOpT = LessT; }
    break;

  case 73:
#line 499 "yacc_sql.y"
    { compOpT = GEqual; }
    break;

  case 74:
#line 500 "yacc_sql.y"
    { compOpT = NEqual; }
    break;


/* Line 1267 of yacc.c.  */
#line 2051 "yacc_sql.tab.c"
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
      yyerror (YY_("syntax error"));
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
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
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
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
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
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

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

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
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


#line 503 "yacc_sql.y"

//_____________________________________________________________________
int hust_parse( sqlstr *sqls){
	ssql=sqls;
	return yyparse();
}

