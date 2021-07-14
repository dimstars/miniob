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
  CompOp comp[MAX_NUM];
  AggOp agg;
  char id[MAX_NUM];
  Value null_value;
  int lbrace_num;	 // 当前未匹配的左括号数量
  int lbrace_selects[MAX_NUM]; // 表示上一个“左括号select”对应的lbrace数量
  int selects_num;
  Selects selects[MAX_NUM];
  Selects *sub_selects;
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
	context->selects_num = 0;
  	context->ssql->sstr.insertion.tuple_num = 0;
	context->ssql->sstr.errors = position;
	printf("parse sql failed. error=%s", str);
}

ParserContext *get_context(yyscan_t scanner) {
	return (ParserContext *)yyget_extra(scanner);
}

#define CONTEXT get_context(scanner)


#line 140 "yacc_sql.tab.c"

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
  YYSYMBOL_TEXT_T = 25,                    /* TEXT_T  */
  YYSYMBOL_FLOAT_T = 26,                   /* FLOAT_T  */
  YYSYMBOL_DATE_T = 27,                    /* DATE_T  */
  YYSYMBOL_HELP = 28,                      /* HELP  */
  YYSYMBOL_EXIT = 29,                      /* EXIT  */
  YYSYMBOL_DOT = 30,                       /* DOT  */
  YYSYMBOL_INTO = 31,                      /* INTO  */
  YYSYMBOL_VALUES = 32,                    /* VALUES  */
  YYSYMBOL_FROM = 33,                      /* FROM  */
  YYSYMBOL_WHERE = 34,                     /* WHERE  */
  YYSYMBOL_AND = 35,                       /* AND  */
  YYSYMBOL_SET = 36,                       /* SET  */
  YYSYMBOL_ON = 37,                        /* ON  */
  YYSYMBOL_LOAD = 38,                      /* LOAD  */
  YYSYMBOL_DATA = 39,                      /* DATA  */
  YYSYMBOL_INFILE = 40,                    /* INFILE  */
  YYSYMBOL_EQ = 41,                        /* EQ  */
  YYSYMBOL_LT = 42,                        /* LT  */
  YYSYMBOL_GT = 43,                        /* GT  */
  YYSYMBOL_LE = 44,                        /* LE  */
  YYSYMBOL_GE = 45,                        /* GE  */
  YYSYMBOL_NE = 46,                        /* NE  */
  YYSYMBOL_MAX = 47,                       /* MAX  */
  YYSYMBOL_MIN = 48,                       /* MIN  */
  YYSYMBOL_COUNT = 49,                     /* COUNT  */
  YYSYMBOL_AVG = 50,                       /* AVG  */
  YYSYMBOL_INNER = 51,                     /* INNER  */
  YYSYMBOL_JOIN = 52,                      /* JOIN  */
  YYSYMBOL_IS = 53,                        /* IS  */
  YYSYMBOL_NOT = 54,                       /* NOT  */
  YYSYMBOL_NULL_T = 55,                    /* NULL_T  */
  YYSYMBOL_NULLABLE = 56,                  /* NULLABLE  */
  YYSYMBOL_IN = 57,                        /* IN  */
  YYSYMBOL_PLUS_S = 58,                    /* PLUS_S  */
  YYSYMBOL_MINUS_S = 59,                   /* MINUS_S  */
  YYSYMBOL_SLASH = 60,                     /* SLASH  */
  YYSYMBOL_NUMBER = 61,                    /* NUMBER  */
  YYSYMBOL_FLOAT = 62,                     /* FLOAT  */
  YYSYMBOL_ID = 63,                        /* ID  */
  YYSYMBOL_PATH = 64,                      /* PATH  */
  YYSYMBOL_SSS = 65,                       /* SSS  */
  YYSYMBOL_STAR = 66,                      /* STAR  */
  YYSYMBOL_STRING_V = 67,                  /* STRING_V  */
  YYSYMBOL_DATE = 68,                      /* DATE  */
  YYSYMBOL_YYACCEPT = 69,                  /* $accept  */
  YYSYMBOL_commands = 70,                  /* commands  */
  YYSYMBOL_command = 71,                   /* command  */
  YYSYMBOL_exit = 72,                      /* exit  */
  YYSYMBOL_help = 73,                      /* help  */
  YYSYMBOL_sync = 74,                      /* sync  */
  YYSYMBOL_begin = 75,                     /* begin  */
  YYSYMBOL_commit = 76,                    /* commit  */
  YYSYMBOL_rollback = 77,                  /* rollback  */
  YYSYMBOL_drop_table = 78,                /* drop_table  */
  YYSYMBOL_show_tables = 79,               /* show_tables  */
  YYSYMBOL_desc_table = 80,                /* desc_table  */
  YYSYMBOL_create_index = 81,              /* create_index  */
  YYSYMBOL_create_unique_index = 82,       /* create_unique_index  */
  YYSYMBOL_drop_index = 83,                /* drop_index  */
  YYSYMBOL_create_table = 84,              /* create_table  */
  YYSYMBOL_attr_def_list = 85,             /* attr_def_list  */
  YYSYMBOL_attr_def = 86,                  /* attr_def  */
  YYSYMBOL_index_attr_def_list = 87,       /* index_attr_def_list  */
  YYSYMBOL_index_attr_def = 88,            /* index_attr_def  */
  YYSYMBOL_number = 89,                    /* number  */
  YYSYMBOL_float = 90,                     /* float  */
  YYSYMBOL_type = 91,                      /* type  */
  YYSYMBOL_ID_get = 92,                    /* ID_get  */
  YYSYMBOL_insert = 93,                    /* insert  */
  YYSYMBOL_tuple_list = 94,                /* tuple_list  */
  YYSYMBOL_tuple = 95,                     /* tuple  */
  YYSYMBOL_value_list = 96,                /* value_list  */
  YYSYMBOL_tuple_value = 97,               /* tuple_value  */
  YYSYMBOL_value = 98,                     /* value  */
  YYSYMBOL_delete = 99,                    /* delete  */
  YYSYMBOL_update = 100,                   /* update  */
  YYSYMBOL_select = 101,                   /* select  */
  YYSYMBOL_sub_select = 102,               /* sub_select  */
  YYSYMBOL_lbrace_select = 103,            /* lbrace_select  */
  YYSYMBOL_lbrace = 104,                   /* lbrace  */
  YYSYMBOL_rbrace = 105,                   /* rbrace  */
  YYSYMBOL_join_list = 106,                /* join_list  */
  YYSYMBOL_join_list_ = 107,               /* join_list_  */
  YYSYMBOL_inner_join = 108,               /* inner_join  */
  YYSYMBOL_select_attr = 109,              /* select_attr  */
  YYSYMBOL_agg_attr = 110,                 /* agg_attr  */
  YYSYMBOL_attr_list = 111,                /* attr_list  */
  YYSYMBOL_rel_list = 112,                 /* rel_list  */
  YYSYMBOL_where = 113,                    /* where  */
  YYSYMBOL_condition_list = 114,           /* condition_list  */
  YYSYMBOL_condition = 115,                /* condition  */
  YYSYMBOL_expr = 116,                     /* expr  */
  YYSYMBOL_term = 117,                     /* term  */
  YYSYMBOL_factor = 118,                   /* factor  */
  YYSYMBOL_base_value = 119,               /* base_value  */
  YYSYMBOL_sub_select_condition = 120,     /* sub_select_condition  */
  YYSYMBOL_comOp = 121,                    /* comOp  */
  YYSYMBOL_load_data = 122                 /* load_data  */
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
#define YYLAST   444

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  69
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  54
/* YYNRULES -- Number of rules.  */
#define YYNRULES  167
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  388

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   323


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
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   167,   167,   169,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   186,   187,   188,
     189,   190,   194,   199,   204,   210,   216,   222,   228,   234,
     240,   247,   254,   261,   268,   277,   279,   283,   294,   301,
     308,   320,   327,   336,   338,   340,   346,   347,   350,   351,
     354,   355,   356,   357,   358,   361,   369,   380,   381,   385,
     389,   390,   394,   397,   400,   404,   408,   414,   417,   420,
     424,   428,   434,   444,   453,   466,   482,   494,   506,   518,
     525,   531,   540,   545,   546,   552,   553,   557,   562,   567,
     572,   577,   581,   586,   591,   596,   601,   606,   611,   616,
     621,   626,   631,   636,   641,   647,   649,   654,   660,   666,
     672,   678,   684,   690,   696,   702,   708,   714,   720,   728,
     730,   734,   736,   738,   740,   743,   756,   767,   780,   792,
     804,   817,   831,   843,   855,   869,   883,   897,   911,   912,
     932,   935,   938,   944,   947,   950,   956,   959,   962,   968,
     973,   978,   983,   991,  1000,  1009,  1018,  1027,  1036,  1045,
    1054,  1066,  1067,  1068,  1069,  1070,  1071,  1075
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
  "TRX_BEGIN", "TRX_COMMIT", "TRX_ROLLBACK", "INT_T", "STRING_T", "TEXT_T",
  "FLOAT_T", "DATE_T", "HELP", "EXIT", "DOT", "INTO", "VALUES", "FROM",
  "WHERE", "AND", "SET", "ON", "LOAD", "DATA", "INFILE", "EQ", "LT", "GT",
  "LE", "GE", "NE", "MAX", "MIN", "COUNT", "AVG", "INNER", "JOIN", "IS",
  "NOT", "NULL_T", "NULLABLE", "IN", "PLUS_S", "MINUS_S", "SLASH",
  "NUMBER", "FLOAT", "ID", "PATH", "SSS", "STAR", "STRING_V", "DATE",
  "$accept", "commands", "command", "exit", "help", "sync", "begin",
  "commit", "rollback", "drop_table", "show_tables", "desc_table",
  "create_index", "create_unique_index", "drop_index", "create_table",
  "attr_def_list", "attr_def", "index_attr_def_list", "index_attr_def",
  "number", "float", "type", "ID_get", "insert", "tuple_list", "tuple",
  "value_list", "tuple_value", "value", "delete", "update", "select",
  "sub_select", "lbrace_select", "lbrace", "rbrace", "join_list",
  "join_list_", "inner_join", "select_attr", "agg_attr", "attr_list",
  "rel_list", "where", "condition_list", "condition", "expr", "term",
  "factor", "base_value", "sub_select_condition", "comOp", "load_data", YY_NULLPTR
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
     315,   316,   317,   318,   319,   320,   321,   322,   323
};
#endif

#define YYPACT_NINF (-277)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-151)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -277,   231,  -277,   124,   226,   -16,   -36,    33,    42,    30,
      56,    12,    91,   163,   168,   189,   195,    72,  -277,  -277,
    -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,
    -277,  -277,  -277,  -277,  -277,  -277,  -277,   150,   167,   264,
     218,   243,   270,   270,   270,   270,    50,  -277,   250,   289,
     291,   321,  -277,   267,   271,   322,  -277,  -277,  -277,  -277,
    -277,   317,   270,   323,   296,   358,   359,  -277,   -22,    47,
     266,    81,   154,   131,  -277,   300,  -277,  -277,  -277,   332,
     331,   303,   304,   305,   307,   330,  -277,  -277,    18,   353,
      19,   353,   311,  -277,    90,   353,   353,   106,   353,   270,
     270,   270,   270,   170,   289,   289,    87,   270,    60,   370,
     333,   344,  -277,   357,   325,   270,   314,  -277,   315,  -277,
    -277,   316,  -277,  -277,  -277,   318,  -277,  -277,  -277,   319,
    -277,  -277,   320,   324,   272,   326,   149,  -277,  -277,  -277,
     327,   328,  -277,   331,   329,   331,   365,   102,  -277,    -8,
     190,   241,   142,  -277,  -277,  -277,  -277,   275,   301,   161,
      29,   350,   220,   108,  -277,  -277,  -277,  -277,   212,   380,
     305,   353,  -277,  -277,  -277,  -277,  -277,    39,   334,   270,
     353,   353,   353,   353,   115,   116,   123,   353,   353,   175,
     289,   289,   369,  -277,   388,   356,   391,   270,   392,  -277,
      23,  -277,  -277,  -277,  -277,  -277,   377,     9,   190,   241,
     368,     9,  -277,   336,  -277,  -277,  -277,  -277,  -277,  -277,
     184,   343,   270,    97,   242,   227,   338,   224,   371,  -277,
    -277,  -277,    24,    60,  -277,     9,     9,     9,     9,     9,
     331,   339,   357,   400,   351,  -277,   209,  -277,   386,   334,
    -277,  -277,  -277,  -277,   345,   289,   346,   289,   347,   289,
     289,   289,   348,   289,  -277,  -277,  -277,  -277,    60,  -277,
     365,  -277,  -277,   102,   353,   349,   269,   352,  -277,   270,
    -277,   404,  -277,  -277,  -277,   360,  -277,   384,  -277,   387,
     355,   361,   362,  -277,   350,   108,   108,   278,  -277,  -277,
     413,   416,  -277,  -277,  -277,   353,   334,   353,   386,   353,
    -277,   353,  -277,   353,  -277,  -277,  -277,   353,  -277,   350,
    -277,   377,  -277,  -277,   285,   363,   270,   114,  -277,  -277,
    -277,   364,   366,   389,   331,   331,  -277,  -277,  -277,   237,
     386,   418,   353,   289,   289,   289,   289,   302,  -277,   373,
    -277,   270,  -277,   393,  -277,  -277,  -277,  -277,   367,   353,
     353,   376,  -277,  -277,  -277,   423,  -277,  -277,  -277,  -277,
    -277,   372,  -277,  -277,   374,   331,  -277,  -277,  -277,  -277,
     395,  -277,   353,    60,  -277,   350,   302,  -277
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     3,    21,
      20,    15,    16,    17,    18,     9,    10,    11,    12,    13,
      14,     8,     5,     7,     6,     4,    19,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   105,    87,     0,   105,
       0,     0,    24,     0,     0,     0,    25,    26,    27,    23,
      22,     0,     0,     0,     0,     0,     0,    80,     0,     0,
       0,     0,     0,     0,    89,     0,    91,    30,    29,     0,
     121,     0,     0,     0,     0,     0,    28,    33,     0,     0,
       0,     0,     0,    46,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   105,   105,   105,   119,     0,     0,     0,
       0,     0,    55,    35,     0,     0,     0,    81,     0,    92,
      93,     0,    95,    96,    47,     0,    99,    98,   101,     0,
     102,   103,     0,     0,     0,     0,     0,   107,    90,    88,
       0,     0,    86,   121,     0,   121,    57,     0,    71,     0,
      46,    48,   151,    69,    70,    67,    68,     0,     0,     0,
       0,   123,     0,   142,   145,   148,   138,    72,     0,     0,
       0,     0,    50,    51,    54,    52,    53,    40,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     105,   105,   119,    85,     0,     0,     0,     0,     0,    66,
       0,    48,    64,    65,    62,    63,    60,     0,    47,    49,
     151,     0,   147,     0,   161,   162,   163,   164,   165,   166,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    79,
     149,   150,     0,     0,   122,     0,     0,     0,     0,     0,
     121,     0,    35,     0,     0,    42,     0,    45,    43,     0,
      94,    97,   100,   104,     0,   105,     0,   105,     0,   105,
     105,   105,     0,   105,   108,   106,   120,    75,     0,    74,
      57,    56,    49,     0,     0,     0,   152,     0,   134,     0,
     153,     0,   127,   125,   157,     0,   132,   128,   126,   159,
       0,     0,     0,   146,   123,   140,   141,   139,   144,   143,
       0,     0,    36,    34,    41,     0,     0,     0,    43,     0,
     109,     0,   111,     0,   114,   113,   116,     0,   117,   123,
      58,    60,    59,   152,     0,     0,     0,     0,   136,   155,
     133,     0,     0,     0,   121,   121,   124,    73,   167,    37,
      43,     0,     0,   105,   105,   105,   105,    83,    61,     0,
     135,     0,   154,     0,   129,   158,   130,   160,     0,     0,
       0,     0,    39,    44,    31,     0,   110,   112,   115,   118,
      82,     0,   137,   156,     0,   121,    76,    78,    38,    32,
       0,   131,     0,     0,    77,   123,    83,    84
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,  -277,
    -277,  -277,  -277,  -277,  -277,  -277,   191,   268,  -273,  -228,
     -18,  -131,  -277,  -277,  -277,   164,   239,   118,   169,  -149,
    -277,  -277,  -277,  -200,  -277,   -42,   -84,  -277,    54,   335,
    -277,   284,   -41,   252,  -128,  -276,  -229,  -130,   120,  -121,
    -277,  -277,  -133,  -277
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,   171,   113,   307,   248,
     155,   156,   177,   114,    32,   198,   146,   274,   206,   157,
      33,    34,    35,   158,   159,   211,   119,   143,   370,   371,
      48,    49,    74,   145,   109,   234,   161,   162,   163,   164,
     165,   166,   223,    36
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      68,    69,    70,    71,   294,   120,   122,   123,    76,    67,
     126,   127,   128,   130,   131,   194,   205,   196,   336,   240,
      83,   308,   280,   284,   225,   226,    67,    50,   212,   237,
     232,    42,    43,    44,    45,   342,   117,   117,   229,   319,
      51,    88,   117,   347,    89,    52,    67,    46,   118,   121,
      47,   207,    96,   208,   209,   210,    67,   132,   133,   134,
     135,    53,   137,   138,   139,   147,   160,   363,   207,    72,
     230,   231,   210,   178,   283,    55,   288,    67,   340,   329,
      73,   232,   235,   236,   124,   272,   212,   243,   207,    54,
     230,   231,   210,   244,    56,   245,   250,   251,   252,   253,
     255,   257,   259,   260,   261,   263,   140,   297,   117,   386,
      90,    61,   300,    91,    67,   148,   188,   298,   299,   149,
     125,   150,   151,   152,   117,   153,   352,   355,   154,   204,
      37,    67,    38,   117,   117,   246,   129,   249,   141,   142,
      39,   117,   205,   327,    97,   254,   256,    98,   293,   264,
     265,   373,   148,   258,   385,   147,   200,   199,    93,   201,
     282,   200,   153,    93,   201,   154,    57,   202,   238,   148,
     203,    58,   213,   200,   239,    93,   201,   353,   354,   153,
     281,   281,   154,   214,   215,   216,   217,   218,   219,    72,
     322,   160,    59,   117,   104,   220,   221,   105,    60,   222,
     136,    99,   100,   101,   102,   262,   359,   360,    42,    43,
      44,    45,   190,    62,   310,   191,   312,   103,   314,   315,
     316,   339,   318,   341,   227,   343,   160,   344,   305,   345,
      63,     2,    40,   346,    41,     3,     4,   281,   277,   278,
       5,     6,     7,     8,     9,    10,    11,   382,  -149,  -149,
    -149,    12,    13,    14,   290,   204,  -149,   291,   365,    15,
      16,   214,   215,   216,   217,   218,   219,   148,    92,    17,
      93,   200,    64,    93,   201,   376,   377,   153,   235,   236,
     154,    65,   148,    75,   281,   281,   200,    67,    93,   201,
     287,   361,   153,   362,    77,   154,   285,   286,   384,  -150,
    -150,  -150,   366,   367,   368,   369,    66,  -150,    72,   281,
     214,   215,   216,   217,   218,   219,   214,   215,   216,   217,
     218,   219,   324,   325,    78,    92,   326,    93,   224,    94,
      79,    92,    95,    93,    80,   186,   235,   236,   187,   349,
     350,   160,   214,   215,   216,   217,   218,   219,   172,   173,
     174,   175,   176,   141,   142,   295,   296,    82,    81,    85,
      84,    86,    87,   106,   107,   108,   110,   116,   112,   111,
     115,   117,   124,   167,   168,   169,   170,   179,   180,   181,
     193,   182,   183,   184,   197,   233,   241,   185,   140,   189,
     192,   267,   195,   268,   269,   271,   273,   247,   275,   276,
     279,   289,   301,   303,   292,   306,   304,   328,   309,   311,
     313,   317,   323,   229,   331,   330,   337,   332,   333,   338,
     351,   364,   358,   374,   334,   335,   379,   356,   372,   357,
     375,   378,   383,   302,   320,   380,   270,   381,   242,   348,
     387,   144,   321,   228,   266
};

static const yytype_int16 yycheck[] =
{
      42,    43,    44,    45,   233,    89,    90,    91,    49,    17,
      94,    95,    96,    97,    98,   143,   147,   145,   294,   168,
      62,   249,   222,   223,   157,   158,    17,    63,   149,   162,
     160,    47,    48,    49,    50,   308,    18,    18,     9,   268,
       7,    63,    18,   319,    66,     3,    17,    63,    30,    30,
      66,    59,    70,    61,    62,    63,    17,    99,   100,   101,
     102,    31,   103,   104,   105,   107,   108,   340,    59,    19,
      61,    62,    63,   115,   223,    63,   225,    17,   306,   279,
      30,   211,    58,    59,    61,    62,   207,   171,    59,    33,
      61,    62,    63,    54,     3,    56,   180,   181,   182,   183,
     184,   185,   186,   187,   188,   189,    19,   237,    18,   385,
      63,    39,   240,    66,    17,    55,   134,   238,   239,    59,
      30,    61,    62,    63,    18,    65,   326,   327,    68,   147,
       6,    17,     8,    18,    18,   177,    30,   179,    51,    52,
      16,    18,   273,   276,    63,    30,    30,    66,   232,   190,
     191,   351,    55,    30,   383,   197,    59,    55,    61,    62,
      63,    59,    65,    61,    62,    68,     3,    65,    60,    55,
      68,     3,    30,    59,    66,    61,    62,    63,   327,    65,
     222,   223,    68,    41,    42,    43,    44,    45,    46,    19,
     274,   233,     3,    18,    63,    53,    54,    66,     3,    57,
      30,    47,    48,    49,    50,    30,   334,   335,    47,    48,
      49,    50,    63,    63,   255,    66,   257,    63,   259,   260,
     261,   305,   263,   307,    63,   309,   268,   311,   246,   313,
      63,     0,     6,   317,     8,     4,     5,   279,    54,    55,
       9,    10,    11,    12,    13,    14,    15,   375,    58,    59,
      60,    20,    21,    22,    30,   273,    66,    33,   342,    28,
      29,    41,    42,    43,    44,    45,    46,    55,    59,    38,
      61,    59,     8,    61,    62,   359,   360,    65,    58,    59,
      68,    63,    55,    33,   326,   327,    59,    17,    61,    62,
      63,    54,    65,    56,     3,    68,    54,    55,   382,    58,
      59,    60,   343,   344,   345,   346,    63,    66,    19,   351,
      41,    42,    43,    44,    45,    46,    41,    42,    43,    44,
      45,    46,    53,    54,     3,    59,    57,    61,    53,    63,
      63,    59,    66,    61,    63,    63,    58,    59,    66,    54,
      55,   383,    41,    42,    43,    44,    45,    46,    23,    24,
      25,    26,    27,    51,    52,   235,   236,    40,    36,    63,
      37,     3,     3,    63,    32,    34,    63,    37,    63,    65,
      63,    18,    61,     3,    41,    31,    19,    63,    63,    63,
      52,    63,    63,    63,    19,    35,     6,    63,    19,    63,
      63,     3,    63,    37,     3,     3,    19,    63,    30,    63,
      57,    63,    63,     3,    33,    19,    55,    55,    63,    63,
      63,    63,    63,     9,    30,    55,     3,    30,    63,     3,
      57,     3,    33,    30,    63,    63,     3,    63,    55,    63,
      63,    55,    37,   242,   270,    63,   197,    63,   170,   321,
     386,   106,   273,   159,   192
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    70,     0,     4,     5,     9,    10,    11,    12,    13,
      14,    15,    20,    21,    22,    28,    29,    38,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    93,    99,   100,   101,   122,     6,     8,    16,
       6,     8,    47,    48,    49,    50,    63,    66,   109,   110,
      63,     7,     3,    31,    33,    63,     3,     3,     3,     3,
       3,    39,    63,    63,     8,    63,    63,    17,   104,   104,
     104,   104,    19,    30,   111,    33,   111,     3,     3,    63,
      63,    36,    40,   104,    37,    63,     3,     3,    63,    66,
      63,    66,    59,    61,    63,    66,    89,    63,    66,    47,
      48,    49,    50,    63,    63,    66,    63,    32,    34,   113,
      63,    65,    63,    86,    92,    63,    37,    18,    30,   105,
     105,    30,   105,   105,    61,    30,   105,   105,   105,    30,
     105,   105,   104,   104,   104,   104,    30,   111,   111,   111,
      19,    51,    52,   106,   108,   112,    95,   104,    55,    59,
      61,    62,    63,    65,    68,    89,    90,    98,   102,   103,
     104,   115,   116,   117,   118,   119,   120,     3,    41,    31,
      19,    85,    23,    24,    25,    26,    27,    91,   104,    63,
      63,    63,    63,    63,    63,    63,    63,    66,    89,    63,
      63,    66,    63,    52,   113,    63,   113,    19,    94,    55,
      59,    62,    65,    68,    89,    90,    97,    59,    61,    62,
      63,   104,   118,    30,    41,    42,    43,    44,    45,    46,
      53,    54,    57,   121,    53,   121,   121,    63,   110,     9,
      61,    62,   116,    35,   114,    58,    59,   121,    60,    66,
      98,     6,    86,   105,    54,    56,   104,    63,    88,   104,
     105,   105,   105,   105,    30,   105,    30,   105,    30,   105,
     105,   105,    30,   105,   111,   111,   112,     3,    37,     3,
      95,     3,    62,    19,    96,    30,    63,    54,    55,    57,
     102,   104,    63,    98,   102,    54,    55,    63,    98,    63,
      30,    33,    33,   105,   115,   117,   117,   116,   118,   118,
     113,    63,    85,     3,    55,    89,    19,    87,    88,    63,
     111,    63,   111,    63,   111,   111,   111,    63,   111,   115,
      94,    97,   105,    63,    53,    54,    57,   121,    55,   102,
      55,    30,    30,    63,    63,    63,   114,     3,     3,   105,
      88,   105,    87,   105,   105,   105,   105,   114,    96,    54,
      55,    57,   102,    63,    98,   102,    63,    63,    33,   113,
     113,    54,    56,    87,     3,   105,   111,   111,   111,   111,
     107,   108,    55,   102,    30,    63,   105,   105,    55,     3,
      63,    63,   113,    37,   105,   115,   114,   107
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    69,    70,    70,    71,    71,    71,    71,    71,    71,
      71,    71,    71,    71,    71,    71,    71,    71,    71,    71,
      71,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    85,    86,    86,    86,
      86,    86,    86,    87,    87,    88,    89,    89,    90,    90,
      91,    91,    91,    91,    91,    92,    93,    94,    94,    95,
      96,    96,    97,    97,    97,    97,    97,    98,    98,    98,
      98,    98,    99,   100,   101,   101,   102,   102,   102,   103,
     104,   105,   106,   107,   107,   108,   108,   109,   109,   109,
     109,   109,   110,   110,   110,   110,   110,   110,   110,   110,
     110,   110,   110,   110,   110,   111,   111,   111,   111,   111,
     111,   111,   111,   111,   111,   111,   111,   111,   111,   112,
     112,   113,   113,   114,   114,   115,   115,   115,   115,   115,
     115,   115,   115,   115,   115,   115,   115,   115,   115,   115,
     116,   116,   116,   117,   117,   117,   118,   118,   118,   119,
     119,   119,   119,   120,   120,   120,   120,   120,   120,   120,
     120,   121,   121,   121,   121,   121,   121,   122
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     2,     2,     2,     2,     4,     3,
       3,    10,    11,     4,     8,     0,     3,     5,     7,     6,
       2,     4,     3,     0,     3,     1,     1,     2,     1,     2,
       1,     1,     1,     1,     1,     1,     7,     0,     3,     4,
       0,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     5,     8,     7,     7,     6,     8,     6,     2,
       1,     1,     6,     0,     6,     2,     1,     1,     4,     2,
       4,     2,     4,     4,     6,     4,     4,     6,     4,     4,
       6,     4,     4,     4,     6,     0,     5,     3,     5,     6,
       8,     6,     8,     6,     6,     8,     6,     6,     8,     0,
       3,     0,     3,     0,     3,     3,     3,     3,     3,     5,
       5,     7,     3,     4,     3,     5,     4,     6,     1,     3,
       3,     3,     1,     3,     3,     1,     3,     2,     1,     1,
       1,     1,     3,     3,     5,     4,     6,     3,     5,     3,
       5,     1,     1,     1,     1,     1,     1,     8
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
#line 194 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_EXIT;//"exit";
    }
#line 1530 "yacc_sql.tab.c"
    break;

  case 23: /* help: HELP SEMICOLON  */
#line 199 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_HELP;//"help";
    }
#line 1538 "yacc_sql.tab.c"
    break;

  case 24: /* sync: SYNC SEMICOLON  */
#line 204 "yacc_sql.y"
                   {
      CONTEXT->ssql->flag = SCF_SYNC;
    }
#line 1546 "yacc_sql.tab.c"
    break;

  case 25: /* begin: TRX_BEGIN SEMICOLON  */
#line 210 "yacc_sql.y"
                        {
      CONTEXT->ssql->flag = SCF_BEGIN;
    }
#line 1554 "yacc_sql.tab.c"
    break;

  case 26: /* commit: TRX_COMMIT SEMICOLON  */
#line 216 "yacc_sql.y"
                         {
      CONTEXT->ssql->flag = SCF_COMMIT;
    }
#line 1562 "yacc_sql.tab.c"
    break;

  case 27: /* rollback: TRX_ROLLBACK SEMICOLON  */
#line 222 "yacc_sql.y"
                           {
      CONTEXT->ssql->flag = SCF_ROLLBACK;
    }
#line 1570 "yacc_sql.tab.c"
    break;

  case 28: /* drop_table: DROP TABLE ID SEMICOLON  */
#line 228 "yacc_sql.y"
                            {
        CONTEXT->ssql->flag = SCF_DROP_TABLE;//"drop_table";
				drop_table_init(&CONTEXT->ssql->sstr.drop_table, (yyvsp[-1].string));
    }
#line 1579 "yacc_sql.tab.c"
    break;

  case 29: /* show_tables: SHOW TABLES SEMICOLON  */
#line 234 "yacc_sql.y"
                          {
      CONTEXT->ssql->flag = SCF_SHOW_TABLES;
    }
#line 1587 "yacc_sql.tab.c"
    break;

  case 30: /* desc_table: DESC ID SEMICOLON  */
#line 240 "yacc_sql.y"
                      {
      CONTEXT->ssql->flag = SCF_DESC_TABLE;
			desc_table_init(&CONTEXT->ssql->sstr.desc_table, (yyvsp[-1].string));
    }
#line 1596 "yacc_sql.tab.c"
    break;

  case 31: /* create_index: CREATE INDEX ID ON ID lbrace index_attr_def index_attr_def_list rbrace SEMICOLON  */
#line 248 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, (yyvsp[-7].string), (yyvsp[-5].string));
		}
#line 1605 "yacc_sql.tab.c"
    break;

  case 32: /* create_unique_index: CREATE UNIQUE INDEX ID ON ID lbrace index_attr_def index_attr_def_list rbrace SEMICOLON  */
#line 255 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_CREATE_UNIQUE_INDEX;//"create_unique_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, (yyvsp[-7].string), (yyvsp[-5].string));
		}
#line 1614 "yacc_sql.tab.c"
    break;

  case 33: /* drop_index: DROP INDEX ID SEMICOLON  */
#line 262 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_DROP_INDEX;//"drop_index";
			drop_index_init(&CONTEXT->ssql->sstr.drop_index, (yyvsp[-1].string));
		}
#line 1623 "yacc_sql.tab.c"
    break;

  case 34: /* create_table: CREATE TABLE ID lbrace attr_def attr_def_list rbrace SEMICOLON  */
#line 269 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_CREATE_TABLE;//"create_table";
			// CONTEXT->ssql->sstr.create_table.attribute_count = CONTEXT->value_length; // TODO
			create_table_init_name(&CONTEXT->ssql->sstr.create_table, (yyvsp[-5].string));
			//临时变量清零	
			CONTEXT->value_length = 0;
		}
#line 1635 "yacc_sql.tab.c"
    break;

  case 36: /* attr_def_list: COMMA attr_def attr_def_list  */
#line 279 "yacc_sql.y"
                                   {    }
#line 1641 "yacc_sql.tab.c"
    break;

  case 37: /* attr_def: ID_get type lbrace number rbrace  */
#line 284 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-3].number), (yyvsp[-1].number), 0);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name =(char*)malloc(sizeof(char)); // TODO FATAL
			// strcpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type = $2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length = $4;
			CONTEXT->value_length++;
		}
#line 1656 "yacc_sql.tab.c"
    break;

  case 38: /* attr_def: ID_get type lbrace number rbrace NOT NULL_T  */
#line 295 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-5].number), (yyvsp[-3].number), 0);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
		}
#line 1667 "yacc_sql.tab.c"
    break;

  case 39: /* attr_def: ID_get type lbrace number rbrace NULLABLE  */
#line 302 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-4].number), (yyvsp[-2].number), 1);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
		}
#line 1678 "yacc_sql.tab.c"
    break;

  case 40: /* attr_def: ID_get type  */
#line 309 "yacc_sql.y"
                {
			AttrInfo attribute;
			// text : 4B len + 4B page num or 8B ptr
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[0].number), (yyvsp[0].number) == TEXTS ? 8 : 4, 0);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name=(char*)malloc(sizeof(char));
			// strcpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type=$2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length=4; // default attribute length
			CONTEXT->value_length++;
		}
#line 1694 "yacc_sql.tab.c"
    break;

  case 41: /* attr_def: ID_get type NOT NULL_T  */
#line 321 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-2].number), (yyvsp[-2].number) == TEXTS ? 8 : 4, 0);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
		}
#line 1705 "yacc_sql.tab.c"
    break;

  case 42: /* attr_def: ID_get type NULLABLE  */
#line 328 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-1].number), (yyvsp[-1].number) == TEXTS ? 8 : 4, 1);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
		}
#line 1716 "yacc_sql.tab.c"
    break;

  case 44: /* index_attr_def_list: COMMA index_attr_def index_attr_def_list  */
#line 338 "yacc_sql.y"
                                                   {    }
#line 1722 "yacc_sql.tab.c"
    break;

  case 45: /* index_attr_def: ID  */
#line 341 "yacc_sql.y"
                {
			create_index_append_attribute(&CONTEXT->ssql->sstr.create_index, (yyvsp[0].string));
			CONTEXT->value_length++;
		}
#line 1731 "yacc_sql.tab.c"
    break;

  case 46: /* number: NUMBER  */
#line 346 "yacc_sql.y"
               {(yyval.number) = (yyvsp[0].number);}
#line 1737 "yacc_sql.tab.c"
    break;

  case 47: /* number: MINUS_S NUMBER  */
#line 347 "yacc_sql.y"
                         {(yyval.number) = -(yyvsp[0].number);}
#line 1743 "yacc_sql.tab.c"
    break;

  case 48: /* float: FLOAT  */
#line 350 "yacc_sql.y"
              {(yyval.floats) = (yyvsp[0].floats);}
#line 1749 "yacc_sql.tab.c"
    break;

  case 49: /* float: MINUS_S FLOAT  */
#line 351 "yacc_sql.y"
                        {(yyval.floats) = -(yyvsp[0].floats);}
#line 1755 "yacc_sql.tab.c"
    break;

  case 50: /* type: INT_T  */
#line 354 "yacc_sql.y"
              { (yyval.number)=INTS; }
#line 1761 "yacc_sql.tab.c"
    break;

  case 51: /* type: STRING_T  */
#line 355 "yacc_sql.y"
                  { (yyval.number)=CHARS; }
#line 1767 "yacc_sql.tab.c"
    break;

  case 52: /* type: FLOAT_T  */
#line 356 "yacc_sql.y"
                 { (yyval.number)=FLOATS; }
#line 1773 "yacc_sql.tab.c"
    break;

  case 53: /* type: DATE_T  */
#line 357 "yacc_sql.y"
                    { (yyval.number)=DATES; }
#line 1779 "yacc_sql.tab.c"
    break;

  case 54: /* type: TEXT_T  */
#line 358 "yacc_sql.y"
                    { (yyval.number)=TEXTS; }
#line 1785 "yacc_sql.tab.c"
    break;

  case 55: /* ID_get: ID  */
#line 362 "yacc_sql.y"
        {
		char *temp=(yyvsp[0].string); 
		snprintf(CONTEXT->id, sizeof(CONTEXT->id), "%s", temp);
	}
#line 1794 "yacc_sql.tab.c"
    break;

  case 56: /* insert: INSERT INTO ID VALUES tuple tuple_list SEMICOLON  */
#line 370 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_INSERT;
			inserts_init(&CONTEXT->ssql->sstr.insertion, (yyvsp[-4].string), CONTEXT->tuples, CONTEXT->tuple_num, CONTEXT->tuple_length);
      		//临时变量清零
      		for (int i = 0; i < CONTEXT->tuple_num; i++) {
				CONTEXT->tuple_length[i] = 0;
			}
			CONTEXT->tuple_num = 0;
    }
#line 1808 "yacc_sql.tab.c"
    break;

  case 58: /* tuple_list: COMMA tuple tuple_list  */
#line 381 "yacc_sql.y"
                                 {
	}
#line 1815 "yacc_sql.tab.c"
    break;

  case 59: /* tuple: lbrace tuple_value value_list rbrace  */
#line 385 "yacc_sql.y"
                                             {
		CONTEXT->tuple_num++;
	}
#line 1823 "yacc_sql.tab.c"
    break;

  case 61: /* value_list: COMMA tuple_value value_list  */
#line 390 "yacc_sql.y"
                                    { 
	  	}
#line 1830 "yacc_sql.tab.c"
    break;

  case 62: /* tuple_value: number  */
#line 394 "yacc_sql.y"
           {
  			value_init_integer(&CONTEXT->tuples[CONTEXT->tuple_num][CONTEXT->tuple_length[CONTEXT->tuple_num]++], (yyvsp[0].number));
		}
#line 1838 "yacc_sql.tab.c"
    break;

  case 63: /* tuple_value: float  */
#line 397 "yacc_sql.y"
            {
  			value_init_float(&CONTEXT->tuples[CONTEXT->tuple_num][CONTEXT->tuple_length[CONTEXT->tuple_num]++], (yyvsp[0].floats));
		}
#line 1846 "yacc_sql.tab.c"
    break;

  case 64: /* tuple_value: SSS  */
#line 400 "yacc_sql.y"
         {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
  			value_init_string(&CONTEXT->tuples[CONTEXT->tuple_num][CONTEXT->tuple_length[CONTEXT->tuple_num]++], (yyvsp[0].string));
		}
#line 1855 "yacc_sql.tab.c"
    break;

  case 65: /* tuple_value: DATE  */
#line 404 "yacc_sql.y"
              {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
			value_init_date(&CONTEXT->tuples[CONTEXT->tuple_num][CONTEXT->tuple_length[CONTEXT->tuple_num]++], (yyvsp[0].string));
		}
#line 1864 "yacc_sql.tab.c"
    break;

  case 66: /* tuple_value: NULL_T  */
#line 408 "yacc_sql.y"
                {
			value_init_null(&CONTEXT->tuples[CONTEXT->tuple_num][CONTEXT->tuple_length[CONTEXT->tuple_num]++]);
		}
#line 1872 "yacc_sql.tab.c"
    break;

  case 67: /* value: number  */
#line 414 "yacc_sql.y"
          {	
  		value_init_integer(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].number));
		}
#line 1880 "yacc_sql.tab.c"
    break;

  case 68: /* value: float  */
#line 417 "yacc_sql.y"
            {
  		value_init_float(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].floats));
		}
#line 1888 "yacc_sql.tab.c"
    break;

  case 69: /* value: SSS  */
#line 420 "yacc_sql.y"
         {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
  		value_init_string(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].string));
		}
#line 1897 "yacc_sql.tab.c"
    break;

  case 70: /* value: DATE  */
#line 424 "yacc_sql.y"
              {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
			value_init_date(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].string));
		}
#line 1906 "yacc_sql.tab.c"
    break;

  case 71: /* value: NULL_T  */
#line 428 "yacc_sql.y"
                {
			value_init_null(&CONTEXT->values[CONTEXT->value_length++]);
		}
#line 1914 "yacc_sql.tab.c"
    break;

  case 72: /* delete: DELETE FROM ID where SEMICOLON  */
#line 435 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_DELETE;//"delete";
			deletes_init_relation(&CONTEXT->ssql->sstr.deletion, (yyvsp[-2].string));
			deletes_set_conditions(&CONTEXT->ssql->sstr.deletion, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;	
    }
#line 1926 "yacc_sql.tab.c"
    break;

  case 73: /* update: UPDATE ID SET ID EQ value where SEMICOLON  */
#line 445 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_UPDATE;//"update";
			Value *value = &CONTEXT->values[0];
			updates_init(&CONTEXT->ssql->sstr.update, (yyvsp[-6].string), (yyvsp[-4].string), value, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;
		}
#line 1938 "yacc_sql.tab.c"
    break;

  case 74: /* select: SELECT select_attr FROM ID rel_list where SEMICOLON  */
#line 453 "yacc_sql.y"
                                                        {
		CONTEXT->ssql->sstr.selection = CONTEXT->selects[CONTEXT->selects_num];
		memset(&CONTEXT->selects[CONTEXT->selects_num], 0, sizeof(Selects));
		selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-3].string));

		CONTEXT->ssql->flag=SCF_SELECT;//"select";

		//临时变量清零
		CONTEXT->condition_length=0;
		CONTEXT->from_length=0;
		CONTEXT->select_length=0;
		CONTEXT->value_length = 0;
	}
#line 1956 "yacc_sql.tab.c"
    break;

  case 75: /* select: SELECT select_attr FROM ID join_list where SEMICOLON  */
#line 466 "yacc_sql.y"
                                                               {
		CONTEXT->ssql->sstr.selection = CONTEXT->selects[CONTEXT->selects_num];
		memset(&CONTEXT->selects[CONTEXT->selects_num], 0, sizeof(Selects));
		selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-3].string));

		CONTEXT->ssql->flag=SCF_SELECT;//"select";

		//临时变量清零
		CONTEXT->condition_length=0;
		CONTEXT->from_length=0;
		CONTEXT->select_length=0;
		CONTEXT->value_length = 0;
	}
#line 1974 "yacc_sql.tab.c"
    break;

  case 76: /* sub_select: lbrace_select ID FROM ID where rbrace  */
#line 482 "yacc_sql.y"
                                         {
		Selects *sub_selects = (Selects *)malloc(sizeof(Selects));
		*sub_selects = CONTEXT->selects[CONTEXT->selects_num + 1];
		memset(&CONTEXT->selects[CONTEXT->selects_num + 1], 0, sizeof(Selects));
		
		RelAttr left_attr;
		relation_attr_init(&left_attr, (yyvsp[-2].string), (yyvsp[-4].string));
		selects_append_attribute(sub_selects, &left_attr);
		selects_append_relation(sub_selects, (yyvsp[-2].string));

		CONTEXT->sub_selects = sub_selects;
    }
#line 1991 "yacc_sql.tab.c"
    break;

  case 77: /* sub_select: lbrace_select ID DOT ID FROM ID where rbrace  */
#line 494 "yacc_sql.y"
                                                   {
		Selects *sub_selects = (Selects *)malloc(sizeof(Selects));
		*sub_selects = CONTEXT->selects[CONTEXT->selects_num + 1];
		memset(&CONTEXT->selects[CONTEXT->selects_num + 1], 0, sizeof(Selects));
		
		RelAttr left_attr;
		relation_attr_init(&left_attr, (yyvsp[-6].string), (yyvsp[-4].string));
		selects_append_attribute(sub_selects, &left_attr);
		selects_append_relation(sub_selects, (yyvsp[-2].string));

		CONTEXT->sub_selects = sub_selects;
    }
#line 2008 "yacc_sql.tab.c"
    break;

  case 78: /* sub_select: lbrace_select agg_attr FROM ID where rbrace  */
#line 506 "yacc_sql.y"
                                                  {
		Selects *sub_selects = (Selects *)malloc(sizeof(Selects));
		*sub_selects = CONTEXT->selects[CONTEXT->selects_num + 1];
		memset(&CONTEXT->selects[CONTEXT->selects_num + 1], 0, sizeof(Selects));

		selects_append_relation(sub_selects, (yyvsp[-2].string));

		CONTEXT->sub_selects = sub_selects;
    }
#line 2022 "yacc_sql.tab.c"
    break;

  case 79: /* lbrace_select: lbrace SELECT  */
#line 518 "yacc_sql.y"
                      {
		CONTEXT->selects_num ++;
		CONTEXT->lbrace_selects[CONTEXT->selects_num] = CONTEXT->lbrace_num;
	}
#line 2031 "yacc_sql.tab.c"
    break;

  case 80: /* lbrace: LBRACE  */
#line 525 "yacc_sql.y"
               {
		CONTEXT->lbrace_num ++;
	}
#line 2039 "yacc_sql.tab.c"
    break;

  case 81: /* rbrace: RBRACE  */
#line 531 "yacc_sql.y"
               {
		if (CONTEXT->lbrace_selects[CONTEXT->selects_num] == CONTEXT->lbrace_num && CONTEXT->selects_num) {
			CONTEXT->selects_num --;
		}
		CONTEXT->lbrace_num --;
	}
#line 2050 "yacc_sql.tab.c"
    break;

  case 82: /* join_list: inner_join ID ON condition condition_list join_list_  */
#line 540 "yacc_sql.y"
                                                             {
		selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-4].string));
		selects_append_relation(&CONTEXT->selects[CONTEXT->selects_num], (yyvsp[-4].string));
	}
#line 2059 "yacc_sql.tab.c"
    break;

  case 84: /* join_list_: inner_join ID ON condition condition_list join_list_  */
#line 546 "yacc_sql.y"
                                                               {
		selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-4].string));
		selects_append_relation(&CONTEXT->selects[CONTEXT->selects_num], (yyvsp[-4].string));
	}
#line 2068 "yacc_sql.tab.c"
    break;

  case 85: /* inner_join: INNER JOIN  */
#line 552 "yacc_sql.y"
                   {}
#line 2074 "yacc_sql.tab.c"
    break;

  case 86: /* inner_join: JOIN  */
#line 553 "yacc_sql.y"
               {}
#line 2080 "yacc_sql.tab.c"
    break;

  case 87: /* select_attr: STAR  */
#line 557 "yacc_sql.y"
         {  
			RelAttr attr;
			relation_attr_init(&attr, NULL, "*");
			selects_append_attribute(&CONTEXT->selects[CONTEXT->selects_num], &attr);
		}
#line 2090 "yacc_sql.tab.c"
    break;

  case 88: /* select_attr: ID DOT STAR attr_list  */
#line 562 "yacc_sql.y"
                                {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), "*");
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 2100 "yacc_sql.tab.c"
    break;

  case 89: /* select_attr: ID attr_list  */
#line 567 "yacc_sql.y"
                   {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->selects[CONTEXT->selects_num], &attr);
		}
#line 2110 "yacc_sql.tab.c"
    break;

  case 90: /* select_attr: ID DOT ID attr_list  */
#line 572 "yacc_sql.y"
                              {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->selects[CONTEXT->selects_num], &attr);
		}
#line 2120 "yacc_sql.tab.c"
    break;

  case 91: /* select_attr: agg_attr attr_list  */
#line 577 "yacc_sql.y"
                             {}
#line 2126 "yacc_sql.tab.c"
    break;

  case 92: /* agg_attr: MAX lbrace ID rbrace  */
#line 581 "yacc_sql.y"
                             {
			AggOp agg;
			aggregation_init_string(&agg, NULL, (yyvsp[-1].string), MAX_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2136 "yacc_sql.tab.c"
    break;

  case 93: /* agg_attr: MAX lbrace STAR rbrace  */
#line 586 "yacc_sql.y"
                                 {
			AggOp agg;
			aggregation_init_string(&agg, NULL, "*", MAX_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2146 "yacc_sql.tab.c"
    break;

  case 94: /* agg_attr: MAX lbrace ID DOT ID rbrace  */
#line 591 "yacc_sql.y"
                                      {
			AggOp agg;
			aggregation_init_string(&agg, (yyvsp[-3].string), (yyvsp[-1].string), MAX_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2156 "yacc_sql.tab.c"
    break;

  case 95: /* agg_attr: MIN lbrace ID rbrace  */
#line 596 "yacc_sql.y"
                               {
			AggOp agg;
			aggregation_init_string(&agg, NULL, (yyvsp[-1].string), MIN_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2166 "yacc_sql.tab.c"
    break;

  case 96: /* agg_attr: MIN lbrace STAR rbrace  */
#line 601 "yacc_sql.y"
                                 {
			AggOp agg;
			aggregation_init_string(&agg, NULL, "*", MIN_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2176 "yacc_sql.tab.c"
    break;

  case 97: /* agg_attr: MIN lbrace ID DOT ID rbrace  */
#line 606 "yacc_sql.y"
                                      {
			AggOp agg;
			aggregation_init_string(&agg, (yyvsp[-3].string), (yyvsp[-1].string), MIN_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2186 "yacc_sql.tab.c"
    break;

  case 98: /* agg_attr: COUNT lbrace STAR rbrace  */
#line 611 "yacc_sql.y"
                                   {
			AggOp agg;
			aggregation_init_string(&agg, NULL, "*", COUNT_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2196 "yacc_sql.tab.c"
    break;

  case 99: /* agg_attr: COUNT lbrace ID rbrace  */
#line 616 "yacc_sql.y"
                                 {
			AggOp agg;
			aggregation_init_string(&agg, NULL, (yyvsp[-1].string), COUNT_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2206 "yacc_sql.tab.c"
    break;

  case 100: /* agg_attr: COUNT lbrace ID DOT ID rbrace  */
#line 621 "yacc_sql.y"
                                        {
			AggOp agg;
			aggregation_init_string(&agg, (yyvsp[-3].string), (yyvsp[-1].string), COUNT_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2216 "yacc_sql.tab.c"
    break;

  case 101: /* agg_attr: COUNT lbrace number rbrace  */
#line 626 "yacc_sql.y"
                                     {  //  TODO optimize count(n)
			AggOp agg;
			aggregation_init_integer(&agg, NULL, (yyvsp[-1].number), COUNT_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2226 "yacc_sql.tab.c"
    break;

  case 102: /* agg_attr: AVG lbrace ID rbrace  */
#line 631 "yacc_sql.y"
                               {
			AggOp agg;
			aggregation_init_string(&agg, NULL, (yyvsp[-1].string), AVG_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2236 "yacc_sql.tab.c"
    break;

  case 103: /* agg_attr: AVG lbrace STAR rbrace  */
#line 636 "yacc_sql.y"
                                 {
			AggOp agg;
			aggregation_init_string(&agg, NULL, "*", AVG_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2246 "yacc_sql.tab.c"
    break;

  case 104: /* agg_attr: AVG lbrace ID DOT ID rbrace  */
#line 641 "yacc_sql.y"
                                      {
			AggOp agg;
			aggregation_init_string(&agg, (yyvsp[-3].string), (yyvsp[-1].string), AVG_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2256 "yacc_sql.tab.c"
    break;

  case 106: /* attr_list: COMMA ID DOT STAR attr_list  */
#line 649 "yacc_sql.y"
                                      {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), "*");
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 2266 "yacc_sql.tab.c"
    break;

  case 107: /* attr_list: COMMA ID attr_list  */
#line 654 "yacc_sql.y"
                         {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			selects_append_attribute(&CONTEXT->selects[CONTEXT->selects_num], &attr);
      }
#line 2277 "yacc_sql.tab.c"
    break;

  case 108: /* attr_list: COMMA ID DOT ID attr_list  */
#line 660 "yacc_sql.y"
                                {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			selects_append_attribute(&CONTEXT->selects[CONTEXT->selects_num], &attr);
  	  }
#line 2288 "yacc_sql.tab.c"
    break;

  case 109: /* attr_list: COMMA MAX lbrace ID rbrace attr_list  */
#line 666 "yacc_sql.y"
                                               {
			AggOp agg;
			aggregation_init_string(&agg, NULL, (yyvsp[-2].string), MAX_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2299 "yacc_sql.tab.c"
    break;

  case 110: /* attr_list: COMMA MAX lbrace ID DOT ID rbrace attr_list  */
#line 672 "yacc_sql.y"
                                                      {
			AggOp agg;
			aggregation_init_string(&agg, (yyvsp[-4].string), (yyvsp[-2].string), MAX_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2310 "yacc_sql.tab.c"
    break;

  case 111: /* attr_list: COMMA MIN lbrace ID rbrace attr_list  */
#line 678 "yacc_sql.y"
                                               {
			AggOp agg;
			aggregation_init_string(&agg, NULL, (yyvsp[-2].string), MIN_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2321 "yacc_sql.tab.c"
    break;

  case 112: /* attr_list: COMMA MIN lbrace ID DOT ID rbrace attr_list  */
#line 684 "yacc_sql.y"
                                                      {
			AggOp agg;
			aggregation_init_string(&agg, (yyvsp[-4].string), (yyvsp[-2].string), MIN_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2332 "yacc_sql.tab.c"
    break;

  case 113: /* attr_list: COMMA COUNT lbrace STAR rbrace attr_list  */
#line 690 "yacc_sql.y"
                                                   { // count(*) 忽略null记录
			AggOp agg;
			aggregation_init_string(&agg, NULL, "*", COUNT_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2343 "yacc_sql.tab.c"
    break;

  case 114: /* attr_list: COMMA COUNT lbrace ID rbrace attr_list  */
#line 696 "yacc_sql.y"
                                                 {
			AggOp agg;
			aggregation_init_string(&agg, NULL, (yyvsp[-2].string), COUNT_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2354 "yacc_sql.tab.c"
    break;

  case 115: /* attr_list: COMMA COUNT lbrace ID DOT ID rbrace attr_list  */
#line 702 "yacc_sql.y"
                                                        {
			AggOp agg;
			aggregation_init_string(&agg, (yyvsp[-4].string), (yyvsp[-2].string), COUNT_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2365 "yacc_sql.tab.c"
    break;

  case 116: /* attr_list: COMMA COUNT lbrace number rbrace attr_list  */
#line 708 "yacc_sql.y"
                                                     {  //  TODO optimize count(n) // conut(n) 不忽略null记录
			AggOp agg;
			aggregation_init_integer(&agg, NULL, (yyvsp[-2].number), COUNT_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2376 "yacc_sql.tab.c"
    break;

  case 117: /* attr_list: COMMA AVG lbrace ID rbrace attr_list  */
#line 714 "yacc_sql.y"
                                               {
			AggOp agg;
			aggregation_init_string(&agg, NULL, (yyvsp[-2].string), AVG_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2387 "yacc_sql.tab.c"
    break;

  case 118: /* attr_list: COMMA AVG lbrace ID DOT ID rbrace attr_list  */
#line 720 "yacc_sql.y"
                                                      {
			AggOp agg;
			aggregation_init_string(&agg, (yyvsp[-4].string), (yyvsp[-2].string), AVG_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2398 "yacc_sql.tab.c"
    break;

  case 120: /* rel_list: COMMA ID rel_list  */
#line 730 "yacc_sql.y"
                        {	
				selects_append_relation(&CONTEXT->selects[CONTEXT->selects_num], (yyvsp[-1].string));
		  }
#line 2406 "yacc_sql.tab.c"
    break;

  case 122: /* where: WHERE condition condition_list  */
#line 736 "yacc_sql.y"
                                     {}
#line 2412 "yacc_sql.tab.c"
    break;

  case 124: /* condition_list: AND condition condition_list  */
#line 740 "yacc_sql.y"
                                   {}
#line 2418 "yacc_sql.tab.c"
    break;

  case 125: /* condition: ID comOp value  */
#line 744 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, (yyvsp[-2].string));

			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp[CONTEXT->selects_num], 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
			selects->conditions[selects->condition_num++] = condition;
		}
#line 2435 "yacc_sql.tab.c"
    break;

  case 126: /* condition: value comOp value  */
#line 757 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 2];
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp[CONTEXT->selects_num], 0, NULL, left_value, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
			selects->conditions[selects->condition_num++] = condition;
		}
#line 2450 "yacc_sql.tab.c"
    break;

  case 127: /* condition: ID comOp ID  */
#line 768 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, (yyvsp[-2].string));
			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp[CONTEXT->selects_num], 1, &left_attr, NULL, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
			selects->conditions[selects->condition_num++] = condition;
		}
#line 2467 "yacc_sql.tab.c"
    break;

  case 128: /* condition: value comOp ID  */
#line 781 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];
			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp[CONTEXT->selects_num], 0, NULL, left_value, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
			selects->conditions[selects->condition_num++] = condition;
		}
#line 2483 "yacc_sql.tab.c"
    break;

  case 129: /* condition: ID DOT ID comOp value  */
#line 793 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, (yyvsp[-4].string), (yyvsp[-2].string));
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp[CONTEXT->selects_num], 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
			selects->conditions[selects->condition_num++] = condition;								
    }
#line 2499 "yacc_sql.tab.c"
    break;

  case 130: /* condition: value comOp ID DOT ID  */
#line 805 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];

			RelAttr right_attr;
			relation_attr_init(&right_attr, (yyvsp[-2].string), (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp[CONTEXT->selects_num], 0, NULL, left_value, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
			selects->conditions[selects->condition_num++] = condition;								
    }
#line 2516 "yacc_sql.tab.c"
    break;

  case 131: /* condition: ID DOT ID comOp ID DOT ID  */
#line 818 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, (yyvsp[-6].string), (yyvsp[-4].string));
			RelAttr right_attr;
			relation_attr_init(&right_attr, (yyvsp[-2].string), (yyvsp[0].string));

			Condition condition;
			condition_init(&condition, CONTEXT->comp[CONTEXT->selects_num], 1, &left_attr, NULL, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
			selects->conditions[selects->condition_num++] = condition;
       }
#line 2533 "yacc_sql.tab.c"
    break;

  case 132: /* condition: value IS NULL_T  */
#line 832 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];
			Value *right_value = &CONTEXT->null_value;
			value_init_null(right_value);

			Condition condition;
			condition_init(&condition, IS_NULL, 0, NULL, left_value, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
			selects->conditions[selects->condition_num++] = condition;
		}
#line 2549 "yacc_sql.tab.c"
    break;

  case 133: /* condition: value IS NOT NULL_T  */
#line 844 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];
			Value *right_value = &CONTEXT->null_value;
			value_init_null(right_value);

			Condition condition;
			condition_init(&condition, IS_NOT_NULL, 0, NULL, left_value, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
			selects->conditions[selects->condition_num++] = condition;
		}
#line 2565 "yacc_sql.tab.c"
    break;

  case 134: /* condition: ID IS NULL_T  */
#line 856 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, (yyvsp[-2].string));
			
			Value *right_value = &CONTEXT->null_value;
			value_init_null(right_value);

			Condition condition;
			condition_init(&condition, IS_NULL, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
			selects->conditions[selects->condition_num++] = condition;
		}
#line 2583 "yacc_sql.tab.c"
    break;

  case 135: /* condition: ID DOT ID IS NULL_T  */
#line 870 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, (yyvsp[-4].string), (yyvsp[-2].string));

			Value *right_value = &CONTEXT->null_value;;
			value_init_null(right_value);

			Condition condition;
			condition_init(&condition, IS_NULL, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
			selects->conditions[selects->condition_num++] = condition;
		}
#line 2601 "yacc_sql.tab.c"
    break;

  case 136: /* condition: ID IS NOT NULL_T  */
#line 884 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, (yyvsp[-3].string));
			
			Value *right_value = &CONTEXT->null_value;;
			value_init_null(right_value);

			Condition condition;
			condition_init(&condition, IS_NOT_NULL, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
			selects->conditions[selects->condition_num++] = condition;
		}
#line 2619 "yacc_sql.tab.c"
    break;

  case 137: /* condition: ID DOT ID IS NOT NULL_T  */
#line 898 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, (yyvsp[-5].string), (yyvsp[-3].string));

			Value *right_value = &CONTEXT->null_value;;
			value_init_null(right_value);

			Condition condition;
			condition_init(&condition, IS_NOT_NULL, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
			selects->conditions[selects->condition_num++] = condition;
		}
#line 2637 "yacc_sql.tab.c"
    break;

  case 138: /* condition: sub_select_condition  */
#line 911 "yacc_sql.y"
                               {}
#line 2643 "yacc_sql.tab.c"
    break;

  case 139: /* condition: expr comOp expr  */
#line 912 "yacc_sql.y"
                          {
			Condition condition;
			expression_condition_init(&condition, CONTEXT->comp[CONTEXT->selects_num], (yyvsp[-2].exp), (yyvsp[0].exp));
			if (condition.right_exp != NULL && condition.left_exp->is_attr && condition.right_exp->is_attr) {
				condition.left_is_attr = 1;
				condition.left_attr = condition.left_exp->attr;
				free(condition.left_exp);
				condition.left_exp = NULL;
				condition.right_is_attr = 1;
				condition.right_attr = condition.right_exp->attr;
				free(condition.right_exp);
				condition.right_exp = NULL;
			}
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
			selects->conditions[selects->condition_num++] = condition;
	}
#line 2665 "yacc_sql.tab.c"
    break;

  case 140: /* expr: expr PLUS_S term  */
#line 932 "yacc_sql.y"
                         {
		(yyval.exp) = expression_create(NULL, NULL, (yyvsp[-2].exp), (yyvsp[0].exp), PLUS);
	}
#line 2673 "yacc_sql.tab.c"
    break;

  case 141: /* expr: expr MINUS_S term  */
#line 935 "yacc_sql.y"
                            {
		(yyval.exp) = expression_create(NULL, NULL, (yyvsp[-2].exp), (yyvsp[0].exp), MINUS);
	}
#line 2681 "yacc_sql.tab.c"
    break;

  case 142: /* expr: term  */
#line 938 "yacc_sql.y"
               {
		(yyval.exp) = (yyvsp[0].exp);
	}
#line 2689 "yacc_sql.tab.c"
    break;

  case 143: /* term: term STAR factor  */
#line 944 "yacc_sql.y"
                         {
		(yyval.exp) = expression_create(NULL, NULL, (yyvsp[-2].exp), (yyvsp[0].exp), MULT);
	}
#line 2697 "yacc_sql.tab.c"
    break;

  case 144: /* term: term SLASH factor  */
#line 947 "yacc_sql.y"
                            {
		(yyval.exp) = expression_create(NULL, NULL, (yyvsp[-2].exp), (yyvsp[0].exp), DIV);
	}
#line 2705 "yacc_sql.tab.c"
    break;

  case 145: /* term: factor  */
#line 950 "yacc_sql.y"
                 {
		(yyval.exp) = (yyvsp[0].exp);
	}
#line 2713 "yacc_sql.tab.c"
    break;

  case 146: /* factor: lbrace expr rbrace  */
#line 956 "yacc_sql.y"
                           {
		(yyval.exp) = (yyvsp[-1].exp);
	}
#line 2721 "yacc_sql.tab.c"
    break;

  case 147: /* factor: MINUS_S factor  */
#line 959 "yacc_sql.y"
                         {
		(yyval.exp) = expression_create(NULL, NULL, NULL, (yyvsp[0].exp), MINUS);
	}
#line 2729 "yacc_sql.tab.c"
    break;

  case 148: /* factor: base_value  */
#line 962 "yacc_sql.y"
                     {
		(yyval.exp) = (yyvsp[0].exp);
	}
#line 2737 "yacc_sql.tab.c"
    break;

  case 149: /* base_value: NUMBER  */
#line 968 "yacc_sql.y"
               {
		Value value;
  		value_init_integer(&value, (yyvsp[0].number));
		(yyval.exp) = expression_create(NULL, &value, NULL, NULL, NO_CAL_OP);
	}
#line 2747 "yacc_sql.tab.c"
    break;

  case 150: /* base_value: FLOAT  */
#line 973 "yacc_sql.y"
                {
		Value value;
  		value_init_float(&value, (yyvsp[0].floats));
		(yyval.exp) = expression_create(NULL, &value, NULL, NULL, NO_CAL_OP);
	}
#line 2757 "yacc_sql.tab.c"
    break;

  case 151: /* base_value: ID  */
#line 978 "yacc_sql.y"
             {
		RelAttr attr;
		relation_attr_init(&attr, NULL, (yyvsp[0].string));
		(yyval.exp) = expression_create(&attr, NULL, NULL, NULL, NO_CAL_OP);
	}
#line 2767 "yacc_sql.tab.c"
    break;

  case 152: /* base_value: ID DOT ID  */
#line 983 "yacc_sql.y"
                    {
		RelAttr attr;
		relation_attr_init(&attr, (yyvsp[-2].string), (yyvsp[0].string));
		(yyval.exp) = expression_create(&attr, NULL, NULL, NULL, NO_CAL_OP);
	}
#line 2777 "yacc_sql.tab.c"
    break;

  case 153: /* sub_select_condition: ID IN sub_select  */
#line 991 "yacc_sql.y"
                         {
		RelAttr left_attr;
		relation_attr_init(&left_attr, NULL, (yyvsp[-2].string));

		Condition condition;
		subquery_condition_init(&condition, &left_attr, CONTEXT->sub_selects, WHERE_IN);
		Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
		selects->conditions[selects->condition_num++] = condition;
	}
#line 2791 "yacc_sql.tab.c"
    break;

  case 154: /* sub_select_condition: ID DOT ID IN sub_select  */
#line 1000 "yacc_sql.y"
                                  {
		RelAttr left_attr;
		relation_attr_init(&left_attr, (yyvsp[-4].string), (yyvsp[-2].string));

		Condition condition;
		subquery_condition_init(&condition, &left_attr, CONTEXT->sub_selects, WHERE_IN);
		Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
		selects->conditions[selects->condition_num++] = condition;
	}
#line 2805 "yacc_sql.tab.c"
    break;

  case 155: /* sub_select_condition: ID NOT IN sub_select  */
#line 1009 "yacc_sql.y"
                               {
		RelAttr left_attr;
		relation_attr_init(&left_attr, NULL, (yyvsp[-3].string));

		Condition condition;
		subquery_condition_init(&condition, &left_attr, CONTEXT->sub_selects, NOT_IN);
		Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
		selects->conditions[selects->condition_num++] = condition;
	}
#line 2819 "yacc_sql.tab.c"
    break;

  case 156: /* sub_select_condition: ID DOT ID NOT IN sub_select  */
#line 1018 "yacc_sql.y"
                                      {
		RelAttr left_attr;
		relation_attr_init(&left_attr, (yyvsp[-5].string), (yyvsp[-3].string));

		Condition condition;
		subquery_condition_init(&condition, &left_attr, CONTEXT->sub_selects, NOT_IN);
		Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
		selects->conditions[selects->condition_num++] = condition;
	}
#line 2833 "yacc_sql.tab.c"
    break;

  case 157: /* sub_select_condition: ID comOp sub_select  */
#line 1027 "yacc_sql.y"
                              {
		RelAttr left_attr;
		relation_attr_init(&left_attr, NULL, (yyvsp[-2].string));

		Condition condition;
		subquery_condition_init(&condition, &left_attr, CONTEXT->sub_selects, CONTEXT->comp[CONTEXT->selects_num]);
		Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
		selects->conditions[selects->condition_num++] = condition;
	}
#line 2847 "yacc_sql.tab.c"
    break;

  case 158: /* sub_select_condition: ID DOT ID comOp sub_select  */
#line 1036 "yacc_sql.y"
                                     {
		RelAttr left_attr;
		relation_attr_init(&left_attr, (yyvsp[-4].string), (yyvsp[-2].string));

		Condition condition;
		subquery_condition_init(&condition, &left_attr, CONTEXT->sub_selects, CONTEXT->comp[CONTEXT->selects_num]);
		Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
		selects->conditions[selects->condition_num++] = condition;
	}
#line 2861 "yacc_sql.tab.c"
    break;

  case 159: /* sub_select_condition: sub_select comOp ID  */
#line 1045 "yacc_sql.y"
                              {
		RelAttr left_attr;
		relation_attr_init(&left_attr, NULL, (yyvsp[0].string));

		Condition condition;
		subquery_condition_init(&condition, &left_attr, CONTEXT->sub_selects, reverse(CONTEXT->comp[CONTEXT->selects_num]));
		Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
		selects->conditions[selects->condition_num++] = condition;
	}
#line 2875 "yacc_sql.tab.c"
    break;

  case 160: /* sub_select_condition: sub_select comOp ID DOT ID  */
#line 1054 "yacc_sql.y"
                                     {
		RelAttr left_attr;
		relation_attr_init(&left_attr, (yyvsp[-2].string), (yyvsp[0].string));

		Condition condition;
		subquery_condition_init(&condition, &left_attr, CONTEXT->sub_selects, reverse(CONTEXT->comp[CONTEXT->selects_num]));
		Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
		selects->conditions[selects->condition_num++] = condition;
	}
#line 2889 "yacc_sql.tab.c"
    break;

  case 161: /* comOp: EQ  */
#line 1066 "yacc_sql.y"
         { CONTEXT->comp[CONTEXT->selects_num] = EQUAL_TO; }
#line 2895 "yacc_sql.tab.c"
    break;

  case 162: /* comOp: LT  */
#line 1067 "yacc_sql.y"
         { CONTEXT->comp[CONTEXT->selects_num] = LESS_THAN; }
#line 2901 "yacc_sql.tab.c"
    break;

  case 163: /* comOp: GT  */
#line 1068 "yacc_sql.y"
         { CONTEXT->comp[CONTEXT->selects_num] = GREAT_THAN; }
#line 2907 "yacc_sql.tab.c"
    break;

  case 164: /* comOp: LE  */
#line 1069 "yacc_sql.y"
         { CONTEXT->comp[CONTEXT->selects_num] = LESS_EQUAL; }
#line 2913 "yacc_sql.tab.c"
    break;

  case 165: /* comOp: GE  */
#line 1070 "yacc_sql.y"
         { CONTEXT->comp[CONTEXT->selects_num] = GREAT_EQUAL; }
#line 2919 "yacc_sql.tab.c"
    break;

  case 166: /* comOp: NE  */
#line 1071 "yacc_sql.y"
         { CONTEXT->comp[CONTEXT->selects_num] = NOT_EQUAL; }
#line 2925 "yacc_sql.tab.c"
    break;

  case 167: /* load_data: LOAD DATA INFILE SSS INTO TABLE ID SEMICOLON  */
#line 1076 "yacc_sql.y"
                {
		  CONTEXT->ssql->flag = SCF_LOAD_DATA;
			load_data_init(&CONTEXT->ssql->sstr.load_data, (yyvsp[-1].string), (yyvsp[-4].string));
		}
#line 2934 "yacc_sql.tab.c"
    break;


#line 2938 "yacc_sql.tab.c"

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

#line 1081 "yacc_sql.y"

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
