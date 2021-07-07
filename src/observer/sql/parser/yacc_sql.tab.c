/* A Bison parser, made by GNU Bison 3.7.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.7"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 2 "yacc_sql.y"


#include "sql/parser/parse_defs.h"
#include "sql/parser/yacc_sql.tab.h"
#include "sql/parser/lex.yy.h"
// #include "common/log/log.h" // TODO 包含C++中的头文件

extern char * position;
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct ParserContext {
  Query * ssql;
  int select_length;
  int condition_length;
  int from_length;
  int value_length;
  int tuple_num;
  int tuple_length[MAX_NUM];
  Value tuples[MAX_NUM][MAX_NUM];
  Value values[MAX_NUM];
  Condition conditions[MAX_NUM];
  CompOp comp;
  AggOp agg;
  char id[MAX_NUM];
} ParserContext;

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

void yyerror(yyscan_t scanner, const char *str) {

	ParserContext *context = (ParserContext *)(yyget_extra(scanner));
	query_reset(context->ssql);
	context->ssql->flag = SCF_ERROR;
	context->condition_length = 0;
	context->from_length = 0;
	context->select_length = 0;
	context->value_length = 0;
  	context->ssql->sstr.insertion.tuple_num = 0;
	context->ssql->sstr.errors = position;
	printf("parse sql failed. error=%s", str);
}

ParserContext *get_context(yyscan_t scanner) {
	return (ParserContext *)yyget_extra(scanner);
}

#define CONTEXT get_context(scanner)


#line 133 "yacc_sql.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "yacc_sql.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_SEMICOLON = 3,                  /* SEMICOLON  */
  YYSYMBOL_CREATE = 4,                     /* CREATE  */
  YYSYMBOL_DROP = 5,                       /* DROP  */
  YYSYMBOL_TABLE = 6,                      /* TABLE  */
  YYSYMBOL_TABLES = 7,                     /* TABLES  */
  YYSYMBOL_INDEX = 8,                      /* INDEX  */
  YYSYMBOL_SELECT = 9,                     /* SELECT  */
  YYSYMBOL_DESC = 10,                      /* DESC  */
  YYSYMBOL_SHOW = 11,                      /* SHOW  */
  YYSYMBOL_SYNC = 12,                      /* SYNC  */
  YYSYMBOL_INSERT = 13,                    /* INSERT  */
  YYSYMBOL_DELETE = 14,                    /* DELETE  */
  YYSYMBOL_UPDATE = 15,                    /* UPDATE  */
  YYSYMBOL_UNIQUE = 16,                    /* UNIQUE  */
  YYSYMBOL_LBRACE = 17,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 18,                    /* RBRACE  */
  YYSYMBOL_COMMA = 19,                     /* COMMA  */
  YYSYMBOL_TRX_BEGIN = 20,                 /* TRX_BEGIN  */
  YYSYMBOL_TRX_COMMIT = 21,                /* TRX_COMMIT  */
  YYSYMBOL_TRX_ROLLBACK = 22,              /* TRX_ROLLBACK  */
  YYSYMBOL_INT_T = 23,                     /* INT_T  */
  YYSYMBOL_STRING_T = 24,                  /* STRING_T  */
  YYSYMBOL_FLOAT_T = 25,                   /* FLOAT_T  */
  YYSYMBOL_DATE_T = 26,                    /* DATE_T  */
  YYSYMBOL_HELP = 27,                      /* HELP  */
  YYSYMBOL_EXIT = 28,                      /* EXIT  */
  YYSYMBOL_DOT = 29,                       /* DOT  */
  YYSYMBOL_INTO = 30,                      /* INTO  */
  YYSYMBOL_VALUES = 31,                    /* VALUES  */
  YYSYMBOL_FROM = 32,                      /* FROM  */
  YYSYMBOL_WHERE = 33,                     /* WHERE  */
  YYSYMBOL_AND = 34,                       /* AND  */
  YYSYMBOL_SET = 35,                       /* SET  */
  YYSYMBOL_ON = 36,                        /* ON  */
  YYSYMBOL_LOAD = 37,                      /* LOAD  */
  YYSYMBOL_DATA = 38,                      /* DATA  */
  YYSYMBOL_INFILE = 39,                    /* INFILE  */
  YYSYMBOL_EQ = 40,                        /* EQ  */
  YYSYMBOL_LT = 41,                        /* LT  */
  YYSYMBOL_GT = 42,                        /* GT  */
  YYSYMBOL_LE = 43,                        /* LE  */
  YYSYMBOL_GE = 44,                        /* GE  */
  YYSYMBOL_NE = 45,                        /* NE  */
  YYSYMBOL_MAX = 46,                       /* MAX  */
  YYSYMBOL_MIN = 47,                       /* MIN  */
  YYSYMBOL_COUNT = 48,                     /* COUNT  */
  YYSYMBOL_AVG = 49,                       /* AVG  */
  YYSYMBOL_NUMBER = 50,                    /* NUMBER  */
  YYSYMBOL_FLOAT = 51,                     /* FLOAT  */
  YYSYMBOL_ID = 52,                        /* ID  */
  YYSYMBOL_PATH = 53,                      /* PATH  */
  YYSYMBOL_SSS = 54,                       /* SSS  */
  YYSYMBOL_STAR = 55,                      /* STAR  */
  YYSYMBOL_STRING_V = 56,                  /* STRING_V  */
  YYSYMBOL_DATE = 57,                      /* DATE  */
  YYSYMBOL_YYACCEPT = 58,                  /* $accept  */
  YYSYMBOL_commands = 59,                  /* commands  */
  YYSYMBOL_command = 60,                   /* command  */
  YYSYMBOL_exit = 61,                      /* exit  */
  YYSYMBOL_help = 62,                      /* help  */
  YYSYMBOL_sync = 63,                      /* sync  */
  YYSYMBOL_begin = 64,                     /* begin  */
  YYSYMBOL_commit = 65,                    /* commit  */
  YYSYMBOL_rollback = 66,                  /* rollback  */
  YYSYMBOL_drop_table = 67,                /* drop_table  */
  YYSYMBOL_show_tables = 68,               /* show_tables  */
  YYSYMBOL_desc_table = 69,                /* desc_table  */
  YYSYMBOL_create_index = 70,              /* create_index  */
  YYSYMBOL_create_unique_index = 71,       /* create_unique_index  */
  YYSYMBOL_drop_index = 72,                /* drop_index  */
  YYSYMBOL_create_table = 73,              /* create_table  */
  YYSYMBOL_attr_def_list = 74,             /* attr_def_list  */
  YYSYMBOL_attr_def = 75,                  /* attr_def  */
  YYSYMBOL_number = 76,                    /* number  */
  YYSYMBOL_type = 77,                      /* type  */
  YYSYMBOL_ID_get = 78,                    /* ID_get  */
  YYSYMBOL_insert = 79,                    /* insert  */
  YYSYMBOL_tuple_list = 80,                /* tuple_list  */
  YYSYMBOL_tuple = 81,                     /* tuple  */
  YYSYMBOL_value_list = 82,                /* value_list  */
  YYSYMBOL_tuple_value = 83,               /* tuple_value  */
  YYSYMBOL_value = 84,                     /* value  */
  YYSYMBOL_delete = 85,                    /* delete  */
  YYSYMBOL_update = 86,                    /* update  */
  YYSYMBOL_select = 87,                    /* select  */
  YYSYMBOL_select_attr = 88,               /* select_attr  */
  YYSYMBOL_attr_list = 89,                 /* attr_list  */
  YYSYMBOL_rel_list = 90,                  /* rel_list  */
  YYSYMBOL_where = 91,                     /* where  */
  YYSYMBOL_condition_list = 92,            /* condition_list  */
  YYSYMBOL_condition = 93,                 /* condition  */
  YYSYMBOL_comOp = 94,                     /* comOp  */
  YYSYMBOL_load_data = 95                  /* load_data  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   253

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  58
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  38
/* YYNRULES -- Number of rules.  */
#define YYNRULES  107
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  267

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   312


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
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
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   144,   144,   146,   150,   151,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   171,   176,   181,   187,   193,   199,   205,   211,
     217,   224,   231,   238,   245,   254,   256,   260,   271,   285,
     288,   289,   290,   291,   294,   302,   313,   314,   318,   322,
     323,   327,   330,   333,   337,   344,   347,   350,   354,   361,
     371,   381,   400,   405,   410,   415,   420,   425,   430,   435,
     440,   445,   450,   455,   460,   466,   468,   475,   482,   487,
     492,   497,   502,   507,   512,   517,   522,   527,   534,   536,
     540,   542,   546,   548,   553,   574,   594,   614,   636,   657,
     678,   700,   701,   702,   703,   704,   705,   709
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "SEMICOLON", "CREATE",
  "DROP", "TABLE", "TABLES", "INDEX", "SELECT", "DESC", "SHOW", "SYNC",
  "INSERT", "DELETE", "UPDATE", "UNIQUE", "LBRACE", "RBRACE", "COMMA",
  "TRX_BEGIN", "TRX_COMMIT", "TRX_ROLLBACK", "INT_T", "STRING_T",
  "FLOAT_T", "DATE_T", "HELP", "EXIT", "DOT", "INTO", "VALUES", "FROM",
  "WHERE", "AND", "SET", "ON", "LOAD", "DATA", "INFILE", "EQ", "LT", "GT",
  "LE", "GE", "NE", "MAX", "MIN", "COUNT", "AVG", "NUMBER", "FLOAT", "ID",
  "PATH", "SSS", "STAR", "STRING_V", "DATE", "$accept", "commands",
  "command", "exit", "help", "sync", "begin", "commit", "rollback",
  "drop_table", "show_tables", "desc_table", "create_index",
  "create_unique_index", "drop_index", "create_table", "attr_def_list",
  "attr_def", "number", "type", "ID_get", "insert", "tuple_list", "tuple",
  "value_list", "tuple_value", "value", "delete", "update", "select",
  "select_attr", "attr_list", "rel_list", "where", "condition_list",
  "condition", "comOp", "load_data", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312
};
#endif

#define YYPACT_NINF (-135)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -135,    22,  -135,    56,     1,    30,   -46,     3,     8,    -1,
      -8,   -14,    58,    71,    80,    81,    86,     7,  -135,  -135,
    -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,
    -135,  -135,  -135,  -135,  -135,  -135,  -135,    40,    43,    72,
      44,    62,    74,    99,   100,   105,    39,  -135,    92,    91,
     122,  -135,    78,    83,   102,  -135,  -135,  -135,  -135,  -135,
     106,   121,   108,   110,   161,   162,   115,   116,    52,   117,
      94,   118,  -135,   119,  -135,  -135,   141,   140,   123,   120,
     124,   125,   142,  -135,  -135,   -13,   -10,   163,    -6,   164,
      -4,   166,   167,   168,   169,    46,   160,   170,   171,    36,
     177,   147,   165,  -135,   172,   130,   173,   144,   160,   145,
     160,   146,   160,   160,   148,   160,   160,   149,   150,   151,
     111,   152,   153,  -135,  -135,   154,   140,    16,   174,  -135,
    -135,    11,  -135,  -135,   107,   158,  -135,    82,   188,   124,
     181,  -135,  -135,  -135,  -135,   190,   156,   192,  -135,   193,
    -135,   194,  -135,  -135,   195,  -135,  -135,   196,    10,    12,
     197,    28,   198,    42,   160,   170,   207,  -135,  -135,  -135,
    -135,   199,   171,   214,   175,  -135,  -135,  -135,  -135,  -135,
    -135,    61,    69,    36,  -135,   140,   176,   172,   216,   179,
     202,   178,   160,   160,   160,   160,   160,   180,   160,   182,
     160,   160,   183,   160,   160,   184,  -135,  -135,  -135,    16,
     203,   174,  -135,   107,  -135,  -135,   204,  -135,   158,   219,
     220,  -135,  -135,  -135,   206,   222,   208,  -135,  -135,  -135,
    -135,  -135,   213,  -135,   221,  -135,  -135,   223,  -135,  -135,
     224,   199,  -135,  -135,    77,   185,  -135,  -135,  -135,  -135,
    -135,   235,   160,   160,   160,   160,  -135,   211,  -135,  -135,
    -135,  -135,  -135,  -135,  -135,   191,  -135
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     3,    21,
      20,    15,    16,    17,    18,     9,    10,    11,    12,    13,
      14,     8,     5,     7,     6,     4,    19,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    75,    62,     0,     0,
       0,    24,     0,     0,     0,    25,    26,    27,    23,    22,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    63,     0,    30,    29,     0,    90,     0,     0,
       0,     0,     0,    28,    33,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    75,    75,    88,     0,     0,
       0,     0,     0,    44,    35,     0,     0,     0,    75,     0,
      75,     0,    75,    75,     0,    75,    75,     0,     0,     0,
       0,     0,     0,    76,    64,     0,    90,     0,    46,    55,
      56,     0,    57,    58,     0,    92,    59,     0,     0,     0,
       0,    40,    41,    42,    43,    38,     0,     0,    65,     0,
      67,     0,    72,    70,     0,    69,    73,     0,     0,     0,
       0,     0,     0,     0,    75,    88,     0,    51,    52,    53,
      54,    49,     0,     0,     0,   101,   102,   103,   104,   105,
     106,     0,     0,     0,    91,    90,     0,    35,     0,     0,
       0,     0,    75,    75,    75,    75,    75,     0,    75,     0,
      75,    75,     0,    75,    75,     0,    77,    89,    61,     0,
       0,    46,    45,     0,    96,    94,    97,    95,    92,     0,
       0,    36,    34,    39,     0,     0,     0,    66,    68,    71,
      74,    78,     0,    80,     0,    85,    83,     0,    82,    86,
       0,    49,    48,    47,     0,     0,    93,    60,   107,    37,
      31,     0,    75,    75,    75,    75,    50,     0,    98,    99,
      32,    79,    81,    84,    87,     0,   100
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,
    -135,  -135,  -135,  -135,  -135,  -135,    57,   109,  -135,  -135,
    -135,  -135,    34,    75,     5,    41,  -134,  -135,  -135,  -135,
    -135,   -95,    84,  -122,    33,    70,  -132,  -135
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,   140,   104,   224,   145,
     105,    32,   173,   128,   210,   171,   134,    33,    34,    35,
      48,    72,   126,   100,   184,   135,   181,    36
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     123,   124,   182,   185,   166,   108,    49,    40,   110,    41,
      50,    51,   113,   148,   116,   150,   109,   152,   153,   111,
     155,   156,     2,   114,    53,   117,     3,     4,   196,    52,
     198,     5,     6,     7,     8,     9,    10,    11,    54,   197,
     174,   199,    12,    13,    14,    60,   201,   215,   217,    15,
      16,   175,   176,   177,   178,   179,   180,   202,    70,    17,
     204,    55,    37,   219,    38,    70,   167,   168,    71,   206,
     169,   205,    39,   170,    56,   122,    42,    43,    44,    45,
      63,   244,    46,    57,    58,    47,   129,   130,   131,    59,
     132,    66,    61,   133,    74,    62,    64,   227,   228,   229,
     230,   231,    87,   233,    88,   235,   236,    89,   238,   239,
     258,   129,   130,   214,    65,   132,    67,    68,   133,   129,
     130,   216,    69,   132,    73,    75,   133,   129,   130,   257,
      76,   132,   129,   130,   133,    77,   132,    78,    80,   133,
      91,    92,    93,    94,    81,    79,    95,   175,   176,   177,
     178,   179,   180,   141,   142,   143,   144,   261,   262,   263,
     264,   160,    82,   161,    83,    84,   162,    85,    86,    90,
      96,    97,    98,    99,   102,   101,   103,   106,   107,    70,
     136,   112,   115,   118,   119,   120,   121,   137,   127,   125,
     146,   139,   183,   172,   186,   138,   147,   149,   151,   188,
     154,   157,   158,   159,   163,   164,   165,   189,   190,   191,
     208,   192,   193,   194,   195,   200,   203,   212,   209,   222,
     225,   242,   247,   248,   249,   250,   251,   213,   220,   223,
     226,   252,   232,   245,   234,   237,   240,   259,   260,   253,
     265,   254,   255,   266,   221,   243,   256,   211,   187,   207,
     241,   246,     0,   218
};

static const yytype_int16 yycheck[] =
{
      95,    96,   134,   137,   126,    18,    52,     6,    18,     8,
       7,     3,    18,   108,    18,   110,    29,   112,   113,    29,
     115,   116,     0,    29,    32,    29,     4,     5,    18,    30,
      18,     9,    10,    11,    12,    13,    14,    15,    52,    29,
      29,    29,    20,    21,    22,    38,    18,   181,   182,    27,
      28,    40,    41,    42,    43,    44,    45,    29,    19,    37,
      18,     3,     6,   185,     8,    19,    50,    51,    29,   164,
      54,    29,    16,    57,     3,    29,    46,    47,    48,    49,
       8,   213,    52,     3,     3,    55,    50,    51,    52,     3,
      54,    17,    52,    57,     3,    52,    52,   192,   193,   194,
     195,   196,    50,   198,    52,   200,   201,    55,   203,   204,
     244,    50,    51,    52,    52,    54,    17,    17,    57,    50,
      51,    52,    17,    54,    32,     3,    57,    50,    51,    52,
      52,    54,    50,    51,    57,    52,    54,    35,    17,    57,
      46,    47,    48,    49,    36,    39,    52,    40,    41,    42,
      43,    44,    45,    23,    24,    25,    26,   252,   253,   254,
     255,    50,    52,    52,     3,     3,    55,    52,    52,    52,
      52,    52,    31,    33,    54,    52,    52,    52,    36,    19,
       3,    18,    18,    17,    17,    17,    17,    40,    17,    19,
      17,    19,    34,    19,     6,    30,    52,    52,    52,    18,
      52,    52,    52,    52,    52,    52,    52,    17,    52,    17,
       3,    18,    18,    18,    18,    18,    18,     3,    19,     3,
      18,    18,     3,     3,    18,     3,    18,    52,    52,    50,
      52,    18,    52,    29,    52,    52,    52,    52,     3,    18,
      29,    18,    18,    52,   187,   211,   241,   172,   139,   165,
     209,   218,    -1,   183
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    59,     0,     4,     5,     9,    10,    11,    12,    13,
      14,    15,    20,    21,    22,    27,    28,    37,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    79,    85,    86,    87,    95,     6,     8,    16,
       6,     8,    46,    47,    48,    49,    52,    55,    88,    52,
       7,     3,    30,    32,    52,     3,     3,     3,     3,     3,
      38,    52,    52,     8,    52,    52,    17,    17,    17,    17,
      19,    29,    89,    32,     3,     3,    52,    52,    35,    39,
      17,    36,    52,     3,     3,    52,    52,    50,    52,    55,
      52,    46,    47,    48,    49,    52,    52,    52,    31,    33,
      91,    52,    54,    52,    75,    78,    52,    36,    18,    29,
      18,    29,    18,    18,    29,    18,    18,    29,    17,    17,
      17,    17,    29,    89,    89,    19,    90,    17,    81,    50,
      51,    52,    54,    57,    84,    93,     3,    40,    30,    19,
      74,    23,    24,    25,    26,    77,    17,    52,    89,    52,
      89,    52,    89,    89,    52,    89,    89,    52,    52,    52,
      50,    52,    55,    52,    52,    52,    91,    50,    51,    54,
      57,    83,    19,    80,    29,    40,    41,    42,    43,    44,
      45,    94,    94,    34,    92,    84,     6,    75,    18,    17,
      52,    17,    18,    18,    18,    18,    18,    29,    18,    29,
      18,    18,    29,    18,    18,    29,    89,    90,     3,    19,
      82,    81,     3,    52,    52,    84,    52,    84,    93,    91,
      52,    74,     3,    50,    76,    18,    52,    89,    89,    89,
      89,    89,    52,    89,    52,    89,    89,    52,    89,    89,
      52,    83,    18,    80,    94,    29,    92,     3,     3,    18,
       3,    18,    18,    18,    18,    18,    82,    52,    84,    52,
       3,    89,    89,    89,    89,    29,    52
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    58,    59,    59,    60,    60,    60,    60,    60,    60,
      60,    60,    60,    60,    60,    60,    60,    60,    60,    60,
      60,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    74,    75,    75,    76,
      77,    77,    77,    77,    78,    79,    80,    80,    81,    82,
      82,    83,    83,    83,    83,    84,    84,    84,    84,    85,
      86,    87,    88,    88,    88,    88,    88,    88,    88,    88,
      88,    88,    88,    88,    88,    89,    89,    89,    89,    89,
      89,    89,    89,    89,    89,    89,    89,    89,    90,    90,
      91,    91,    92,    92,    93,    93,    93,    93,    93,    93,
      93,    94,    94,    94,    94,    94,    94,    95
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     2,     2,     2,     2,     4,     3,
       3,     9,    10,     4,     8,     0,     3,     5,     2,     1,
       1,     1,     1,     1,     1,     7,     0,     3,     4,     0,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     5,
       8,     7,     1,     2,     4,     5,     7,     5,     7,     5,
       5,     7,     5,     5,     7,     0,     3,     5,     6,     8,
       6,     8,     6,     6,     8,     6,     6,     8,     0,     3,
       0,     3,     0,     3,     3,     3,     3,     3,     5,     5,
       7,     1,     1,     1,     1,     1,     1,     8
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (scanner, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
# ifndef YY_LOCATION_PRINT
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, scanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, void *scanner)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  YYUSE (scanner);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yykind < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yykind], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, void *scanner)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep, scanner);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule, void *scanner)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)], scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, scanner); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
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






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, void *scanner)
{
  YYUSE (yyvaluep);
  YYUSE (scanner);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (void *scanner)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

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
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval, scanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
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
      if (yytable_value_is_error (yyn))
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
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 22: /* exit: EXIT SEMICOLON  */
#line 171 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_EXIT;//"exit";
    }
#line 1394 "yacc_sql.tab.c"
    break;

  case 23: /* help: HELP SEMICOLON  */
#line 176 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_HELP;//"help";
    }
#line 1402 "yacc_sql.tab.c"
    break;

  case 24: /* sync: SYNC SEMICOLON  */
#line 181 "yacc_sql.y"
                   {
      CONTEXT->ssql->flag = SCF_SYNC;
    }
#line 1410 "yacc_sql.tab.c"
    break;

  case 25: /* begin: TRX_BEGIN SEMICOLON  */
#line 187 "yacc_sql.y"
                        {
      CONTEXT->ssql->flag = SCF_BEGIN;
    }
#line 1418 "yacc_sql.tab.c"
    break;

  case 26: /* commit: TRX_COMMIT SEMICOLON  */
#line 193 "yacc_sql.y"
                         {
      CONTEXT->ssql->flag = SCF_COMMIT;
    }
#line 1426 "yacc_sql.tab.c"
    break;

  case 27: /* rollback: TRX_ROLLBACK SEMICOLON  */
#line 199 "yacc_sql.y"
                           {
      CONTEXT->ssql->flag = SCF_ROLLBACK;
    }
#line 1434 "yacc_sql.tab.c"
    break;

  case 28: /* drop_table: DROP TABLE ID SEMICOLON  */
#line 205 "yacc_sql.y"
                            {
        CONTEXT->ssql->flag = SCF_DROP_TABLE;//"drop_table";
				drop_table_init(&CONTEXT->ssql->sstr.drop_table, (yyvsp[-1].string));
    }
#line 1443 "yacc_sql.tab.c"
    break;

  case 29: /* show_tables: SHOW TABLES SEMICOLON  */
#line 211 "yacc_sql.y"
                          {
      CONTEXT->ssql->flag = SCF_SHOW_TABLES;
    }
#line 1451 "yacc_sql.tab.c"
    break;

  case 30: /* desc_table: DESC ID SEMICOLON  */
#line 217 "yacc_sql.y"
                      {
      CONTEXT->ssql->flag = SCF_DESC_TABLE;
			desc_table_init(&CONTEXT->ssql->sstr.desc_table, (yyvsp[-1].string));
    }
#line 1460 "yacc_sql.tab.c"
    break;

  case 31: /* create_index: CREATE INDEX ID ON ID LBRACE ID RBRACE SEMICOLON  */
#line 225 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, (yyvsp[-6].string), (yyvsp[-4].string), (yyvsp[-2].string));
		}
#line 1469 "yacc_sql.tab.c"
    break;

  case 32: /* create_unique_index: CREATE UNIQUE INDEX ID ON ID LBRACE ID RBRACE SEMICOLON  */
#line 232 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_CREATE_UNIQUE_INDEX;//"create_unique_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, (yyvsp[-6].string), (yyvsp[-4].string), (yyvsp[-2].string));
		}
#line 1478 "yacc_sql.tab.c"
    break;

  case 33: /* drop_index: DROP INDEX ID SEMICOLON  */
#line 239 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_DROP_INDEX;//"drop_index";
			drop_index_init(&CONTEXT->ssql->sstr.drop_index, (yyvsp[-1].string));
		}
#line 1487 "yacc_sql.tab.c"
    break;

  case 34: /* create_table: CREATE TABLE ID LBRACE attr_def attr_def_list RBRACE SEMICOLON  */
#line 246 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_CREATE_TABLE;//"create_table";
			// CONTEXT->ssql->sstr.create_table.attribute_count = CONTEXT->value_length; // TODO
			create_table_init_name(&CONTEXT->ssql->sstr.create_table, (yyvsp[-5].string));
			//临时变量清零	
			CONTEXT->value_length = 0;
		}
#line 1499 "yacc_sql.tab.c"
    break;

  case 36: /* attr_def_list: COMMA attr_def attr_def_list  */
#line 256 "yacc_sql.y"
                                   {    }
#line 1505 "yacc_sql.tab.c"
    break;

  case 37: /* attr_def: ID_get type LBRACE number RBRACE  */
#line 261 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-3].number), (yyvsp[-1].number));
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name =(char*)malloc(sizeof(char)); // TODO FATAL
			// strcpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type = $2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length = $4;
			CONTEXT->value_length++;
		}
#line 1520 "yacc_sql.tab.c"
    break;

  case 38: /* attr_def: ID_get type  */
#line 272 "yacc_sql.y"
                {
			AttrInfo attribute;
			// TODO 如果支持NULL，需要考虑如何用4字节表示或其他方式
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[0].number), 4);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name=(char*)malloc(sizeof(char));
			// strcpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type=$2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length=4; // default attribute length
			CONTEXT->value_length++;
		}
#line 1536 "yacc_sql.tab.c"
    break;

  case 39: /* number: NUMBER  */
#line 285 "yacc_sql.y"
                       {(yyval.number) = (yyvsp[0].number);}
#line 1542 "yacc_sql.tab.c"
    break;

  case 40: /* type: INT_T  */
#line 288 "yacc_sql.y"
              { (yyval.number)=INTS; }
#line 1548 "yacc_sql.tab.c"
    break;

  case 41: /* type: STRING_T  */
#line 289 "yacc_sql.y"
                  { (yyval.number)=CHARS; }
#line 1554 "yacc_sql.tab.c"
    break;

  case 42: /* type: FLOAT_T  */
#line 290 "yacc_sql.y"
                 { (yyval.number)=FLOATS; }
#line 1560 "yacc_sql.tab.c"
    break;

  case 43: /* type: DATE_T  */
#line 291 "yacc_sql.y"
                    { (yyval.number)=DATES; }
#line 1566 "yacc_sql.tab.c"
    break;

  case 44: /* ID_get: ID  */
#line 295 "yacc_sql.y"
        {
		char *temp=(yyvsp[0].string); 
		snprintf(CONTEXT->id, sizeof(CONTEXT->id), "%s", temp);
	}
#line 1575 "yacc_sql.tab.c"
    break;

  case 45: /* insert: INSERT INTO ID VALUES tuple tuple_list SEMICOLON  */
#line 303 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_INSERT;
			inserts_init(&CONTEXT->ssql->sstr.insertion, (yyvsp[-4].string), CONTEXT->tuples, CONTEXT->tuple_num, CONTEXT->tuple_length);
      		//临时变量清零
      		for (int i = 0; i < CONTEXT->tuple_num; i++) {
				CONTEXT->tuple_length[i] = 0;
			}
			CONTEXT->tuple_num = 0;
    }
#line 1589 "yacc_sql.tab.c"
    break;

  case 47: /* tuple_list: COMMA tuple tuple_list  */
#line 314 "yacc_sql.y"
                                 {
	}
#line 1596 "yacc_sql.tab.c"
    break;

  case 48: /* tuple: LBRACE tuple_value value_list RBRACE  */
#line 318 "yacc_sql.y"
                                             {
		CONTEXT->tuple_num++;
	}
#line 1604 "yacc_sql.tab.c"
    break;

  case 50: /* value_list: COMMA tuple_value value_list  */
#line 323 "yacc_sql.y"
                                    { 
	  	}
#line 1611 "yacc_sql.tab.c"
    break;

  case 51: /* tuple_value: NUMBER  */
#line 327 "yacc_sql.y"
           {
  			value_init_integer(&CONTEXT->tuples[CONTEXT->tuple_num][CONTEXT->tuple_length[CONTEXT->tuple_num]++], (yyvsp[0].number));
		}
#line 1619 "yacc_sql.tab.c"
    break;

  case 52: /* tuple_value: FLOAT  */
#line 330 "yacc_sql.y"
           {
  			value_init_float(&CONTEXT->tuples[CONTEXT->tuple_num][CONTEXT->tuple_length[CONTEXT->tuple_num]++], (yyvsp[0].floats));
		}
#line 1627 "yacc_sql.tab.c"
    break;

  case 53: /* tuple_value: SSS  */
#line 333 "yacc_sql.y"
         {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
  			value_init_string(&CONTEXT->tuples[CONTEXT->tuple_num][CONTEXT->tuple_length[CONTEXT->tuple_num]++], (yyvsp[0].string));
		}
#line 1636 "yacc_sql.tab.c"
    break;

  case 54: /* tuple_value: DATE  */
#line 337 "yacc_sql.y"
              {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
			value_init_date(&CONTEXT->tuples[CONTEXT->tuple_num][CONTEXT->tuple_length[CONTEXT->tuple_num]++], (yyvsp[0].string));
		}
#line 1645 "yacc_sql.tab.c"
    break;

  case 55: /* value: NUMBER  */
#line 344 "yacc_sql.y"
          {	
  		value_init_integer(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].number));
		}
#line 1653 "yacc_sql.tab.c"
    break;

  case 56: /* value: FLOAT  */
#line 347 "yacc_sql.y"
          {
  		value_init_float(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].floats));
		}
#line 1661 "yacc_sql.tab.c"
    break;

  case 57: /* value: SSS  */
#line 350 "yacc_sql.y"
         {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
  		value_init_string(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].string));
		}
#line 1670 "yacc_sql.tab.c"
    break;

  case 58: /* value: DATE  */
#line 354 "yacc_sql.y"
              {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
			value_init_date(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].string));
		}
#line 1679 "yacc_sql.tab.c"
    break;

  case 59: /* delete: DELETE FROM ID where SEMICOLON  */
#line 362 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_DELETE;//"delete";
			deletes_init_relation(&CONTEXT->ssql->sstr.deletion, (yyvsp[-2].string));
			deletes_set_conditions(&CONTEXT->ssql->sstr.deletion, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;	
    }
#line 1691 "yacc_sql.tab.c"
    break;

  case 60: /* update: UPDATE ID SET ID EQ value where SEMICOLON  */
#line 372 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_UPDATE;//"update";
			Value *value = &CONTEXT->values[0];
			updates_init(&CONTEXT->ssql->sstr.update, (yyvsp[-6].string), (yyvsp[-4].string), value, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;
		}
#line 1703 "yacc_sql.tab.c"
    break;

  case 61: /* select: SELECT select_attr FROM ID rel_list where SEMICOLON  */
#line 382 "yacc_sql.y"
                {
			// CONTEXT->ssql->sstr.selection.relations[CONTEXT->from_length++]=$4;
			selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-3].string));

			selects_append_conditions(&CONTEXT->ssql->sstr.selection, CONTEXT->conditions, CONTEXT->condition_length);

			CONTEXT->ssql->flag=SCF_SELECT;//"select";
			// CONTEXT->ssql->sstr.selection.attr_num = CONTEXT->select_length;

			//临时变量清零
			CONTEXT->condition_length=0;
			CONTEXT->from_length=0;
			CONTEXT->select_length=0;
			CONTEXT->value_length = 0;
	}
#line 1723 "yacc_sql.tab.c"
    break;

  case 62: /* select_attr: STAR  */
#line 400 "yacc_sql.y"
         {  
			RelAttr attr;
			relation_attr_init(&attr, NULL, "*");
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1733 "yacc_sql.tab.c"
    break;

  case 63: /* select_attr: ID attr_list  */
#line 405 "yacc_sql.y"
                   {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1743 "yacc_sql.tab.c"
    break;

  case 64: /* select_attr: ID DOT ID attr_list  */
#line 410 "yacc_sql.y"
                              {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1753 "yacc_sql.tab.c"
    break;

  case 65: /* select_attr: MAX LBRACE ID RBRACE attr_list  */
#line 415 "yacc_sql.y"
                                         {
			AggOp agg;
			aggregation_init_string(&agg, NULL, (yyvsp[-2].string), MAX_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 1763 "yacc_sql.tab.c"
    break;

  case 66: /* select_attr: MAX LBRACE ID DOT ID RBRACE attr_list  */
#line 420 "yacc_sql.y"
                                                {
			AggOp agg;
			aggregation_init_string(&agg, (yyvsp[-4].string), (yyvsp[-2].string), MAX_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 1773 "yacc_sql.tab.c"
    break;

  case 67: /* select_attr: MIN LBRACE ID RBRACE attr_list  */
#line 425 "yacc_sql.y"
                                         {
			AggOp agg;
			aggregation_init_string(&agg, NULL, (yyvsp[-2].string), MIN_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 1783 "yacc_sql.tab.c"
    break;

  case 68: /* select_attr: MIN LBRACE ID DOT ID RBRACE attr_list  */
#line 430 "yacc_sql.y"
                                                {
			AggOp agg;
			aggregation_init_string(&agg, (yyvsp[-4].string), (yyvsp[-2].string), MIN_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 1793 "yacc_sql.tab.c"
    break;

  case 69: /* select_attr: COUNT LBRACE STAR RBRACE attr_list  */
#line 435 "yacc_sql.y"
                                             {
			AggOp agg;
			aggregation_init_string(&agg, NULL, "*", COUNT_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 1803 "yacc_sql.tab.c"
    break;

  case 70: /* select_attr: COUNT LBRACE ID RBRACE attr_list  */
#line 440 "yacc_sql.y"
                                           {
			AggOp agg;
			aggregation_init_string(&agg, NULL, (yyvsp[-2].string), COUNT_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 1813 "yacc_sql.tab.c"
    break;

  case 71: /* select_attr: COUNT LBRACE ID DOT ID RBRACE attr_list  */
#line 445 "yacc_sql.y"
                                                  {
			AggOp agg;
			aggregation_init_string(&agg, (yyvsp[-4].string), (yyvsp[-2].string), COUNT_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 1823 "yacc_sql.tab.c"
    break;

  case 72: /* select_attr: COUNT LBRACE NUMBER RBRACE attr_list  */
#line 450 "yacc_sql.y"
                                               {  //  TODO optimize count(n)
			AggOp agg;
			aggregation_init_integer(&agg, NULL, (yyvsp[-2].number), COUNT_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 1833 "yacc_sql.tab.c"
    break;

  case 73: /* select_attr: AVG LBRACE ID RBRACE attr_list  */
#line 455 "yacc_sql.y"
                                         {
			AggOp agg;
			aggregation_init_string(&agg, NULL, (yyvsp[-2].string), AVG_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 1843 "yacc_sql.tab.c"
    break;

  case 74: /* select_attr: AVG LBRACE ID DOT ID RBRACE attr_list  */
#line 460 "yacc_sql.y"
                                                {
			AggOp agg;
			aggregation_init_string(&agg, (yyvsp[-4].string), (yyvsp[-2].string), AVG_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 1853 "yacc_sql.tab.c"
    break;

  case 76: /* attr_list: COMMA ID attr_list  */
#line 468 "yacc_sql.y"
                         {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
#line 1865 "yacc_sql.tab.c"
    break;

  case 77: /* attr_list: COMMA ID DOT ID attr_list  */
#line 475 "yacc_sql.y"
                                {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
  	  }
#line 1877 "yacc_sql.tab.c"
    break;

  case 78: /* attr_list: COMMA MAX LBRACE ID RBRACE attr_list  */
#line 482 "yacc_sql.y"
                                               {
			AggOp agg;
			aggregation_init_string(&agg, NULL, (yyvsp[-2].string), MAX_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 1887 "yacc_sql.tab.c"
    break;

  case 79: /* attr_list: COMMA MAX LBRACE ID DOT ID RBRACE attr_list  */
#line 487 "yacc_sql.y"
                                                      {
			AggOp agg;
			aggregation_init_string(&agg, (yyvsp[-4].string), (yyvsp[-2].string), MAX_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 1897 "yacc_sql.tab.c"
    break;

  case 80: /* attr_list: COMMA MIN LBRACE ID RBRACE attr_list  */
#line 492 "yacc_sql.y"
                                               {
			AggOp agg;
			aggregation_init_string(&agg, NULL, (yyvsp[-2].string), MIN_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 1907 "yacc_sql.tab.c"
    break;

  case 81: /* attr_list: COMMA MIN LBRACE ID DOT ID RBRACE attr_list  */
#line 497 "yacc_sql.y"
                                                      {
			AggOp agg;
			aggregation_init_string(&agg, (yyvsp[-4].string), (yyvsp[-2].string), MIN_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 1917 "yacc_sql.tab.c"
    break;

  case 82: /* attr_list: COMMA COUNT LBRACE STAR RBRACE attr_list  */
#line 502 "yacc_sql.y"
                                                   { // count(*) 忽略null记录
			AggOp agg;
			aggregation_init_string(&agg, NULL, "*", COUNT_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 1927 "yacc_sql.tab.c"
    break;

  case 83: /* attr_list: COMMA COUNT LBRACE ID RBRACE attr_list  */
#line 507 "yacc_sql.y"
                                                 {
			AggOp agg;
			aggregation_init_string(&agg, NULL, (yyvsp[-2].string), COUNT_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 1937 "yacc_sql.tab.c"
    break;

  case 84: /* attr_list: COMMA COUNT LBRACE ID DOT ID RBRACE attr_list  */
#line 512 "yacc_sql.y"
                                                        {
			AggOp agg;
			aggregation_init_string(&agg, (yyvsp[-4].string), (yyvsp[-2].string), COUNT_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 1947 "yacc_sql.tab.c"
    break;

  case 85: /* attr_list: COMMA COUNT LBRACE NUMBER RBRACE attr_list  */
#line 517 "yacc_sql.y"
                                                     {  //  TODO optimize count(n) // conut(n) 不忽略null记录
			AggOp agg;
			aggregation_init_integer(&agg, NULL, (yyvsp[-2].number), COUNT_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 1957 "yacc_sql.tab.c"
    break;

  case 86: /* attr_list: COMMA AVG LBRACE ID RBRACE attr_list  */
#line 522 "yacc_sql.y"
                                               {
			AggOp agg;
			aggregation_init_string(&agg, NULL, (yyvsp[-2].string), AVG_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 1967 "yacc_sql.tab.c"
    break;

  case 87: /* attr_list: COMMA AVG LBRACE ID DOT ID RBRACE attr_list  */
#line 527 "yacc_sql.y"
                                                      {
			AggOp agg;
			aggregation_init_string(&agg, (yyvsp[-4].string), (yyvsp[-2].string), AVG_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 1977 "yacc_sql.tab.c"
    break;

  case 89: /* rel_list: COMMA ID rel_list  */
#line 536 "yacc_sql.y"
                        {	
				selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-1].string));
		  }
#line 1985 "yacc_sql.tab.c"
    break;

  case 91: /* where: WHERE condition condition_list  */
#line 542 "yacc_sql.y"
                                     {	
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 1993 "yacc_sql.tab.c"
    break;

  case 93: /* condition_list: AND condition condition_list  */
#line 548 "yacc_sql.y"
                                   {
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 2001 "yacc_sql.tab.c"
    break;

  case 94: /* condition: ID comOp value  */
#line 554 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, (yyvsp[-2].string));

			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$ = ( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 1;
			// $$->left_attr.relation_name = NULL;
			// $$->left_attr.attribute_name= $1;
			// $$->comp = CONTEXT->comp;
			// $$->right_is_attr = 0;
			// $$->right_attr.relation_name = NULL;
			// $$->right_attr.attribute_name = NULL;
			// $$->right_value = *$3;

		}
#line 2026 "yacc_sql.tab.c"
    break;

  case 95: /* condition: value comOp value  */
#line 575 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 2];
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$ = ( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 0;
			// $$->left_attr.relation_name=NULL;
			// $$->left_attr.attribute_name=NULL;
			// $$->left_value = *$1;
			// $$->comp = CONTEXT->comp;
			// $$->right_is_attr = 0;
			// $$->right_attr.relation_name = NULL;
			// $$->right_attr.attribute_name = NULL;
			// $$->right_value = *$3;

		}
#line 2050 "yacc_sql.tab.c"
    break;

  case 96: /* condition: ID comOp ID  */
#line 595 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, (yyvsp[-2].string));
			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 1;
			// $$->left_attr.relation_name=NULL;
			// $$->left_attr.attribute_name=$1;
			// $$->comp = CONTEXT->comp;
			// $$->right_is_attr = 1;
			// $$->right_attr.relation_name=NULL;
			// $$->right_attr.attribute_name=$3;

		}
#line 2074 "yacc_sql.tab.c"
    break;

  case 97: /* condition: value comOp ID  */
#line 615 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];
			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;

			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 0;
			// $$->left_attr.relation_name=NULL;
			// $$->left_attr.attribute_name=NULL;
			// $$->left_value = *$1;
			// $$->comp=CONTEXT->comp;
			
			// $$->right_is_attr = 1;
			// $$->right_attr.relation_name=NULL;
			// $$->right_attr.attribute_name=$3;
		
		}
#line 2100 "yacc_sql.tab.c"
    break;

  case 98: /* condition: ID DOT ID comOp value  */
#line 637 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, (yyvsp[-4].string), (yyvsp[-2].string));
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;

			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 1;
			// $$->left_attr.relation_name=$1;
			// $$->left_attr.attribute_name=$3;
			// $$->comp=CONTEXT->comp;
			// $$->right_is_attr = 0;   //属性值
			// $$->right_attr.relation_name=NULL;
			// $$->right_attr.attribute_name=NULL;
			// $$->right_value =*$5;			
							
    }
#line 2125 "yacc_sql.tab.c"
    break;

  case 99: /* condition: value comOp ID DOT ID  */
#line 658 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];

			RelAttr right_attr;
			relation_attr_init(&right_attr, (yyvsp[-2].string), (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 0;//属性值
			// $$->left_attr.relation_name=NULL;
			// $$->left_attr.attribute_name=NULL;
			// $$->left_value = *$1;
			// $$->comp =CONTEXT->comp;
			// $$->right_is_attr = 1;//属性
			// $$->right_attr.relation_name = $3;
			// $$->right_attr.attribute_name = $5;
									
    }
#line 2150 "yacc_sql.tab.c"
    break;

  case 100: /* condition: ID DOT ID comOp ID DOT ID  */
#line 679 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, (yyvsp[-6].string), (yyvsp[-4].string));
			RelAttr right_attr;
			relation_attr_init(&right_attr, (yyvsp[-2].string), (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 1;		//属性
			// $$->left_attr.relation_name=$1;
			// $$->left_attr.attribute_name=$3;
			// $$->comp =CONTEXT->comp;
			// $$->right_is_attr = 1;		//属性
			// $$->right_attr.relation_name=$5;
			// $$->right_attr.attribute_name=$7;
    }
#line 2173 "yacc_sql.tab.c"
    break;

  case 101: /* comOp: EQ  */
#line 700 "yacc_sql.y"
             { CONTEXT->comp = EQUAL_TO; }
#line 2179 "yacc_sql.tab.c"
    break;

  case 102: /* comOp: LT  */
#line 701 "yacc_sql.y"
         { CONTEXT->comp = LESS_THAN; }
#line 2185 "yacc_sql.tab.c"
    break;

  case 103: /* comOp: GT  */
#line 702 "yacc_sql.y"
         { CONTEXT->comp = GREAT_THAN; }
#line 2191 "yacc_sql.tab.c"
    break;

  case 104: /* comOp: LE  */
#line 703 "yacc_sql.y"
         { CONTEXT->comp = LESS_EQUAL; }
#line 2197 "yacc_sql.tab.c"
    break;

  case 105: /* comOp: GE  */
#line 704 "yacc_sql.y"
         { CONTEXT->comp = GREAT_EQUAL; }
#line 2203 "yacc_sql.tab.c"
    break;

  case 106: /* comOp: NE  */
#line 705 "yacc_sql.y"
         { CONTEXT->comp = NOT_EQUAL; }
#line 2209 "yacc_sql.tab.c"
    break;

  case 107: /* load_data: LOAD DATA INFILE SSS INTO TABLE ID SEMICOLON  */
#line 710 "yacc_sql.y"
                {
		  CONTEXT->ssql->flag = SCF_LOAD_DATA;
			load_data_init(&CONTEXT->ssql->sstr.load_data, (yyvsp[-1].string), (yyvsp[-4].string));
		}
#line 2218 "yacc_sql.tab.c"
    break;


#line 2222 "yacc_sql.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (scanner, YY_("syntax error"));
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
                      yytoken, &yylval, scanner);
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
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
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, scanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

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


#if !defined yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (scanner, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;
#endif


/*-------------------------------------------------------.
| yyreturn -- parsing is finished, clean up and return.  |
`-------------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, scanner);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, scanner);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 715 "yacc_sql.y"

//_____________________________________________________________________
extern void scan_string(const char *str, yyscan_t scanner);

int sql_parse(const char *s, Query *sqls){
	ParserContext context;
	memset(&context, 0, sizeof(context));

	yyscan_t scanner;
	yylex_init_extra(&context, &scanner);
	context.ssql = sqls;
	scan_string(s, scanner);
	int result = yyparse(scanner);
	yylex_destroy(scanner);
	return result;
}
