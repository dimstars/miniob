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
  Value null_value;
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


#line 134 "yacc_sql.tab.c"

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
  YYSYMBOL_INNER = 50,                     /* INNER  */
  YYSYMBOL_JOIN = 51,                      /* JOIN  */
  YYSYMBOL_IS = 52,                        /* IS  */
  YYSYMBOL_NOT = 53,                       /* NOT  */
  YYSYMBOL_NULL_T = 54,                    /* NULL_T  */
  YYSYMBOL_NULLABLE = 55,                  /* NULLABLE  */
  YYSYMBOL_NUMBER = 56,                    /* NUMBER  */
  YYSYMBOL_FLOAT = 57,                     /* FLOAT  */
  YYSYMBOL_ID = 58,                        /* ID  */
  YYSYMBOL_PATH = 59,                      /* PATH  */
  YYSYMBOL_SSS = 60,                       /* SSS  */
  YYSYMBOL_STAR = 61,                      /* STAR  */
  YYSYMBOL_STRING_V = 62,                  /* STRING_V  */
  YYSYMBOL_DATE = 63,                      /* DATE  */
  YYSYMBOL_YYACCEPT = 64,                  /* $accept  */
  YYSYMBOL_commands = 65,                  /* commands  */
  YYSYMBOL_command = 66,                   /* command  */
  YYSYMBOL_exit = 67,                      /* exit  */
  YYSYMBOL_help = 68,                      /* help  */
  YYSYMBOL_sync = 69,                      /* sync  */
  YYSYMBOL_begin = 70,                     /* begin  */
  YYSYMBOL_commit = 71,                    /* commit  */
  YYSYMBOL_rollback = 72,                  /* rollback  */
  YYSYMBOL_drop_table = 73,                /* drop_table  */
  YYSYMBOL_show_tables = 74,               /* show_tables  */
  YYSYMBOL_desc_table = 75,                /* desc_table  */
  YYSYMBOL_create_index = 76,              /* create_index  */
  YYSYMBOL_create_unique_index = 77,       /* create_unique_index  */
  YYSYMBOL_drop_index = 78,                /* drop_index  */
  YYSYMBOL_create_table = 79,              /* create_table  */
  YYSYMBOL_attr_def_list = 80,             /* attr_def_list  */
  YYSYMBOL_attr_def = 81,                  /* attr_def  */
  YYSYMBOL_82_1 = 82,                      /* $@1  */
  YYSYMBOL_83_2 = 83,                      /* $@2  */
  YYSYMBOL_number = 84,                    /* number  */
  YYSYMBOL_type = 85,                      /* type  */
  YYSYMBOL_ID_get = 86,                    /* ID_get  */
  YYSYMBOL_insert = 87,                    /* insert  */
  YYSYMBOL_tuple_list = 88,                /* tuple_list  */
  YYSYMBOL_tuple = 89,                     /* tuple  */
  YYSYMBOL_value_list = 90,                /* value_list  */
  YYSYMBOL_tuple_value = 91,               /* tuple_value  */
  YYSYMBOL_value = 92,                     /* value  */
  YYSYMBOL_delete = 93,                    /* delete  */
  YYSYMBOL_update = 94,                    /* update  */
  YYSYMBOL_select = 95,                    /* select  */
  YYSYMBOL_join_list = 96,                 /* join_list  */
  YYSYMBOL_join_list_ = 97,                /* join_list_  */
  YYSYMBOL_inner_join = 98,                /* inner_join  */
  YYSYMBOL_select_attr = 99,               /* select_attr  */
  YYSYMBOL_attr_list = 100,                /* attr_list  */
  YYSYMBOL_rel_list = 101,                 /* rel_list  */
  YYSYMBOL_where = 102,                    /* where  */
  YYSYMBOL_condition_list = 103,           /* condition_list  */
  YYSYMBOL_condition = 104,                /* condition  */
  YYSYMBOL_comOp = 105,                    /* comOp  */
  YYSYMBOL_load_data = 106                 /* load_data  */
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
#define YYLAST   327

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  64
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  43
/* YYNRULES -- Number of rules.  */
#define YYNRULES  125
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  319

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   318


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
      55,    56,    57,    58,    59,    60,    61,    62,    63
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   151,   151,   153,   157,   158,   159,   160,   161,   162,
     163,   164,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   178,   183,   188,   194,   200,   206,   212,   218,
     224,   231,   238,   245,   252,   261,   263,   268,   279,   267,
     292,   304,   311,   320,   323,   324,   325,   326,   329,   337,
     348,   349,   353,   357,   358,   362,   365,   368,   372,   376,
     382,   385,   388,   392,   399,   409,   417,   428,   444,   461,
     465,   466,   471,   472,   476,   481,   486,   491,   496,   501,
     506,   511,   516,   521,   526,   531,   536,   541,   547,   549,
     554,   561,   568,   573,   578,   583,   588,   593,   598,   603,
     608,   613,   620,   622,   626,   628,   632,   634,   639,   660,
     680,   700,   722,   743,   764,   783,   795,   807,   819,   834,
     835,   836,   837,   838,   839,   843
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
  "LE", "GE", "NE", "MAX", "MIN", "COUNT", "AVG", "INNER", "JOIN", "IS",
  "NOT", "NULL_T", "NULLABLE", "NUMBER", "FLOAT", "ID", "PATH", "SSS",
  "STAR", "STRING_V", "DATE", "$accept", "commands", "command", "exit",
  "help", "sync", "begin", "commit", "rollback", "drop_table",
  "show_tables", "desc_table", "create_index", "create_unique_index",
  "drop_index", "create_table", "attr_def_list", "attr_def", "$@1", "$@2",
  "number", "type", "ID_get", "insert", "tuple_list", "tuple",
  "value_list", "tuple_value", "value", "delete", "update", "select",
  "join_list", "join_list_", "inner_join", "select_attr", "attr_list",
  "rel_list", "where", "condition_list", "condition", "comOp", "load_data", YY_NULLPTR
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
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318
};
#endif

#define YYPACT_NINF (-282)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -282,   122,  -282,    49,   147,     2,   -47,    10,    30,     8,
      20,     0,    59,    81,    86,   102,   106,    87,  -282,  -282,
    -282,  -282,  -282,  -282,  -282,  -282,  -282,  -282,  -282,  -282,
    -282,  -282,  -282,  -282,  -282,  -282,  -282,    61,    88,   144,
     109,   112,   152,   158,   163,   164,    71,  -282,   150,   187,
     189,  -282,   135,   143,   167,  -282,  -282,  -282,  -282,  -282,
     165,   186,   169,   148,   204,   205,   151,   153,     3,   154,
      33,   -21,  -282,   156,  -282,  -282,   179,   180,   157,   159,
     160,   162,   181,  -282,  -282,    -6,    12,   198,    14,   203,
      27,   206,   207,   208,   209,    73,   210,   210,    -4,   211,
     100,   219,   190,   197,  -282,   212,   115,   215,   175,   210,
     176,   210,   177,   210,   210,   178,   210,   210,   182,   183,
     184,    50,   185,    35,  -282,  -282,  -282,   188,   193,  -282,
     180,   191,   180,    47,   218,  -282,  -282,   -16,  -282,  -282,
     155,   213,  -282,    91,   232,   160,   221,  -282,  -282,  -282,
    -282,   -11,   192,   228,  -282,   230,  -282,   233,  -282,  -282,
     234,  -282,  -282,   235,    56,    65,   236,    68,   237,    69,
     210,   210,   238,  -282,   253,   222,   256,  -282,  -282,  -282,
    -282,  -282,   241,   211,   258,   214,  -282,  -282,  -282,  -282,
    -282,  -282,    76,   108,   116,   100,  -282,   180,   180,   216,
     212,   259,   217,   223,  -282,   245,   220,   210,   210,   210,
     210,   210,   224,   210,   225,   210,   210,   226,   210,   210,
     227,  -282,  -282,  -282,  -282,   100,  -282,    47,   246,   218,
    -282,    26,   239,  -282,  -282,  -282,   240,  -282,   213,   262,
     263,   264,  -282,  -282,  -282,   250,  -282,   267,   257,  -282,
    -282,  -282,  -282,  -282,   261,  -282,   268,  -282,  -282,   269,
    -282,  -282,   270,   213,   241,  -282,  -282,   124,   131,  -282,
     231,  -282,  -282,  -282,  -282,  -282,  -282,   273,   210,   210,
     210,   210,   111,  -282,   242,  -282,   251,  -282,  -282,   160,
    -282,  -282,  -282,  -282,  -282,  -282,   243,  -282,   244,   115,
     254,  -282,   274,   100,   217,   213,   276,   111,   247,  -282,
     249,  -282,   160,   115,   275,   217,   277,   252,  -282
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
       0,     0,     0,     0,     0,     0,    88,    74,     0,     0,
       0,    24,     0,     0,     0,    25,    26,    27,    23,    22,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    76,     0,    30,    29,     0,   104,     0,     0,
       0,     0,     0,    28,    33,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    88,    88,    88,   102,     0,
       0,     0,     0,     0,    48,    35,     0,     0,     0,    88,
       0,    88,     0,    88,    88,     0,    88,    88,     0,     0,
       0,     0,     0,     0,    90,    77,    75,     0,     0,    73,
     104,     0,   104,     0,    50,    60,    61,     0,    62,    63,
       0,   106,    64,     0,     0,     0,     0,    44,    45,    46,
      47,    40,     0,     0,    78,     0,    80,     0,    85,    83,
       0,    82,    86,     0,     0,     0,     0,     0,     0,     0,
      88,    88,   102,    72,     0,     0,     0,    59,    55,    56,
      57,    58,    53,     0,     0,     0,   119,   120,   121,   122,
     123,   124,     0,     0,     0,     0,   105,   104,   104,     0,
      35,     0,     0,     0,    42,     0,     0,    88,    88,    88,
      88,    88,     0,    88,     0,    88,    88,     0,    88,    88,
       0,    91,    89,   103,    68,     0,    67,     0,     0,    50,
      49,     0,     0,   115,   110,   108,   111,   109,   106,     0,
       0,     0,    36,    34,    43,     0,    41,     0,     0,    79,
      81,    84,    87,    92,     0,    94,     0,    99,    97,     0,
      96,   100,     0,   106,    53,    52,    51,     0,     0,   117,
       0,   107,    66,    65,   125,    37,    31,     0,    88,    88,
      88,    88,    70,    54,     0,   116,     0,   112,   113,     0,
      32,    93,    95,    98,   101,    69,     0,   118,     0,     0,
       0,   114,     0,     0,     0,   106,     0,    70,     0,    71,
       0,    38,     0,     0,     0,     0,     0,     0,    39
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -282,  -282,  -282,  -282,  -282,  -282,  -282,  -282,  -282,  -282,
    -282,  -282,  -282,  -282,  -282,  -282,    97,   126,  -282,  -282,
    -216,  -279,  -281,  -282,    52,   121,    34,    72,  -140,  -282,
    -282,  -282,  -282,    -2,   229,  -282,   -95,   134,  -125,  -228,
    -186,  -136,  -282
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,   146,   105,   289,   312,
     245,   151,   106,    32,   184,   134,   228,   182,   140,    33,
      34,    35,   130,   295,   296,    48,    72,   132,   101,   196,
     141,   193,    36
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     124,   125,   126,   198,   194,   174,   202,   176,   299,   238,
     271,    49,   109,   185,   154,   127,   156,    50,   158,   159,
     302,   161,   162,   110,   186,   187,   188,   189,   190,   191,
     111,   313,   114,    51,   314,   282,   192,    96,    52,   263,
      97,   112,   203,   115,   204,   117,   128,   129,    42,    43,
      44,    45,    53,   235,   237,    37,   118,    38,    54,    87,
      46,    88,    55,    47,    89,    39,   186,   187,   188,   189,
     190,   191,   239,   240,   211,   221,   222,   307,   267,    91,
      92,    93,    94,   213,    56,   212,   216,   219,   306,    57,
      70,    95,    70,   170,   214,   268,   171,   217,   220,   316,
      71,   177,   123,   178,   179,    58,   166,   180,   167,    59,
     181,   168,   249,   250,   251,   252,   253,   305,   255,    61,
     257,   258,     2,   260,   261,    60,     3,     4,   287,   232,
     233,     5,     6,     7,     8,     9,    10,    11,   147,   148,
     149,   150,    12,    13,    14,   197,    62,   135,   136,    15,
      16,   138,    63,    40,   139,    41,   135,   136,   137,    17,
     138,   128,   129,   139,   135,   136,   234,    64,   138,    66,
      65,   139,   135,   136,   236,    67,   138,   284,   285,   139,
      68,    69,    73,   291,   292,   293,   294,   135,   136,   286,
      74,   138,    75,    76,   139,   186,   187,   188,   189,   190,
     191,    77,    78,    80,    79,    81,    82,    83,    84,    85,
      99,    86,    90,   100,    98,   102,   113,   108,   104,   103,
     107,   116,   142,   119,   120,   121,   122,   144,   133,    70,
     143,   145,   152,   153,   155,   157,   160,   183,   199,   201,
     163,   164,   165,   169,   173,   206,   172,   195,   207,   175,
     205,   208,   209,   210,   215,   218,   224,   127,   225,   226,
     227,   230,   243,   247,   265,   272,   273,   274,   275,   270,
     276,   200,   231,   244,   241,   277,   290,   246,   248,   278,
     298,   266,   254,   256,   259,   262,   279,   280,   281,   288,
     303,   304,   315,   269,   308,   317,   297,   242,   283,   264,
     310,   300,   301,   311,   229,   309,   223,   318,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   131
};

static const yytype_int16 yycheck[] =
{
      95,    96,    97,   143,   140,   130,    17,   132,   289,   195,
     238,    58,    18,    29,   109,    19,   111,     7,   113,   114,
     299,   116,   117,    29,    40,    41,    42,    43,    44,    45,
      18,   312,    18,     3,   313,   263,    52,    58,    30,   225,
      61,    29,    53,    29,    55,    18,    50,    51,    46,    47,
      48,    49,    32,   193,   194,     6,    29,     8,    58,    56,
      58,    58,     3,    61,    61,    16,    40,    41,    42,    43,
      44,    45,   197,   198,    18,   170,   171,   305,    52,    46,
      47,    48,    49,    18,     3,    29,    18,    18,   304,     3,
      19,    58,    19,    58,    29,   231,    61,    29,    29,   315,
      29,    54,    29,    56,    57,     3,    56,    60,    58,     3,
      63,    61,   207,   208,   209,   210,   211,   303,   213,    58,
     215,   216,     0,   218,   219,    38,     4,     5,   268,    53,
      54,     9,    10,    11,    12,    13,    14,    15,    23,    24,
      25,    26,    20,    21,    22,    54,    58,    56,    57,    27,
      28,    60,     8,     6,    63,     8,    56,    57,    58,    37,
      60,    50,    51,    63,    56,    57,    58,    58,    60,    17,
      58,    63,    56,    57,    58,    17,    60,    53,    54,    63,
      17,    17,    32,   278,   279,   280,   281,    56,    57,    58,
       3,    60,     3,    58,    63,    40,    41,    42,    43,    44,
      45,    58,    35,    17,    39,    36,    58,     3,     3,    58,
      31,    58,    58,    33,    58,    58,    18,    36,    58,    60,
      58,    18,     3,    17,    17,    17,    17,    30,    17,    19,
      40,    19,    17,    58,    58,    58,    58,    19,     6,    18,
      58,    58,    58,    58,    51,    17,    58,    34,    18,    58,
      58,    18,    18,    18,    18,    18,     3,    19,    36,     3,
      19,     3,     3,    18,    18,     3,     3,     3,    18,    29,
       3,   145,    58,    56,    58,    18,     3,    54,    58,    18,
      29,   229,    58,    58,    58,    58,    18,    18,    18,    58,
      36,    17,    17,    54,    18,    18,    54,   200,   264,   227,
      53,    58,    58,    54,   183,   307,   172,    55,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    98
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    65,     0,     4,     5,     9,    10,    11,    12,    13,
      14,    15,    20,    21,    22,    27,    28,    37,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    87,    93,    94,    95,   106,     6,     8,    16,
       6,     8,    46,    47,    48,    49,    58,    61,    99,    58,
       7,     3,    30,    32,    58,     3,     3,     3,     3,     3,
      38,    58,    58,     8,    58,    58,    17,    17,    17,    17,
      19,    29,   100,    32,     3,     3,    58,    58,    35,    39,
      17,    36,    58,     3,     3,    58,    58,    56,    58,    61,
      58,    46,    47,    48,    49,    58,    58,    61,    58,    31,
      33,   102,    58,    60,    58,    81,    86,    58,    36,    18,
      29,    18,    29,    18,    18,    29,    18,    18,    29,    17,
      17,    17,    17,    29,   100,   100,   100,    19,    50,    51,
      96,    98,   101,    17,    89,    56,    57,    58,    60,    63,
      92,   104,     3,    40,    30,    19,    80,    23,    24,    25,
      26,    85,    17,    58,   100,    58,   100,    58,   100,   100,
      58,   100,   100,    58,    58,    58,    56,    58,    61,    58,
      58,    61,    58,    51,   102,    58,   102,    54,    56,    57,
      60,    63,    91,    19,    88,    29,    40,    41,    42,    43,
      44,    45,    52,   105,   105,    34,   103,    54,    92,     6,
      81,    18,    17,    53,    55,    58,    17,    18,    18,    18,
      18,    18,    29,    18,    29,    18,    18,    29,    18,    18,
      29,   100,   100,   101,     3,    36,     3,    19,    90,    89,
       3,    58,    53,    54,    58,    92,    58,    92,   104,   102,
     102,    58,    80,     3,    56,    84,    54,    18,    58,   100,
     100,   100,   100,   100,    58,   100,    58,   100,   100,    58,
     100,   100,    58,   104,    91,    18,    88,    52,   105,    54,
      29,   103,     3,     3,     3,    18,     3,    18,    18,    18,
      18,    18,   103,    90,    53,    54,    58,    92,    58,    82,
       3,   100,   100,   100,   100,    97,    98,    54,    29,    86,
      58,    58,    85,    36,    17,   104,    84,   103,    18,    97,
      53,    54,    83,    86,    85,    17,    84,    18,    55
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    64,    65,    65,    66,    66,    66,    66,    66,    66,
      66,    66,    66,    66,    66,    66,    66,    66,    66,    66,
      66,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    80,    82,    83,    81,
      81,    81,    81,    84,    85,    85,    85,    85,    86,    87,
      88,    88,    89,    90,    90,    91,    91,    91,    91,    91,
      92,    92,    92,    92,    93,    94,    94,    95,    95,    96,
      97,    97,    98,    98,    99,    99,    99,    99,    99,    99,
      99,    99,    99,    99,    99,    99,    99,    99,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   101,   101,   102,   102,   103,   103,   104,   104,
     104,   104,   104,   104,   104,   104,   104,   104,   104,   105,
     105,   105,   105,   105,   105,   106
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     2,     2,     2,     2,     4,     3,
       3,     9,    10,     4,     8,     0,     3,     0,     0,    20,
       2,     4,     3,     1,     1,     1,     1,     1,     1,     7,
       0,     3,     4,     0,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     5,     8,     8,     7,     7,     6,
       0,     6,     2,     1,     1,     4,     2,     4,     5,     7,
       5,     7,     5,     5,     7,     5,     5,     7,     0,     5,
       3,     5,     6,     8,     6,     8,     6,     6,     8,     6,
       6,     8,     0,     3,     0,     3,     0,     3,     3,     3,
       3,     3,     5,     5,     7,     3,     5,     4,     6,     1,
       1,     1,     1,     1,     1,     8
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
#line 178 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_EXIT;//"exit";
    }
#line 1445 "yacc_sql.tab.c"
    break;

  case 23: /* help: HELP SEMICOLON  */
#line 183 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_HELP;//"help";
    }
#line 1453 "yacc_sql.tab.c"
    break;

  case 24: /* sync: SYNC SEMICOLON  */
#line 188 "yacc_sql.y"
                   {
      CONTEXT->ssql->flag = SCF_SYNC;
    }
#line 1461 "yacc_sql.tab.c"
    break;

  case 25: /* begin: TRX_BEGIN SEMICOLON  */
#line 194 "yacc_sql.y"
                        {
      CONTEXT->ssql->flag = SCF_BEGIN;
    }
#line 1469 "yacc_sql.tab.c"
    break;

  case 26: /* commit: TRX_COMMIT SEMICOLON  */
#line 200 "yacc_sql.y"
                         {
      CONTEXT->ssql->flag = SCF_COMMIT;
    }
#line 1477 "yacc_sql.tab.c"
    break;

  case 27: /* rollback: TRX_ROLLBACK SEMICOLON  */
#line 206 "yacc_sql.y"
                           {
      CONTEXT->ssql->flag = SCF_ROLLBACK;
    }
#line 1485 "yacc_sql.tab.c"
    break;

  case 28: /* drop_table: DROP TABLE ID SEMICOLON  */
#line 212 "yacc_sql.y"
                            {
        CONTEXT->ssql->flag = SCF_DROP_TABLE;//"drop_table";
				drop_table_init(&CONTEXT->ssql->sstr.drop_table, (yyvsp[-1].string));
    }
#line 1494 "yacc_sql.tab.c"
    break;

  case 29: /* show_tables: SHOW TABLES SEMICOLON  */
#line 218 "yacc_sql.y"
                          {
      CONTEXT->ssql->flag = SCF_SHOW_TABLES;
    }
#line 1502 "yacc_sql.tab.c"
    break;

  case 30: /* desc_table: DESC ID SEMICOLON  */
#line 224 "yacc_sql.y"
                      {
      CONTEXT->ssql->flag = SCF_DESC_TABLE;
			desc_table_init(&CONTEXT->ssql->sstr.desc_table, (yyvsp[-1].string));
    }
#line 1511 "yacc_sql.tab.c"
    break;

  case 31: /* create_index: CREATE INDEX ID ON ID LBRACE ID RBRACE SEMICOLON  */
#line 232 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, (yyvsp[-6].string), (yyvsp[-4].string), (yyvsp[-2].string));
		}
#line 1520 "yacc_sql.tab.c"
    break;

  case 32: /* create_unique_index: CREATE UNIQUE INDEX ID ON ID LBRACE ID RBRACE SEMICOLON  */
#line 239 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_CREATE_UNIQUE_INDEX;//"create_unique_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, (yyvsp[-6].string), (yyvsp[-4].string), (yyvsp[-2].string));
		}
#line 1529 "yacc_sql.tab.c"
    break;

  case 33: /* drop_index: DROP INDEX ID SEMICOLON  */
#line 246 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_DROP_INDEX;//"drop_index";
			drop_index_init(&CONTEXT->ssql->sstr.drop_index, (yyvsp[-1].string));
		}
#line 1538 "yacc_sql.tab.c"
    break;

  case 34: /* create_table: CREATE TABLE ID LBRACE attr_def attr_def_list RBRACE SEMICOLON  */
#line 253 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_CREATE_TABLE;//"create_table";
			// CONTEXT->ssql->sstr.create_table.attribute_count = CONTEXT->value_length; // TODO
			create_table_init_name(&CONTEXT->ssql->sstr.create_table, (yyvsp[-5].string));
			//临时变量清零	
			CONTEXT->value_length = 0;
		}
#line 1550 "yacc_sql.tab.c"
    break;

  case 36: /* attr_def_list: COMMA attr_def attr_def_list  */
#line 263 "yacc_sql.y"
                                   {    }
#line 1556 "yacc_sql.tab.c"
    break;

  case 37: /* $@1: %empty  */
#line 268 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-3].number), (yyvsp[-1].number), 1);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name =(char*)malloc(sizeof(char)); // TODO FATAL
			// strcpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type = $2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length = $4;
			CONTEXT->value_length++;
		}
#line 1571 "yacc_sql.tab.c"
    break;

  case 38: /* $@2: %empty  */
#line 279 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-11].number), (yyvsp[-9].number), 0);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
		}
#line 1582 "yacc_sql.tab.c"
    break;

  case 39: /* attr_def: ID_get type LBRACE number RBRACE $@1 ID_get type LBRACE number RBRACE NOT NULL_T $@2 ID_get type LBRACE number RBRACE NULLABLE  */
#line 286 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-18].number), (yyvsp[-16].number), 1);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
		}
#line 1593 "yacc_sql.tab.c"
    break;

  case 40: /* attr_def: ID_get type  */
#line 293 "yacc_sql.y"
                {
			AttrInfo attribute;
			// TODO 如果支持NULL，需要考虑如何用4字节表示或其他方式
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[0].number), 4, 1);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name=(char*)malloc(sizeof(char));
			// strcpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type=$2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length=4; // default attribute length
			CONTEXT->value_length++;
		}
#line 1609 "yacc_sql.tab.c"
    break;

  case 41: /* attr_def: ID_get type NOT NULL_T  */
#line 305 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-2].number), 4, 0);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
		}
#line 1620 "yacc_sql.tab.c"
    break;

  case 42: /* attr_def: ID_get type NULLABLE  */
#line 312 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-1].number), 4, 1);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
		}
#line 1631 "yacc_sql.tab.c"
    break;

  case 43: /* number: NUMBER  */
#line 320 "yacc_sql.y"
                       {(yyval.number) = (yyvsp[0].number);}
#line 1637 "yacc_sql.tab.c"
    break;

  case 44: /* type: INT_T  */
#line 323 "yacc_sql.y"
              { (yyval.number)=INTS; }
#line 1643 "yacc_sql.tab.c"
    break;

  case 45: /* type: STRING_T  */
#line 324 "yacc_sql.y"
                  { (yyval.number)=CHARS; }
#line 1649 "yacc_sql.tab.c"
    break;

  case 46: /* type: FLOAT_T  */
#line 325 "yacc_sql.y"
                 { (yyval.number)=FLOATS; }
#line 1655 "yacc_sql.tab.c"
    break;

  case 47: /* type: DATE_T  */
#line 326 "yacc_sql.y"
                    { (yyval.number)=DATES; }
#line 1661 "yacc_sql.tab.c"
    break;

  case 48: /* ID_get: ID  */
#line 330 "yacc_sql.y"
        {
		char *temp=(yyvsp[0].string); 
		snprintf(CONTEXT->id, sizeof(CONTEXT->id), "%s", temp);
	}
#line 1670 "yacc_sql.tab.c"
    break;

  case 49: /* insert: INSERT INTO ID VALUES tuple tuple_list SEMICOLON  */
#line 338 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_INSERT;
			inserts_init(&CONTEXT->ssql->sstr.insertion, (yyvsp[-4].string), CONTEXT->tuples, CONTEXT->tuple_num, CONTEXT->tuple_length);
      		//临时变量清零
      		for (int i = 0; i < CONTEXT->tuple_num; i++) {
				CONTEXT->tuple_length[i] = 0;
			}
			CONTEXT->tuple_num = 0;
    }
#line 1684 "yacc_sql.tab.c"
    break;

  case 51: /* tuple_list: COMMA tuple tuple_list  */
#line 349 "yacc_sql.y"
                                 {
	}
#line 1691 "yacc_sql.tab.c"
    break;

  case 52: /* tuple: LBRACE tuple_value value_list RBRACE  */
#line 353 "yacc_sql.y"
                                             {
		CONTEXT->tuple_num++;
	}
#line 1699 "yacc_sql.tab.c"
    break;

  case 54: /* value_list: COMMA tuple_value value_list  */
#line 358 "yacc_sql.y"
                                    { 
	  	}
#line 1706 "yacc_sql.tab.c"
    break;

  case 55: /* tuple_value: NUMBER  */
#line 362 "yacc_sql.y"
           {
  			value_init_integer(&CONTEXT->tuples[CONTEXT->tuple_num][CONTEXT->tuple_length[CONTEXT->tuple_num]++], (yyvsp[0].number));
		}
#line 1714 "yacc_sql.tab.c"
    break;

  case 56: /* tuple_value: FLOAT  */
#line 365 "yacc_sql.y"
           {
  			value_init_float(&CONTEXT->tuples[CONTEXT->tuple_num][CONTEXT->tuple_length[CONTEXT->tuple_num]++], (yyvsp[0].floats));
		}
#line 1722 "yacc_sql.tab.c"
    break;

  case 57: /* tuple_value: SSS  */
#line 368 "yacc_sql.y"
         {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
  			value_init_string(&CONTEXT->tuples[CONTEXT->tuple_num][CONTEXT->tuple_length[CONTEXT->tuple_num]++], (yyvsp[0].string));
		}
#line 1731 "yacc_sql.tab.c"
    break;

  case 58: /* tuple_value: DATE  */
#line 372 "yacc_sql.y"
              {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
			value_init_date(&CONTEXT->tuples[CONTEXT->tuple_num][CONTEXT->tuple_length[CONTEXT->tuple_num]++], (yyvsp[0].string));
		}
#line 1740 "yacc_sql.tab.c"
    break;

  case 59: /* tuple_value: NULL_T  */
#line 376 "yacc_sql.y"
                {
			value_init_null(&CONTEXT->tuples[CONTEXT->tuple_num][CONTEXT->tuple_length[CONTEXT->tuple_num]++]);
		}
#line 1748 "yacc_sql.tab.c"
    break;

  case 60: /* value: NUMBER  */
#line 382 "yacc_sql.y"
          {	
  		value_init_integer(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].number));
		}
#line 1756 "yacc_sql.tab.c"
    break;

  case 61: /* value: FLOAT  */
#line 385 "yacc_sql.y"
          {
  		value_init_float(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].floats));
		}
#line 1764 "yacc_sql.tab.c"
    break;

  case 62: /* value: SSS  */
#line 388 "yacc_sql.y"
         {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
  		value_init_string(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].string));
		}
#line 1773 "yacc_sql.tab.c"
    break;

  case 63: /* value: DATE  */
#line 392 "yacc_sql.y"
              {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
			value_init_date(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].string));
		}
#line 1782 "yacc_sql.tab.c"
    break;

  case 64: /* delete: DELETE FROM ID where SEMICOLON  */
#line 400 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_DELETE;//"delete";
			deletes_init_relation(&CONTEXT->ssql->sstr.deletion, (yyvsp[-2].string));
			deletes_set_conditions(&CONTEXT->ssql->sstr.deletion, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;	
    }
#line 1794 "yacc_sql.tab.c"
    break;

  case 65: /* update: UPDATE ID SET ID EQ value where SEMICOLON  */
#line 410 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_UPDATE;//"update";
			Value *value = &CONTEXT->values[0];
			updates_init(&CONTEXT->ssql->sstr.update, (yyvsp[-6].string), (yyvsp[-4].string), value, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;
		}
#line 1806 "yacc_sql.tab.c"
    break;

  case 66: /* update: UPDATE ID SET ID EQ NULL_T where SEMICOLON  */
#line 418 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_UPDATE;//"update";
			Value *value = &CONTEXT->null_value;
			value_init_null(value);
			updates_init(&CONTEXT->ssql->sstr.update, (yyvsp[-6].string), (yyvsp[-4].string), value, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;
		}
#line 1819 "yacc_sql.tab.c"
    break;

  case 67: /* select: SELECT select_attr FROM ID rel_list where SEMICOLON  */
#line 429 "yacc_sql.y"
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
#line 1839 "yacc_sql.tab.c"
    break;

  case 68: /* select: SELECT select_attr FROM ID join_list where SEMICOLON  */
#line 444 "yacc_sql.y"
                                                               {
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
#line 1858 "yacc_sql.tab.c"
    break;

  case 69: /* join_list: inner_join ID ON condition condition_list join_list_  */
#line 461 "yacc_sql.y"
                                                             {
		selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-4].string));
	}
#line 1866 "yacc_sql.tab.c"
    break;

  case 71: /* join_list_: inner_join ID ON condition condition_list join_list_  */
#line 466 "yacc_sql.y"
                                                               {
		selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-4].string));
	}
#line 1874 "yacc_sql.tab.c"
    break;

  case 72: /* inner_join: INNER JOIN  */
#line 471 "yacc_sql.y"
                   {}
#line 1880 "yacc_sql.tab.c"
    break;

  case 73: /* inner_join: JOIN  */
#line 472 "yacc_sql.y"
               {}
#line 1886 "yacc_sql.tab.c"
    break;

  case 74: /* select_attr: STAR  */
#line 476 "yacc_sql.y"
         {  
			RelAttr attr;
			relation_attr_init(&attr, NULL, "*");
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1896 "yacc_sql.tab.c"
    break;

  case 75: /* select_attr: ID DOT STAR attr_list  */
#line 481 "yacc_sql.y"
                                {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), "*");
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1906 "yacc_sql.tab.c"
    break;

  case 76: /* select_attr: ID attr_list  */
#line 486 "yacc_sql.y"
                   {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1916 "yacc_sql.tab.c"
    break;

  case 77: /* select_attr: ID DOT ID attr_list  */
#line 491 "yacc_sql.y"
                              {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1926 "yacc_sql.tab.c"
    break;

  case 78: /* select_attr: MAX LBRACE ID RBRACE attr_list  */
#line 496 "yacc_sql.y"
                                         {
			AggOp agg;
			aggregation_init_string(&agg, NULL, (yyvsp[-2].string), MAX_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 1936 "yacc_sql.tab.c"
    break;

  case 79: /* select_attr: MAX LBRACE ID DOT ID RBRACE attr_list  */
#line 501 "yacc_sql.y"
                                                {
			AggOp agg;
			aggregation_init_string(&agg, (yyvsp[-4].string), (yyvsp[-2].string), MAX_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 1946 "yacc_sql.tab.c"
    break;

  case 80: /* select_attr: MIN LBRACE ID RBRACE attr_list  */
#line 506 "yacc_sql.y"
                                         {
			AggOp agg;
			aggregation_init_string(&agg, NULL, (yyvsp[-2].string), MIN_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 1956 "yacc_sql.tab.c"
    break;

  case 81: /* select_attr: MIN LBRACE ID DOT ID RBRACE attr_list  */
#line 511 "yacc_sql.y"
                                                {
			AggOp agg;
			aggregation_init_string(&agg, (yyvsp[-4].string), (yyvsp[-2].string), MIN_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 1966 "yacc_sql.tab.c"
    break;

  case 82: /* select_attr: COUNT LBRACE STAR RBRACE attr_list  */
#line 516 "yacc_sql.y"
                                             {
			AggOp agg;
			aggregation_init_string(&agg, NULL, "*", COUNT_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 1976 "yacc_sql.tab.c"
    break;

  case 83: /* select_attr: COUNT LBRACE ID RBRACE attr_list  */
#line 521 "yacc_sql.y"
                                           {
			AggOp agg;
			aggregation_init_string(&agg, NULL, (yyvsp[-2].string), COUNT_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 1986 "yacc_sql.tab.c"
    break;

  case 84: /* select_attr: COUNT LBRACE ID DOT ID RBRACE attr_list  */
#line 526 "yacc_sql.y"
                                                  {
			AggOp agg;
			aggregation_init_string(&agg, (yyvsp[-4].string), (yyvsp[-2].string), COUNT_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 1996 "yacc_sql.tab.c"
    break;

  case 85: /* select_attr: COUNT LBRACE NUMBER RBRACE attr_list  */
#line 531 "yacc_sql.y"
                                               {  //  TODO optimize count(n)
			AggOp agg;
			aggregation_init_integer(&agg, NULL, (yyvsp[-2].number), COUNT_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 2006 "yacc_sql.tab.c"
    break;

  case 86: /* select_attr: AVG LBRACE ID RBRACE attr_list  */
#line 536 "yacc_sql.y"
                                         {
			AggOp agg;
			aggregation_init_string(&agg, NULL, (yyvsp[-2].string), AVG_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 2016 "yacc_sql.tab.c"
    break;

  case 87: /* select_attr: AVG LBRACE ID DOT ID RBRACE attr_list  */
#line 541 "yacc_sql.y"
                                                {
			AggOp agg;
			aggregation_init_string(&agg, (yyvsp[-4].string), (yyvsp[-2].string), AVG_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 2026 "yacc_sql.tab.c"
    break;

  case 89: /* attr_list: COMMA ID DOT STAR attr_list  */
#line 549 "yacc_sql.y"
                                      {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), "*");
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 2036 "yacc_sql.tab.c"
    break;

  case 90: /* attr_list: COMMA ID attr_list  */
#line 554 "yacc_sql.y"
                         {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
#line 2048 "yacc_sql.tab.c"
    break;

  case 91: /* attr_list: COMMA ID DOT ID attr_list  */
#line 561 "yacc_sql.y"
                                {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
  	  }
#line 2060 "yacc_sql.tab.c"
    break;

  case 92: /* attr_list: COMMA MAX LBRACE ID RBRACE attr_list  */
#line 568 "yacc_sql.y"
                                               {
			AggOp agg;
			aggregation_init_string(&agg, NULL, (yyvsp[-2].string), MAX_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 2070 "yacc_sql.tab.c"
    break;

  case 93: /* attr_list: COMMA MAX LBRACE ID DOT ID RBRACE attr_list  */
#line 573 "yacc_sql.y"
                                                      {
			AggOp agg;
			aggregation_init_string(&agg, (yyvsp[-4].string), (yyvsp[-2].string), MAX_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 2080 "yacc_sql.tab.c"
    break;

  case 94: /* attr_list: COMMA MIN LBRACE ID RBRACE attr_list  */
#line 578 "yacc_sql.y"
                                               {
			AggOp agg;
			aggregation_init_string(&agg, NULL, (yyvsp[-2].string), MIN_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 2090 "yacc_sql.tab.c"
    break;

  case 95: /* attr_list: COMMA MIN LBRACE ID DOT ID RBRACE attr_list  */
#line 583 "yacc_sql.y"
                                                      {
			AggOp agg;
			aggregation_init_string(&agg, (yyvsp[-4].string), (yyvsp[-2].string), MIN_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 2100 "yacc_sql.tab.c"
    break;

  case 96: /* attr_list: COMMA COUNT LBRACE STAR RBRACE attr_list  */
#line 588 "yacc_sql.y"
                                                   { // count(*) 忽略null记录
			AggOp agg;
			aggregation_init_string(&agg, NULL, "*", COUNT_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 2110 "yacc_sql.tab.c"
    break;

  case 97: /* attr_list: COMMA COUNT LBRACE ID RBRACE attr_list  */
#line 593 "yacc_sql.y"
                                                 {
			AggOp agg;
			aggregation_init_string(&agg, NULL, (yyvsp[-2].string), COUNT_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 2120 "yacc_sql.tab.c"
    break;

  case 98: /* attr_list: COMMA COUNT LBRACE ID DOT ID RBRACE attr_list  */
#line 598 "yacc_sql.y"
                                                        {
			AggOp agg;
			aggregation_init_string(&agg, (yyvsp[-4].string), (yyvsp[-2].string), COUNT_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 2130 "yacc_sql.tab.c"
    break;

  case 99: /* attr_list: COMMA COUNT LBRACE NUMBER RBRACE attr_list  */
#line 603 "yacc_sql.y"
                                                     {  //  TODO optimize count(n) // conut(n) 不忽略null记录
			AggOp agg;
			aggregation_init_integer(&agg, NULL, (yyvsp[-2].number), COUNT_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 2140 "yacc_sql.tab.c"
    break;

  case 100: /* attr_list: COMMA AVG LBRACE ID RBRACE attr_list  */
#line 608 "yacc_sql.y"
                                               {
			AggOp agg;
			aggregation_init_string(&agg, NULL, (yyvsp[-2].string), AVG_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 2150 "yacc_sql.tab.c"
    break;

  case 101: /* attr_list: COMMA AVG LBRACE ID DOT ID RBRACE attr_list  */
#line 613 "yacc_sql.y"
                                                      {
			AggOp agg;
			aggregation_init_string(&agg, (yyvsp[-4].string), (yyvsp[-2].string), AVG_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
		}
#line 2160 "yacc_sql.tab.c"
    break;

  case 103: /* rel_list: COMMA ID rel_list  */
#line 622 "yacc_sql.y"
                        {	
				selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-1].string));
		  }
#line 2168 "yacc_sql.tab.c"
    break;

  case 105: /* where: WHERE condition condition_list  */
#line 628 "yacc_sql.y"
                                     {	
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 2176 "yacc_sql.tab.c"
    break;

  case 107: /* condition_list: AND condition condition_list  */
#line 634 "yacc_sql.y"
                                   {
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 2184 "yacc_sql.tab.c"
    break;

  case 108: /* condition: ID comOp value  */
#line 640 "yacc_sql.y"
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
#line 2209 "yacc_sql.tab.c"
    break;

  case 109: /* condition: value comOp value  */
#line 661 "yacc_sql.y"
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
#line 2233 "yacc_sql.tab.c"
    break;

  case 110: /* condition: ID comOp ID  */
#line 681 "yacc_sql.y"
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
#line 2257 "yacc_sql.tab.c"
    break;

  case 111: /* condition: value comOp ID  */
#line 701 "yacc_sql.y"
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
#line 2283 "yacc_sql.tab.c"
    break;

  case 112: /* condition: ID DOT ID comOp value  */
#line 723 "yacc_sql.y"
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
#line 2308 "yacc_sql.tab.c"
    break;

  case 113: /* condition: value comOp ID DOT ID  */
#line 744 "yacc_sql.y"
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
#line 2333 "yacc_sql.tab.c"
    break;

  case 114: /* condition: ID DOT ID comOp ID DOT ID  */
#line 765 "yacc_sql.y"
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
#line 2356 "yacc_sql.tab.c"
    break;

  case 115: /* condition: ID IS NULL_T  */
#line 784 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, (yyvsp[-2].string));
			
			Value *right_value = &CONTEXT->null_value;
			value_init_null(right_value);

			Condition condition;
			condition_init(&condition, EQUAL_TO, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		}
#line 2372 "yacc_sql.tab.c"
    break;

  case 116: /* condition: ID DOT ID IS NULL_T  */
#line 796 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, (yyvsp[-4].string), (yyvsp[-2].string));

			Value *right_value = &CONTEXT->null_value;;
			value_init_null(right_value);

			Condition condition;
			condition_init(&condition, EQUAL_TO, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		}
#line 2388 "yacc_sql.tab.c"
    break;

  case 117: /* condition: ID IS NOT NULL_T  */
#line 808 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, (yyvsp[-3].string));
			
			Value *right_value = &CONTEXT->null_value;;
			value_init_null(right_value);

			Condition condition;
			condition_init(&condition, NOT_EQUAL, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		}
#line 2404 "yacc_sql.tab.c"
    break;

  case 118: /* condition: ID DOT ID IS NOT NULL_T  */
#line 820 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, (yyvsp[-5].string), (yyvsp[-3].string));

			Value *right_value = &CONTEXT->null_value;;
			value_init_null(right_value);

			Condition condition;
			condition_init(&condition, NOT_EQUAL, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		}
#line 2420 "yacc_sql.tab.c"
    break;

  case 119: /* comOp: EQ  */
#line 834 "yacc_sql.y"
             { CONTEXT->comp = EQUAL_TO; }
#line 2426 "yacc_sql.tab.c"
    break;

  case 120: /* comOp: LT  */
#line 835 "yacc_sql.y"
         { CONTEXT->comp = LESS_THAN; }
#line 2432 "yacc_sql.tab.c"
    break;

  case 121: /* comOp: GT  */
#line 836 "yacc_sql.y"
         { CONTEXT->comp = GREAT_THAN; }
#line 2438 "yacc_sql.tab.c"
    break;

  case 122: /* comOp: LE  */
#line 837 "yacc_sql.y"
         { CONTEXT->comp = LESS_EQUAL; }
#line 2444 "yacc_sql.tab.c"
    break;

  case 123: /* comOp: GE  */
#line 838 "yacc_sql.y"
         { CONTEXT->comp = GREAT_EQUAL; }
#line 2450 "yacc_sql.tab.c"
    break;

  case 124: /* comOp: NE  */
#line 839 "yacc_sql.y"
         { CONTEXT->comp = NOT_EQUAL; }
#line 2456 "yacc_sql.tab.c"
    break;

  case 125: /* load_data: LOAD DATA INFILE SSS INTO TABLE ID SEMICOLON  */
#line 844 "yacc_sql.y"
                {
		  CONTEXT->ssql->flag = SCF_LOAD_DATA;
			load_data_init(&CONTEXT->ssql->sstr.load_data, (yyvsp[-1].string), (yyvsp[-4].string));
		}
#line 2465 "yacc_sql.tab.c"
    break;


#line 2469 "yacc_sql.tab.c"

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

#line 849 "yacc_sql.y"

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
