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
  YYSYMBOL_str_value = 98,                 /* str_value  */
  YYSYMBOL_value = 99,                     /* value  */
  YYSYMBOL_delete = 100,                   /* delete  */
  YYSYMBOL_update = 101,                   /* update  */
  YYSYMBOL_select = 102,                   /* select  */
  YYSYMBOL_sub_select = 103,               /* sub_select  */
  YYSYMBOL_lbrace_select = 104,            /* lbrace_select  */
  YYSYMBOL_lbrace = 105,                   /* lbrace  */
  YYSYMBOL_rbrace = 106,                   /* rbrace  */
  YYSYMBOL_join_list = 107,                /* join_list  */
  YYSYMBOL_join_list_ = 108,               /* join_list_  */
  YYSYMBOL_inner_join = 109,               /* inner_join  */
  YYSYMBOL_select_attr = 110,              /* select_attr  */
  YYSYMBOL_agg_attr = 111,                 /* agg_attr  */
  YYSYMBOL_attr_list = 112,                /* attr_list  */
  YYSYMBOL_rel_list = 113,                 /* rel_list  */
  YYSYMBOL_where = 114,                    /* where  */
  YYSYMBOL_condition_list = 115,           /* condition_list  */
  YYSYMBOL_condition = 116,                /* condition  */
  YYSYMBOL_expr = 117,                     /* expr  */
  YYSYMBOL_term = 118,                     /* term  */
  YYSYMBOL_factor = 119,                   /* factor  */
  YYSYMBOL_base_value = 120,               /* base_value  */
  YYSYMBOL_sub_select_condition = 121,     /* sub_select_condition  */
  YYSYMBOL_comOp = 122,                    /* comOp  */
  YYSYMBOL_load_data = 123                 /* load_data  */
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
#define YYLAST   402

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  69
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  55
/* YYNRULES -- Number of rules.  */
#define YYNRULES  165
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  384

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
       0,   169,   169,   171,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   203,   208,   213,   219,   225,   231,   237,   243,
     249,   256,   263,   270,   277,   286,   288,   292,   303,   310,
     317,   329,   336,   345,   347,   349,   355,   356,   359,   360,
     363,   364,   365,   366,   367,   370,   378,   389,   390,   394,
     398,   399,   403,   406,   409,   413,   417,   423,   427,   431,
     437,   440,   443,   447,   451,   457,   467,   476,   489,   505,
     518,   531,   544,   551,   557,   566,   571,   572,   578,   579,
     583,   588,   593,   598,   603,   607,   612,   617,   622,   627,
     632,   637,   642,   647,   652,   657,   662,   667,   673,   675,
     680,   686,   692,   698,   704,   710,   716,   722,   728,   734,
     740,   746,   754,   756,   760,   762,   764,   766,   769,   780,
     798,   817,   829,   841,   855,   869,   883,   897,   898,   966,
     969,   972,   978,   981,   984,   990,   993,   996,  1002,  1007,
    1012,  1017,  1025,  1034,  1043,  1052,  1097,  1106,  1115,  1130,
    1131,  1132,  1133,  1134,  1135,  1139
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
  "value_list", "tuple_value", "str_value", "value", "delete", "update",
  "select", "sub_select", "lbrace_select", "lbrace", "rbrace", "join_list",
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

#define YYPACT_NINF (-290)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-75)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -290,   237,  -290,   204,   127,   130,   -32,    31,    89,    82,
      84,    76,   146,   154,   228,   232,   236,   172,  -290,  -290,
    -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,
    -290,  -290,  -290,  -290,  -290,  -290,  -290,   199,   205,   255,
     207,   214,   264,   264,   264,   264,   155,  -290,   268,   283,
     300,   301,  -290,   246,   247,   275,  -290,  -290,  -290,  -290,
    -290,   273,   264,   277,   252,   313,   314,  -290,   141,   161,
     208,   170,    96,   190,  -290,   256,  -290,  -290,  -290,   286,
     287,   257,   258,   259,   261,   288,  -290,  -290,    10,   308,
      16,   308,   266,  -290,    25,   308,   308,    92,   308,   264,
     264,   264,   264,   164,   283,   283,    90,   264,    66,   325,
     289,   298,  -290,   312,   271,   264,   269,  -290,   270,  -290,
    -290,   272,  -290,  -290,  -290,   276,  -290,  -290,  -290,   278,
    -290,  -290,   279,   280,   217,   281,   198,  -290,  -290,  -290,
     282,   284,  -290,   287,   285,   287,   315,   -26,   293,    27,
     294,   296,    -6,   297,   299,  -290,  -290,   243,   302,   243,
     140,    13,   303,   156,    19,  -290,  -290,  -290,  -290,   108,
     334,   259,   308,  -290,  -290,  -290,  -290,  -290,    24,   290,
     264,   308,   308,   308,   308,    93,   100,   122,   308,   308,
     142,   283,   283,   332,  -290,   351,   319,   354,   264,   355,
    -290,     8,  -290,  -290,  -290,  -290,  -290,   340,    32,   307,
     309,   331,    32,  -290,   304,   151,   306,   264,  -290,  -290,
    -290,  -290,  -290,  -290,   103,   200,    32,   249,   333,  -290,
    -290,  -290,     9,    66,  -290,    32,    32,   103,    32,    32,
    -290,  -290,  -290,   287,   305,   312,   361,   310,  -290,    94,
    -290,   350,   290,  -290,  -290,  -290,  -290,   311,   283,   316,
     283,   317,   283,   283,   283,   318,   283,  -290,  -290,  -290,
    -290,    66,  -290,   315,  -290,  -290,   -26,   308,   320,    62,
     321,  -290,   264,  -290,   362,  -290,  -290,  -290,  -290,   123,
     322,  -290,  -290,   123,   323,   324,   326,  -290,   303,    19,
      19,  -290,  -290,   123,  -290,  -290,   367,   369,  -290,  -290,
    -290,   308,   290,   308,   350,   308,  -290,   308,  -290,   308,
    -290,  -290,  -290,   308,  -290,   303,  -290,   340,  -290,  -290,
     245,   327,   264,  -290,  -290,  -290,   342,   287,   287,  -290,
    -290,  -290,   162,   350,   370,   308,   283,   283,   283,   283,
     193,  -290,   330,  -290,   264,  -290,   328,   308,   308,   335,
    -290,  -290,  -290,   375,  -290,  -290,  -290,  -290,  -290,   329,
    -290,  -290,   287,  -290,  -290,  -290,  -290,   345,   308,    66,
    -290,   303,   193,  -290
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
       0,     0,     0,     0,     0,     0,   108,    90,     0,   108,
       0,     0,    24,     0,     0,     0,    25,    26,    27,    23,
      22,     0,     0,     0,     0,     0,     0,    83,     0,     0,
       0,     0,     0,     0,    92,     0,    94,    30,    29,     0,
     124,     0,     0,     0,     0,     0,    28,    33,     0,     0,
       0,     0,     0,    46,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   108,   108,   108,   122,     0,     0,     0,
       0,     0,    55,    35,     0,     0,     0,    84,     0,    95,
      96,     0,    98,    99,    47,     0,   102,   101,   104,     0,
     105,   106,     0,     0,     0,     0,     0,   110,    93,    91,
       0,     0,    89,   124,     0,   124,    57,     0,    69,     0,
     148,   149,   150,    67,    68,    70,    71,     0,     0,     0,
       0,     0,   126,     0,   141,   144,   147,   137,    75,     0,
       0,     0,     0,    50,    51,    54,    52,    53,    40,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   108,   108,   122,    88,     0,     0,     0,     0,     0,
      66,     0,    48,    64,    65,    62,    63,    60,     0,   148,
     149,   150,     0,   146,     0,     0,     0,     0,   159,   160,
     161,   162,   163,   164,     0,     0,     0,     0,     0,    82,
     148,   149,     0,     0,   125,     0,     0,     0,     0,     0,
      74,    72,    73,   124,     0,    35,     0,     0,    42,     0,
      45,    43,     0,    97,   100,   103,   107,     0,   108,     0,
     108,     0,   108,   108,   108,     0,   108,   111,   109,   123,
      78,     0,    77,    57,    56,    49,     0,     0,     0,   151,
       0,   133,     0,   152,     0,    69,    67,    68,   128,   129,
       0,   131,   158,   156,     0,     0,     0,   145,   126,   139,
     140,   130,   157,   138,   143,   142,     0,     0,    36,    34,
      41,     0,     0,     0,    43,     0,   112,     0,   114,     0,
     117,   116,   119,     0,   120,   126,    58,    60,    59,   151,
       0,     0,     0,   135,   154,   132,     0,   124,   124,   127,
      76,   165,    37,    43,     0,     0,   108,   108,   108,   108,
      86,    61,     0,   134,     0,   153,     0,     0,     0,     0,
      39,    44,    31,     0,   113,   115,   118,   121,    85,     0,
     136,   155,   124,    79,    81,    38,    32,     0,     0,     0,
      80,   126,    86,    87
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,
    -290,  -290,  -290,  -290,  -290,  -290,   143,   222,  -289,  -231,
     -63,  -128,  -290,  -290,  -290,   121,   197,    69,   124,  -192,
     229,  -290,  -290,  -290,  -200,  -290,   -42,   -85,  -290,    15,
     295,  -290,   239,   -41,   209,  -129,  -275,  -218,  -100,    56,
    -131,  -290,  -290,  -107,  -290
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,   172,   113,   313,   251,
     155,   156,   178,   114,    32,   199,   146,   277,   207,   157,
     158,    33,    34,    35,   159,   160,   212,   119,   143,   368,
     369,    48,    49,    74,   145,   109,   234,   162,   163,   164,
     165,   166,   167,   224,    36
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      68,    69,    70,    71,   120,   122,   123,    96,    76,   126,
     127,   128,   130,   131,   195,   298,   197,   283,   213,   206,
      83,   314,   229,   339,   214,   345,   292,   117,   117,   200,
      67,    50,   288,   201,   117,    93,   202,   302,    51,   203,
     118,    67,   204,   117,    67,   301,   121,   215,   216,    67,
     350,   217,   226,   325,   361,   125,   237,   132,   133,   134,
     135,   232,   137,   138,   139,   147,   161,   235,   236,   124,
     275,   189,   208,   179,   230,   231,   211,   213,   247,   238,
     248,   343,   334,    67,   205,   239,   208,   246,   209,   210,
     211,   208,    52,   230,   231,   211,   253,   254,   255,   256,
     258,   260,   262,   263,   264,   266,   382,   304,   305,   140,
     117,   117,   232,    53,   306,   330,   331,    54,   117,   332,
      67,   148,   129,   257,   289,   149,   293,   150,   151,   152,
     259,   153,   355,    40,   154,    41,   249,   303,   252,    55,
     117,   141,   142,    99,   100,   101,   102,   297,   206,    56,
     267,   268,   261,    92,   371,    93,   147,    57,   285,   103,
     117,   381,   208,   240,   230,   231,   211,   201,   286,    93,
     202,   287,   265,   241,    72,   284,   242,    42,    43,    44,
      45,   235,   236,    72,   161,    73,   311,    42,    43,    44,
      45,   161,   328,    46,   136,   161,    47,   218,   219,   220,
     221,   222,   223,   227,    88,   280,   281,    89,   357,   358,
      37,    61,    38,   205,   235,   236,   359,   316,   360,   318,
      39,   320,   321,   322,    90,   324,   342,    91,   344,   161,
     346,    58,   347,    97,   348,    59,    98,     2,   349,    60,
     284,     3,     4,   378,   141,   142,     5,     6,     7,     8,
       9,    10,    11,   104,   290,   291,   105,    12,    13,    14,
     363,   191,    62,    64,   192,    15,    16,    92,    63,    93,
      65,    94,   373,   374,    95,    17,    92,    66,    93,   294,
     187,    67,   295,   188,   218,   219,   220,   221,   222,   223,
     284,   299,   300,   380,   173,   174,   175,   176,   177,   352,
     353,    75,    72,    77,    78,   364,   365,   366,   367,    79,
      80,    81,   284,    82,    84,    85,    86,    87,   107,   106,
     110,   108,   112,   111,   115,   116,   117,   124,   168,   170,
     169,   171,   180,   181,   198,   182,   194,   161,   233,   183,
     244,   184,   185,   186,   190,   193,   -74,   -46,   196,   -48,
     -72,   140,   -73,   250,   270,   225,   271,   272,   274,   276,
     -47,   278,   -49,   282,   309,   310,   296,   279,   307,   312,
     340,   229,   341,   362,   315,   356,   333,   335,   376,   317,
     319,   323,   379,   329,   354,   370,   336,   337,   308,   338,
     375,   372,   377,   245,   326,   273,   351,   383,   243,   228,
     327,   144,   269
};

static const yytype_int16 yycheck[] =
{
      42,    43,    44,    45,    89,    90,    91,    70,    49,    94,
      95,    96,    97,    98,   143,   233,   145,   217,   149,   147,
      62,   252,     9,   298,    30,   314,   226,    18,    18,    55,
      17,    63,   224,    59,    18,    61,    62,   237,     7,    65,
      30,    17,    68,    18,    17,   237,    30,    53,    54,    17,
     325,    57,   159,   271,   343,    30,   163,    99,   100,   101,
     102,   161,   103,   104,   105,   107,   108,    58,    59,    61,
      62,   134,    59,   115,    61,    62,    63,   208,    54,    60,
      56,   312,   282,    17,   147,    66,    59,   172,    61,    62,
      63,    59,     3,    61,    62,    63,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   381,   238,   239,    19,
      18,    18,   212,    31,   243,    53,    54,    33,    18,    57,
      17,    55,    30,    30,   224,    59,   226,    61,    62,    63,
      30,    65,   332,     6,    68,     8,   178,   237,   180,    63,
      18,    51,    52,    47,    48,    49,    50,   232,   276,     3,
     191,   192,    30,    59,   354,    61,   198,     3,    55,    63,
      18,   379,    59,    55,    61,    62,    63,    59,    65,    61,
      62,    68,    30,    65,    19,   217,    68,    47,    48,    49,
      50,    58,    59,    19,   226,    30,   249,    47,    48,    49,
      50,   233,   277,    63,    30,   237,    66,    41,    42,    43,
      44,    45,    46,    63,    63,    54,    55,    66,   337,   338,
       6,    39,     8,   276,    58,    59,    54,   258,    56,   260,
      16,   262,   263,   264,    63,   266,   311,    66,   313,   271,
     315,     3,   317,    63,   319,     3,    66,     0,   323,     3,
     282,     4,     5,   372,    51,    52,     9,    10,    11,    12,
      13,    14,    15,    63,    54,    55,    66,    20,    21,    22,
     345,    63,    63,     8,    66,    28,    29,    59,    63,    61,
      63,    63,   357,   358,    66,    38,    59,    63,    61,    30,
      63,    17,    33,    66,    41,    42,    43,    44,    45,    46,
     332,   235,   236,   378,    23,    24,    25,    26,    27,    54,
      55,    33,    19,     3,     3,   346,   347,   348,   349,    63,
      63,    36,   354,    40,    37,    63,     3,     3,    32,    63,
      63,    34,    63,    65,    63,    37,    18,    61,     3,    31,
      41,    19,    63,    63,    19,    63,    52,   379,    35,    63,
       6,    63,    63,    63,    63,    63,    53,    53,    63,    53,
      53,    19,    53,    63,     3,    53,    37,     3,     3,    19,
      53,    30,    53,    57,     3,    55,    33,    63,    63,    19,
       3,     9,     3,     3,    63,    33,    55,    55,     3,    63,
      63,    63,    37,    63,    57,    55,    63,    63,   245,    63,
      55,    63,    63,   171,   273,   198,   327,   382,   169,   160,
     276,   106,   193
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    70,     0,     4,     5,     9,    10,    11,    12,    13,
      14,    15,    20,    21,    22,    28,    29,    38,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    93,   100,   101,   102,   123,     6,     8,    16,
       6,     8,    47,    48,    49,    50,    63,    66,   110,   111,
      63,     7,     3,    31,    33,    63,     3,     3,     3,     3,
       3,    39,    63,    63,     8,    63,    63,    17,   105,   105,
     105,   105,    19,    30,   112,    33,   112,     3,     3,    63,
      63,    36,    40,   105,    37,    63,     3,     3,    63,    66,
      63,    66,    59,    61,    63,    66,    89,    63,    66,    47,
      48,    49,    50,    63,    63,    66,    63,    32,    34,   114,
      63,    65,    63,    86,    92,    63,    37,    18,    30,   106,
     106,    30,   106,   106,    61,    30,   106,   106,   106,    30,
     106,   106,   105,   105,   105,   105,    30,   112,   112,   112,
      19,    51,    52,   107,   109,   113,    95,   105,    55,    59,
      61,    62,    63,    65,    68,    89,    90,    98,    99,   103,
     104,   105,   116,   117,   118,   119,   120,   121,     3,    41,
      31,    19,    85,    23,    24,    25,    26,    27,    91,   105,
      63,    63,    63,    63,    63,    63,    63,    63,    66,    89,
      63,    63,    66,    63,    52,   114,    63,   114,    19,    94,
      55,    59,    62,    65,    68,    89,    90,    97,    59,    61,
      62,    63,   105,   119,    30,    53,    54,    57,    41,    42,
      43,    44,    45,    46,   122,    53,   122,    63,   111,     9,
      61,    62,   117,    35,   115,    58,    59,   122,    60,    66,
      55,    65,    68,    99,     6,    86,   106,    54,    56,   105,
      63,    88,   105,   106,   106,   106,   106,    30,   106,    30,
     106,    30,   106,   106,   106,    30,   106,   112,   112,   113,
       3,    37,     3,    95,     3,    62,    19,    96,    30,    63,
      54,    55,    57,   103,   105,    55,    65,    68,    98,   117,
      54,    55,   103,   117,    30,    33,    33,   106,   116,   118,
     118,    98,   103,   117,   119,   119,   114,    63,    85,     3,
      55,    89,    19,    87,    88,    63,   112,    63,   112,    63,
     112,   112,   112,    63,   112,   116,    94,    97,   106,    63,
      53,    54,    57,    55,   103,    55,    63,    63,    63,   115,
       3,     3,   106,    88,   106,    87,   106,   106,   106,   106,
     115,    96,    54,    55,    57,   103,    33,   114,   114,    54,
      56,    87,     3,   106,   112,   112,   112,   112,   108,   109,
      55,   103,    63,   106,   106,    55,     3,    63,   114,    37,
     106,   116,   115,   108
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
      99,    99,    99,    99,    99,   100,   101,   102,   102,   103,
     103,   103,   104,   105,   106,   107,   108,   108,   109,   109,
     110,   110,   110,   110,   110,   111,   111,   111,   111,   111,
     111,   111,   111,   111,   111,   111,   111,   111,   112,   112,
     112,   112,   112,   112,   112,   112,   112,   112,   112,   112,
     112,   112,   113,   113,   114,   114,   115,   115,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   117,
     117,   117,   118,   118,   118,   119,   119,   119,   120,   120,
     120,   120,   121,   121,   121,   121,   121,   121,   121,   122,
     122,   122,   122,   122,   122,   123
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
       1,     1,     1,     1,     1,     5,     8,     7,     7,     6,
       8,     6,     2,     1,     1,     6,     0,     6,     2,     1,
       1,     4,     2,     4,     2,     4,     4,     6,     4,     4,
       6,     4,     4,     6,     4,     4,     4,     6,     0,     5,
       3,     5,     6,     8,     6,     8,     6,     6,     8,     6,
       6,     8,     0,     3,     0,     3,     0,     3,     3,     3,
       3,     3,     4,     3,     5,     4,     6,     1,     3,     3,
       3,     1,     3,     3,     1,     3,     2,     1,     1,     1,
       1,     3,     3,     5,     4,     6,     3,     3,     3,     1,
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
  case 3: /* commands: commands command  */
#line 171 "yacc_sql.y"
                       {
        memset(CONTEXT->tuple_length, 0,  sizeof(int) * MAX_NUM);
        memset(CONTEXT->tuples, 0,  sizeof(Value) * MAX_NUM * MAX_NUM);
        memset(CONTEXT->values, 0,  sizeof(Value) * MAX_NUM);
        memset(CONTEXT->conditions, 0,  sizeof(Condition) * MAX_NUM);
        memset(CONTEXT->selects, 0,  sizeof(Selects) * MAX_NUM);
        CONTEXT->sub_selects = NULL;
    }
#line 1528 "yacc_sql.tab.c"
    break;

  case 22: /* exit: EXIT SEMICOLON  */
#line 203 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_EXIT;//"exit";
    }
#line 1536 "yacc_sql.tab.c"
    break;

  case 23: /* help: HELP SEMICOLON  */
#line 208 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_HELP;//"help";
    }
#line 1544 "yacc_sql.tab.c"
    break;

  case 24: /* sync: SYNC SEMICOLON  */
#line 213 "yacc_sql.y"
                   {
      CONTEXT->ssql->flag = SCF_SYNC;
    }
#line 1552 "yacc_sql.tab.c"
    break;

  case 25: /* begin: TRX_BEGIN SEMICOLON  */
#line 219 "yacc_sql.y"
                        {
      CONTEXT->ssql->flag = SCF_BEGIN;
    }
#line 1560 "yacc_sql.tab.c"
    break;

  case 26: /* commit: TRX_COMMIT SEMICOLON  */
#line 225 "yacc_sql.y"
                         {
      CONTEXT->ssql->flag = SCF_COMMIT;
    }
#line 1568 "yacc_sql.tab.c"
    break;

  case 27: /* rollback: TRX_ROLLBACK SEMICOLON  */
#line 231 "yacc_sql.y"
                           {
      CONTEXT->ssql->flag = SCF_ROLLBACK;
    }
#line 1576 "yacc_sql.tab.c"
    break;

  case 28: /* drop_table: DROP TABLE ID SEMICOLON  */
#line 237 "yacc_sql.y"
                            {
        CONTEXT->ssql->flag = SCF_DROP_TABLE;//"drop_table";
				drop_table_init(&CONTEXT->ssql->sstr.drop_table, (yyvsp[-1].string));
    }
#line 1585 "yacc_sql.tab.c"
    break;

  case 29: /* show_tables: SHOW TABLES SEMICOLON  */
#line 243 "yacc_sql.y"
                          {
      CONTEXT->ssql->flag = SCF_SHOW_TABLES;
    }
#line 1593 "yacc_sql.tab.c"
    break;

  case 30: /* desc_table: DESC ID SEMICOLON  */
#line 249 "yacc_sql.y"
                      {
      CONTEXT->ssql->flag = SCF_DESC_TABLE;
			desc_table_init(&CONTEXT->ssql->sstr.desc_table, (yyvsp[-1].string));
    }
#line 1602 "yacc_sql.tab.c"
    break;

  case 31: /* create_index: CREATE INDEX ID ON ID lbrace index_attr_def index_attr_def_list rbrace SEMICOLON  */
#line 257 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, (yyvsp[-7].string), (yyvsp[-5].string));
		}
#line 1611 "yacc_sql.tab.c"
    break;

  case 32: /* create_unique_index: CREATE UNIQUE INDEX ID ON ID lbrace index_attr_def index_attr_def_list rbrace SEMICOLON  */
#line 264 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_CREATE_UNIQUE_INDEX;//"create_unique_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, (yyvsp[-7].string), (yyvsp[-5].string));
		}
#line 1620 "yacc_sql.tab.c"
    break;

  case 33: /* drop_index: DROP INDEX ID SEMICOLON  */
#line 271 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_DROP_INDEX;//"drop_index";
			drop_index_init(&CONTEXT->ssql->sstr.drop_index, (yyvsp[-1].string));
		}
#line 1629 "yacc_sql.tab.c"
    break;

  case 34: /* create_table: CREATE TABLE ID lbrace attr_def attr_def_list rbrace SEMICOLON  */
#line 278 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_CREATE_TABLE;//"create_table";
			// CONTEXT->ssql->sstr.create_table.attribute_count = CONTEXT->value_length; // TODO
			create_table_init_name(&CONTEXT->ssql->sstr.create_table, (yyvsp[-5].string));
			//临时变量清零	
			CONTEXT->value_length = 0;
		}
#line 1641 "yacc_sql.tab.c"
    break;

  case 36: /* attr_def_list: COMMA attr_def attr_def_list  */
#line 288 "yacc_sql.y"
                                   {    }
#line 1647 "yacc_sql.tab.c"
    break;

  case 37: /* attr_def: ID_get type lbrace number rbrace  */
#line 293 "yacc_sql.y"
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
#line 1662 "yacc_sql.tab.c"
    break;

  case 38: /* attr_def: ID_get type lbrace number rbrace NOT NULL_T  */
#line 304 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-5].number), (yyvsp[-3].number), 0);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
		}
#line 1673 "yacc_sql.tab.c"
    break;

  case 39: /* attr_def: ID_get type lbrace number rbrace NULLABLE  */
#line 311 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-4].number), (yyvsp[-2].number), 1);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
		}
#line 1684 "yacc_sql.tab.c"
    break;

  case 40: /* attr_def: ID_get type  */
#line 318 "yacc_sql.y"
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
#line 1700 "yacc_sql.tab.c"
    break;

  case 41: /* attr_def: ID_get type NOT NULL_T  */
#line 330 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-2].number), (yyvsp[-2].number) == TEXTS ? 8 : 4, 0);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
		}
#line 1711 "yacc_sql.tab.c"
    break;

  case 42: /* attr_def: ID_get type NULLABLE  */
#line 337 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-1].number), (yyvsp[-1].number) == TEXTS ? 8 : 4, 1);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
		}
#line 1722 "yacc_sql.tab.c"
    break;

  case 44: /* index_attr_def_list: COMMA index_attr_def index_attr_def_list  */
#line 347 "yacc_sql.y"
                                                   {    }
#line 1728 "yacc_sql.tab.c"
    break;

  case 45: /* index_attr_def: ID  */
#line 350 "yacc_sql.y"
                {
			create_index_append_attribute(&CONTEXT->ssql->sstr.create_index, (yyvsp[0].string));
			CONTEXT->value_length++;
		}
#line 1737 "yacc_sql.tab.c"
    break;

  case 46: /* number: NUMBER  */
#line 355 "yacc_sql.y"
               {(yyval.number) = (yyvsp[0].number);}
#line 1743 "yacc_sql.tab.c"
    break;

  case 47: /* number: MINUS_S NUMBER  */
#line 356 "yacc_sql.y"
                         {(yyval.number) = -(yyvsp[0].number);}
#line 1749 "yacc_sql.tab.c"
    break;

  case 48: /* float: FLOAT  */
#line 359 "yacc_sql.y"
              {(yyval.floats) = (yyvsp[0].floats);}
#line 1755 "yacc_sql.tab.c"
    break;

  case 49: /* float: MINUS_S FLOAT  */
#line 360 "yacc_sql.y"
                        {(yyval.floats) = -(yyvsp[0].floats);}
#line 1761 "yacc_sql.tab.c"
    break;

  case 50: /* type: INT_T  */
#line 363 "yacc_sql.y"
              { (yyval.number)=INTS; }
#line 1767 "yacc_sql.tab.c"
    break;

  case 51: /* type: STRING_T  */
#line 364 "yacc_sql.y"
                  { (yyval.number)=CHARS; }
#line 1773 "yacc_sql.tab.c"
    break;

  case 52: /* type: FLOAT_T  */
#line 365 "yacc_sql.y"
                 { (yyval.number)=FLOATS; }
#line 1779 "yacc_sql.tab.c"
    break;

  case 53: /* type: DATE_T  */
#line 366 "yacc_sql.y"
                    { (yyval.number)=DATES; }
#line 1785 "yacc_sql.tab.c"
    break;

  case 54: /* type: TEXT_T  */
#line 367 "yacc_sql.y"
                    { (yyval.number)=TEXTS; }
#line 1791 "yacc_sql.tab.c"
    break;

  case 55: /* ID_get: ID  */
#line 371 "yacc_sql.y"
        {
		char *temp=(yyvsp[0].string); 
		snprintf(CONTEXT->id, sizeof(CONTEXT->id), "%s", temp);
	}
#line 1800 "yacc_sql.tab.c"
    break;

  case 56: /* insert: INSERT INTO ID VALUES tuple tuple_list SEMICOLON  */
#line 379 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_INSERT;
			inserts_init(&CONTEXT->ssql->sstr.insertion, (yyvsp[-4].string), CONTEXT->tuples, CONTEXT->tuple_num, CONTEXT->tuple_length);
      		//临时变量清零
      		for (int i = 0; i < CONTEXT->tuple_num; i++) {
				CONTEXT->tuple_length[i] = 0;
			}
			CONTEXT->tuple_num = 0;
    }
#line 1814 "yacc_sql.tab.c"
    break;

  case 58: /* tuple_list: COMMA tuple tuple_list  */
#line 390 "yacc_sql.y"
                                 {
	}
#line 1821 "yacc_sql.tab.c"
    break;

  case 59: /* tuple: lbrace tuple_value value_list rbrace  */
#line 394 "yacc_sql.y"
                                             {
		CONTEXT->tuple_num++;
	}
#line 1829 "yacc_sql.tab.c"
    break;

  case 61: /* value_list: COMMA tuple_value value_list  */
#line 399 "yacc_sql.y"
                                    { 
	  	}
#line 1836 "yacc_sql.tab.c"
    break;

  case 62: /* tuple_value: number  */
#line 403 "yacc_sql.y"
           {
  			value_init_integer(&CONTEXT->tuples[CONTEXT->tuple_num][CONTEXT->tuple_length[CONTEXT->tuple_num]++], (yyvsp[0].number));
		}
#line 1844 "yacc_sql.tab.c"
    break;

  case 63: /* tuple_value: float  */
#line 406 "yacc_sql.y"
            {
  			value_init_float(&CONTEXT->tuples[CONTEXT->tuple_num][CONTEXT->tuple_length[CONTEXT->tuple_num]++], (yyvsp[0].floats));
		}
#line 1852 "yacc_sql.tab.c"
    break;

  case 64: /* tuple_value: SSS  */
#line 409 "yacc_sql.y"
         {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
  			value_init_string(&CONTEXT->tuples[CONTEXT->tuple_num][CONTEXT->tuple_length[CONTEXT->tuple_num]++], (yyvsp[0].string));
		}
#line 1861 "yacc_sql.tab.c"
    break;

  case 65: /* tuple_value: DATE  */
#line 413 "yacc_sql.y"
              {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
			value_init_date(&CONTEXT->tuples[CONTEXT->tuple_num][CONTEXT->tuple_length[CONTEXT->tuple_num]++], (yyvsp[0].string));
		}
#line 1870 "yacc_sql.tab.c"
    break;

  case 66: /* tuple_value: NULL_T  */
#line 417 "yacc_sql.y"
                {
			value_init_null(&CONTEXT->tuples[CONTEXT->tuple_num][CONTEXT->tuple_length[CONTEXT->tuple_num]++]);
		}
#line 1878 "yacc_sql.tab.c"
    break;

  case 67: /* str_value: SSS  */
#line 423 "yacc_sql.y"
            {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
  			value_init_string(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].string));
		}
#line 1887 "yacc_sql.tab.c"
    break;

  case 68: /* str_value: DATE  */
#line 427 "yacc_sql.y"
              {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
			value_init_date(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].string));
		}
#line 1896 "yacc_sql.tab.c"
    break;

  case 69: /* str_value: NULL_T  */
#line 431 "yacc_sql.y"
                {
			value_init_null(&CONTEXT->values[CONTEXT->value_length++]);
		}
#line 1904 "yacc_sql.tab.c"
    break;

  case 70: /* value: number  */
#line 437 "yacc_sql.y"
          {	
  		value_init_integer(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].number));
		}
#line 1912 "yacc_sql.tab.c"
    break;

  case 71: /* value: float  */
#line 440 "yacc_sql.y"
            {
  		value_init_float(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].floats));
		}
#line 1920 "yacc_sql.tab.c"
    break;

  case 72: /* value: SSS  */
#line 443 "yacc_sql.y"
         {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
  		value_init_string(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].string));
		}
#line 1929 "yacc_sql.tab.c"
    break;

  case 73: /* value: DATE  */
#line 447 "yacc_sql.y"
              {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
			value_init_date(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].string));
		}
#line 1938 "yacc_sql.tab.c"
    break;

  case 74: /* value: NULL_T  */
#line 451 "yacc_sql.y"
                {
			value_init_null(&CONTEXT->values[CONTEXT->value_length++]);
		}
#line 1946 "yacc_sql.tab.c"
    break;

  case 75: /* delete: DELETE FROM ID where SEMICOLON  */
#line 458 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_DELETE;//"delete";
			deletes_init_relation(&CONTEXT->ssql->sstr.deletion, (yyvsp[-2].string));
			deletes_set_conditions(&CONTEXT->ssql->sstr.deletion, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;	
    }
#line 1958 "yacc_sql.tab.c"
    break;

  case 76: /* update: UPDATE ID SET ID EQ value where SEMICOLON  */
#line 468 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_UPDATE;//"update";
			Value *value = &CONTEXT->values[0];
			updates_init(&CONTEXT->ssql->sstr.update, (yyvsp[-6].string), (yyvsp[-4].string), value, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;
		}
#line 1970 "yacc_sql.tab.c"
    break;

  case 77: /* select: SELECT select_attr FROM ID rel_list where SEMICOLON  */
#line 476 "yacc_sql.y"
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
#line 1988 "yacc_sql.tab.c"
    break;

  case 78: /* select: SELECT select_attr FROM ID join_list where SEMICOLON  */
#line 489 "yacc_sql.y"
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
#line 2006 "yacc_sql.tab.c"
    break;

  case 79: /* sub_select: lbrace_select ID FROM ID where rbrace  */
#line 505 "yacc_sql.y"
                                         {
		Selects *sub_selects = (Selects *)malloc(sizeof(Selects));
		*sub_selects = CONTEXT->selects[CONTEXT->selects_num + 1];
		memset(&CONTEXT->selects[CONTEXT->selects_num + 1], 0, sizeof(Selects));
		
		RelAttr left_attr;
		relation_attr_init(&left_attr, (yyvsp[-2].string), (yyvsp[-4].string));
		selects_append_attribute(sub_selects, &left_attr);
		selects_append_relation(sub_selects, (yyvsp[-2].string));

		CONTEXT->sub_selects = sub_selects;
		(yyval.selects) = sub_selects;
    }
#line 2024 "yacc_sql.tab.c"
    break;

  case 80: /* sub_select: lbrace_select ID DOT ID FROM ID where rbrace  */
#line 518 "yacc_sql.y"
                                                   {
		Selects *sub_selects = (Selects *)malloc(sizeof(Selects));
		*sub_selects = CONTEXT->selects[CONTEXT->selects_num + 1];
		memset(&CONTEXT->selects[CONTEXT->selects_num + 1], 0, sizeof(Selects));
		
		RelAttr left_attr;
		relation_attr_init(&left_attr, (yyvsp[-6].string), (yyvsp[-4].string));
		selects_append_attribute(sub_selects, &left_attr);
		selects_append_relation(sub_selects, (yyvsp[-2].string));

		CONTEXT->sub_selects = sub_selects;
		(yyval.selects) = sub_selects;
    }
#line 2042 "yacc_sql.tab.c"
    break;

  case 81: /* sub_select: lbrace_select agg_attr FROM ID where rbrace  */
#line 531 "yacc_sql.y"
                                                  {
		Selects *sub_selects = (Selects *)malloc(sizeof(Selects));
		*sub_selects = CONTEXT->selects[CONTEXT->selects_num + 1];
		memset(&CONTEXT->selects[CONTEXT->selects_num + 1], 0, sizeof(Selects));

		selects_append_relation(sub_selects, (yyvsp[-2].string));

		CONTEXT->sub_selects = sub_selects;
		(yyval.selects) = sub_selects;
    }
#line 2057 "yacc_sql.tab.c"
    break;

  case 82: /* lbrace_select: lbrace SELECT  */
#line 544 "yacc_sql.y"
                      {
		CONTEXT->selects_num ++;
		CONTEXT->lbrace_selects[CONTEXT->selects_num] = CONTEXT->lbrace_num;
	}
#line 2066 "yacc_sql.tab.c"
    break;

  case 83: /* lbrace: LBRACE  */
#line 551 "yacc_sql.y"
               {
		CONTEXT->lbrace_num ++;
	}
#line 2074 "yacc_sql.tab.c"
    break;

  case 84: /* rbrace: RBRACE  */
#line 557 "yacc_sql.y"
               {
		if (CONTEXT->lbrace_selects[CONTEXT->selects_num] == CONTEXT->lbrace_num && CONTEXT->selects_num) {
			CONTEXT->selects_num --;
		}
		CONTEXT->lbrace_num --;
	}
#line 2085 "yacc_sql.tab.c"
    break;

  case 85: /* join_list: inner_join ID ON condition condition_list join_list_  */
#line 566 "yacc_sql.y"
                                                             {
		selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-4].string));
		selects_append_relation(&CONTEXT->selects[CONTEXT->selects_num], (yyvsp[-4].string));
	}
#line 2094 "yacc_sql.tab.c"
    break;

  case 87: /* join_list_: inner_join ID ON condition condition_list join_list_  */
#line 572 "yacc_sql.y"
                                                               {
		selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-4].string));
		selects_append_relation(&CONTEXT->selects[CONTEXT->selects_num], (yyvsp[-4].string));
	}
#line 2103 "yacc_sql.tab.c"
    break;

  case 88: /* inner_join: INNER JOIN  */
#line 578 "yacc_sql.y"
                   {}
#line 2109 "yacc_sql.tab.c"
    break;

  case 89: /* inner_join: JOIN  */
#line 579 "yacc_sql.y"
               {}
#line 2115 "yacc_sql.tab.c"
    break;

  case 90: /* select_attr: STAR  */
#line 583 "yacc_sql.y"
         {  
			RelAttr attr;
			relation_attr_init(&attr, NULL, "*");
			selects_append_attribute(&CONTEXT->selects[CONTEXT->selects_num], &attr);
		}
#line 2125 "yacc_sql.tab.c"
    break;

  case 91: /* select_attr: ID DOT STAR attr_list  */
#line 588 "yacc_sql.y"
                                {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), "*");
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 2135 "yacc_sql.tab.c"
    break;

  case 92: /* select_attr: ID attr_list  */
#line 593 "yacc_sql.y"
                   {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->selects[CONTEXT->selects_num], &attr);
		}
#line 2145 "yacc_sql.tab.c"
    break;

  case 93: /* select_attr: ID DOT ID attr_list  */
#line 598 "yacc_sql.y"
                              {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->selects[CONTEXT->selects_num], &attr);
		}
#line 2155 "yacc_sql.tab.c"
    break;

  case 94: /* select_attr: agg_attr attr_list  */
#line 603 "yacc_sql.y"
                             {}
#line 2161 "yacc_sql.tab.c"
    break;

  case 95: /* agg_attr: MAX lbrace ID rbrace  */
#line 607 "yacc_sql.y"
                             {
			AggOp agg;
			aggregation_init_string(&agg, NULL, (yyvsp[-1].string), MAX_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2171 "yacc_sql.tab.c"
    break;

  case 96: /* agg_attr: MAX lbrace STAR rbrace  */
#line 612 "yacc_sql.y"
                                 {
			AggOp agg;
			aggregation_init_string(&agg, NULL, "*", MAX_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2181 "yacc_sql.tab.c"
    break;

  case 97: /* agg_attr: MAX lbrace ID DOT ID rbrace  */
#line 617 "yacc_sql.y"
                                      {
			AggOp agg;
			aggregation_init_string(&agg, (yyvsp[-3].string), (yyvsp[-1].string), MAX_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2191 "yacc_sql.tab.c"
    break;

  case 98: /* agg_attr: MIN lbrace ID rbrace  */
#line 622 "yacc_sql.y"
                               {
			AggOp agg;
			aggregation_init_string(&agg, NULL, (yyvsp[-1].string), MIN_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2201 "yacc_sql.tab.c"
    break;

  case 99: /* agg_attr: MIN lbrace STAR rbrace  */
#line 627 "yacc_sql.y"
                                 {
			AggOp agg;
			aggregation_init_string(&agg, NULL, "*", MIN_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2211 "yacc_sql.tab.c"
    break;

  case 100: /* agg_attr: MIN lbrace ID DOT ID rbrace  */
#line 632 "yacc_sql.y"
                                      {
			AggOp agg;
			aggregation_init_string(&agg, (yyvsp[-3].string), (yyvsp[-1].string), MIN_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2221 "yacc_sql.tab.c"
    break;

  case 101: /* agg_attr: COUNT lbrace STAR rbrace  */
#line 637 "yacc_sql.y"
                                   {
			AggOp agg;
			aggregation_init_string(&agg, NULL, "*", COUNT_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2231 "yacc_sql.tab.c"
    break;

  case 102: /* agg_attr: COUNT lbrace ID rbrace  */
#line 642 "yacc_sql.y"
                                 {
			AggOp agg;
			aggregation_init_string(&agg, NULL, (yyvsp[-1].string), COUNT_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2241 "yacc_sql.tab.c"
    break;

  case 103: /* agg_attr: COUNT lbrace ID DOT ID rbrace  */
#line 647 "yacc_sql.y"
                                        {
			AggOp agg;
			aggregation_init_string(&agg, (yyvsp[-3].string), (yyvsp[-1].string), COUNT_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2251 "yacc_sql.tab.c"
    break;

  case 104: /* agg_attr: COUNT lbrace number rbrace  */
#line 652 "yacc_sql.y"
                                     {  //  TODO optimize count(n)
			AggOp agg;
			aggregation_init_integer(&agg, NULL, (yyvsp[-1].number), COUNT_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2261 "yacc_sql.tab.c"
    break;

  case 105: /* agg_attr: AVG lbrace ID rbrace  */
#line 657 "yacc_sql.y"
                               {
			AggOp agg;
			aggregation_init_string(&agg, NULL, (yyvsp[-1].string), AVG_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2271 "yacc_sql.tab.c"
    break;

  case 106: /* agg_attr: AVG lbrace STAR rbrace  */
#line 662 "yacc_sql.y"
                                 {
			AggOp agg;
			aggregation_init_string(&agg, NULL, "*", AVG_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2281 "yacc_sql.tab.c"
    break;

  case 107: /* agg_attr: AVG lbrace ID DOT ID rbrace  */
#line 667 "yacc_sql.y"
                                      {
			AggOp agg;
			aggregation_init_string(&agg, (yyvsp[-3].string), (yyvsp[-1].string), AVG_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2291 "yacc_sql.tab.c"
    break;

  case 109: /* attr_list: COMMA ID DOT STAR attr_list  */
#line 675 "yacc_sql.y"
                                      {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), "*");
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 2301 "yacc_sql.tab.c"
    break;

  case 110: /* attr_list: COMMA ID attr_list  */
#line 680 "yacc_sql.y"
                         {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			selects_append_attribute(&CONTEXT->selects[CONTEXT->selects_num], &attr);
      }
#line 2312 "yacc_sql.tab.c"
    break;

  case 111: /* attr_list: COMMA ID DOT ID attr_list  */
#line 686 "yacc_sql.y"
                                {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			selects_append_attribute(&CONTEXT->selects[CONTEXT->selects_num], &attr);
  	  }
#line 2323 "yacc_sql.tab.c"
    break;

  case 112: /* attr_list: COMMA MAX lbrace ID rbrace attr_list  */
#line 692 "yacc_sql.y"
                                               {
			AggOp agg;
			aggregation_init_string(&agg, NULL, (yyvsp[-2].string), MAX_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2334 "yacc_sql.tab.c"
    break;

  case 113: /* attr_list: COMMA MAX lbrace ID DOT ID rbrace attr_list  */
#line 698 "yacc_sql.y"
                                                      {
			AggOp agg;
			aggregation_init_string(&agg, (yyvsp[-4].string), (yyvsp[-2].string), MAX_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2345 "yacc_sql.tab.c"
    break;

  case 114: /* attr_list: COMMA MIN lbrace ID rbrace attr_list  */
#line 704 "yacc_sql.y"
                                               {
			AggOp agg;
			aggregation_init_string(&agg, NULL, (yyvsp[-2].string), MIN_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2356 "yacc_sql.tab.c"
    break;

  case 115: /* attr_list: COMMA MIN lbrace ID DOT ID rbrace attr_list  */
#line 710 "yacc_sql.y"
                                                      {
			AggOp agg;
			aggregation_init_string(&agg, (yyvsp[-4].string), (yyvsp[-2].string), MIN_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2367 "yacc_sql.tab.c"
    break;

  case 116: /* attr_list: COMMA COUNT lbrace STAR rbrace attr_list  */
#line 716 "yacc_sql.y"
                                                   { // count(*) 忽略null记录
			AggOp agg;
			aggregation_init_string(&agg, NULL, "*", COUNT_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2378 "yacc_sql.tab.c"
    break;

  case 117: /* attr_list: COMMA COUNT lbrace ID rbrace attr_list  */
#line 722 "yacc_sql.y"
                                                 {
			AggOp agg;
			aggregation_init_string(&agg, NULL, (yyvsp[-2].string), COUNT_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2389 "yacc_sql.tab.c"
    break;

  case 118: /* attr_list: COMMA COUNT lbrace ID DOT ID rbrace attr_list  */
#line 728 "yacc_sql.y"
                                                        {
			AggOp agg;
			aggregation_init_string(&agg, (yyvsp[-4].string), (yyvsp[-2].string), COUNT_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2400 "yacc_sql.tab.c"
    break;

  case 119: /* attr_list: COMMA COUNT lbrace number rbrace attr_list  */
#line 734 "yacc_sql.y"
                                                     {  //  TODO optimize count(n) // conut(n) 不忽略null记录
			AggOp agg;
			aggregation_init_integer(&agg, NULL, (yyvsp[-2].number), COUNT_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2411 "yacc_sql.tab.c"
    break;

  case 120: /* attr_list: COMMA AVG lbrace ID rbrace attr_list  */
#line 740 "yacc_sql.y"
                                               {
			AggOp agg;
			aggregation_init_string(&agg, NULL, (yyvsp[-2].string), AVG_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2422 "yacc_sql.tab.c"
    break;

  case 121: /* attr_list: COMMA AVG lbrace ID DOT ID rbrace attr_list  */
#line 746 "yacc_sql.y"
                                                      {
			AggOp agg;
			aggregation_init_string(&agg, (yyvsp[-4].string), (yyvsp[-2].string), AVG_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
#line 2433 "yacc_sql.tab.c"
    break;

  case 123: /* rel_list: COMMA ID rel_list  */
#line 756 "yacc_sql.y"
                        {	
				selects_append_relation(&CONTEXT->selects[CONTEXT->selects_num], (yyvsp[-1].string));
		  }
#line 2441 "yacc_sql.tab.c"
    break;

  case 125: /* where: WHERE condition condition_list  */
#line 762 "yacc_sql.y"
                                     {}
#line 2447 "yacc_sql.tab.c"
    break;

  case 127: /* condition_list: AND condition condition_list  */
#line 766 "yacc_sql.y"
                                   {}
#line 2453 "yacc_sql.tab.c"
    break;

  case 128: /* condition: str_value comOp str_value  */
#line 770 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 2];
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp[CONTEXT->selects_num], 0, NULL, left_value, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
			selects->conditions[selects->condition_num++] = condition;
		}
#line 2468 "yacc_sql.tab.c"
    break;

  case 129: /* condition: str_value comOp expr  */
#line 781 "yacc_sql.y"
                {
			CalExp *exp = (yyvsp[0].exp);
			Value *value = &CONTEXT->values[CONTEXT->value_length - 1];
			Condition condition;
			if(exp->cal_op == NO_CAL_OP){
				if(exp->is_attr == 1){
					condition_init(&condition, CONTEXT->comp[CONTEXT->selects_num], 0, NULL, value, 1, &exp->attr , NULL);
				}else{
					condition_init(&condition, CONTEXT->comp[CONTEXT->selects_num], 0, NULL, value, 0, NULL, &exp->value);
				}
				CONTEXT->conditions[CONTEXT->condition_length++] = condition;
				Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
				selects->conditions[selects->condition_num++] = condition;	
			}else{
				//错误格式
			}
		}
#line 2490 "yacc_sql.tab.c"
    break;

  case 130: /* condition: expr comOp str_value  */
#line 799 "yacc_sql.y"
                {
			CalExp *exp = (yyvsp[-2].exp);
			Value *value = &CONTEXT->values[CONTEXT->value_length - 1];
			Condition condition;
			if(exp->cal_op == NO_CAL_OP){
				if(exp->is_attr == 1){
					condition_init(&condition, CONTEXT->comp[CONTEXT->selects_num], 0, NULL, value, 1, &exp->attr , NULL);
				}else{
					condition_init(&condition, CONTEXT->comp[CONTEXT->selects_num], 0, NULL, value, 0, NULL, &exp->value);
				}
				CONTEXT->conditions[CONTEXT->condition_length++] = condition;
				Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
				selects->conditions[selects->condition_num++] = condition;	
			}else{
				//错误格式
			}
		}
#line 2512 "yacc_sql.tab.c"
    break;

  case 131: /* condition: value IS NULL_T  */
#line 818 "yacc_sql.y"
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
#line 2528 "yacc_sql.tab.c"
    break;

  case 132: /* condition: value IS NOT NULL_T  */
#line 830 "yacc_sql.y"
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
#line 2544 "yacc_sql.tab.c"
    break;

  case 133: /* condition: ID IS NULL_T  */
#line 842 "yacc_sql.y"
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
#line 2562 "yacc_sql.tab.c"
    break;

  case 134: /* condition: ID DOT ID IS NULL_T  */
#line 856 "yacc_sql.y"
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
#line 2580 "yacc_sql.tab.c"
    break;

  case 135: /* condition: ID IS NOT NULL_T  */
#line 870 "yacc_sql.y"
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
#line 2598 "yacc_sql.tab.c"
    break;

  case 136: /* condition: ID DOT ID IS NOT NULL_T  */
#line 884 "yacc_sql.y"
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
#line 2616 "yacc_sql.tab.c"
    break;

  case 137: /* condition: sub_select_condition  */
#line 897 "yacc_sql.y"
                               {}
#line 2622 "yacc_sql.tab.c"
    break;

  case 138: /* condition: expr comOp expr  */
#line 898 "yacc_sql.y"
                          {
			Condition condition;
			expression_condition_init(&condition, CONTEXT->comp[CONTEXT->selects_num], (yyvsp[-2].exp), (yyvsp[0].exp));
			if (condition.right_exp != NULL){
				int right_flag = 0;
				int left_flag = 0;
				if(condition.right_exp->is_attr == 1){
					right_flag = 1; //attr
				}else if(condition.right_exp->cal_op == NO_CAL_OP){
					right_flag = 2; //number >= 0
				}else if(condition.right_exp->cal_op == MINUS && condition.right_exp->left_exp == NULL){
					if(condition.right_exp->right_exp->cal_op == NO_CAL_OP && condition.right_exp->right_exp->is_attr == 0){
						right_flag = 3; //number < 0
					}
				}
				if(condition.left_exp->is_attr == 1){
					left_flag = 1; //attr
				}else if(condition.left_exp->cal_op == NO_CAL_OP){
					left_flag = 2; //>0
				}else if(condition.left_exp->cal_op == MINUS && condition.left_exp->left_exp == NULL){
					if(condition.left_exp->right_exp->cal_op == NO_CAL_OP && condition.left_exp->right_exp->is_attr == 0){
						left_flag = 3; //number < 0
					}
				}
				if(left_flag > 0 && right_flag > 0){
					if(left_flag == 1){
						condition.left_is_attr = 1;
						condition.left_attr = condition.left_exp->attr;
					}else if(left_flag == 2){
						condition.left_is_attr = 0;
						condition.left_value = condition.left_exp->value;
					}else{
						condition.left_is_attr = 0;
						condition.left_value = condition.left_exp->value;
						if(condition.left_value.type == FLOATS){
							*((float*)(condition.left_value.data)) = -*(float*)(condition.left_value.data);
						}else{
							*((int*)(condition.left_value.data)) = -*(int*)(condition.left_value.data);
						}
					}
					if(right_flag == 1){
						condition.right_is_attr = 1;
						condition.right_attr = condition.right_exp->attr;
					}else if(right_flag == 2){
						condition.right_is_attr = 0;
						condition.right_value = condition.right_exp->value;
					}else{
						condition.right_is_attr = 0;
						condition.right_value = condition.right_exp->value;
						if(condition.right_value.type == FLOATS){
							*((float*)(condition.right_value.data)) = -*(float*)(condition.right_value.data);
						}else{
							*((int*)(condition.right_value.data)) = -*(int*)(condition.right_value.data);
						}
					}
					free(condition.right_exp);
					condition.right_exp = NULL;
					free(condition.left_exp);
					condition.left_exp = NULL;
				}
			}
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
			selects->conditions[selects->condition_num++] = condition;
	}
#line 2692 "yacc_sql.tab.c"
    break;

  case 139: /* expr: expr PLUS_S term  */
#line 966 "yacc_sql.y"
                         {
		(yyval.exp) = expression_create(NULL, NULL, (yyvsp[-2].exp), (yyvsp[0].exp), PLUS);
	}
#line 2700 "yacc_sql.tab.c"
    break;

  case 140: /* expr: expr MINUS_S term  */
#line 969 "yacc_sql.y"
                            {
		(yyval.exp) = expression_create(NULL, NULL, (yyvsp[-2].exp), (yyvsp[0].exp), MINUS);
	}
#line 2708 "yacc_sql.tab.c"
    break;

  case 141: /* expr: term  */
#line 972 "yacc_sql.y"
               {
		(yyval.exp) = (yyvsp[0].exp);
	}
#line 2716 "yacc_sql.tab.c"
    break;

  case 142: /* term: term STAR factor  */
#line 978 "yacc_sql.y"
                         {
		(yyval.exp) = expression_create(NULL, NULL, (yyvsp[-2].exp), (yyvsp[0].exp), MULT);
	}
#line 2724 "yacc_sql.tab.c"
    break;

  case 143: /* term: term SLASH factor  */
#line 981 "yacc_sql.y"
                            {
		(yyval.exp) = expression_create(NULL, NULL, (yyvsp[-2].exp), (yyvsp[0].exp), DIV);
	}
#line 2732 "yacc_sql.tab.c"
    break;

  case 144: /* term: factor  */
#line 984 "yacc_sql.y"
                 {
		(yyval.exp) = (yyvsp[0].exp);
	}
#line 2740 "yacc_sql.tab.c"
    break;

  case 145: /* factor: lbrace expr rbrace  */
#line 990 "yacc_sql.y"
                           {
		(yyval.exp) = (yyvsp[-1].exp);
	}
#line 2748 "yacc_sql.tab.c"
    break;

  case 146: /* factor: MINUS_S factor  */
#line 993 "yacc_sql.y"
                         {
		(yyval.exp) = expression_create(NULL, NULL, NULL, (yyvsp[0].exp), MINUS);
	}
#line 2756 "yacc_sql.tab.c"
    break;

  case 147: /* factor: base_value  */
#line 996 "yacc_sql.y"
                     {
		(yyval.exp) = (yyvsp[0].exp);
	}
#line 2764 "yacc_sql.tab.c"
    break;

  case 148: /* base_value: NUMBER  */
#line 1002 "yacc_sql.y"
               {
		Value value;
  		value_init_integer(&value, (yyvsp[0].number));
		(yyval.exp) = expression_create(NULL, &value, NULL, NULL, NO_CAL_OP);
	}
#line 2774 "yacc_sql.tab.c"
    break;

  case 149: /* base_value: FLOAT  */
#line 1007 "yacc_sql.y"
                {
		Value value;
  		value_init_float(&value, (yyvsp[0].floats));
		(yyval.exp) = expression_create(NULL, &value, NULL, NULL, NO_CAL_OP);
	}
#line 2784 "yacc_sql.tab.c"
    break;

  case 150: /* base_value: ID  */
#line 1012 "yacc_sql.y"
             {
		RelAttr attr;
		relation_attr_init(&attr, NULL, (yyvsp[0].string));
		(yyval.exp) = expression_create(&attr, NULL, NULL, NULL, NO_CAL_OP);
	}
#line 2794 "yacc_sql.tab.c"
    break;

  case 151: /* base_value: ID DOT ID  */
#line 1017 "yacc_sql.y"
                    {
		RelAttr attr;
		relation_attr_init(&attr, (yyvsp[-2].string), (yyvsp[0].string));
		(yyval.exp) = expression_create(&attr, NULL, NULL, NULL, NO_CAL_OP);
	}
#line 2804 "yacc_sql.tab.c"
    break;

  case 152: /* sub_select_condition: ID IN sub_select  */
#line 1025 "yacc_sql.y"
                         {
		RelAttr left_attr;
		relation_attr_init(&left_attr, NULL, (yyvsp[-2].string));

		Condition condition;
		subquery_condition_init(&condition, &left_attr, CONTEXT->sub_selects, WHERE_IN);
		Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
		selects->conditions[selects->condition_num++] = condition;
	}
#line 2818 "yacc_sql.tab.c"
    break;

  case 153: /* sub_select_condition: ID DOT ID IN sub_select  */
#line 1034 "yacc_sql.y"
                                  {
		RelAttr left_attr;
		relation_attr_init(&left_attr, (yyvsp[-4].string), (yyvsp[-2].string));

		Condition condition;
		subquery_condition_init(&condition, &left_attr, CONTEXT->sub_selects, WHERE_IN);
		Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
		selects->conditions[selects->condition_num++] = condition;
	}
#line 2832 "yacc_sql.tab.c"
    break;

  case 154: /* sub_select_condition: ID NOT IN sub_select  */
#line 1043 "yacc_sql.y"
                               {
		RelAttr left_attr;
		relation_attr_init(&left_attr, NULL, (yyvsp[-3].string));

		Condition condition;
		subquery_condition_init(&condition, &left_attr, CONTEXT->sub_selects, NOT_IN);
		Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
		selects->conditions[selects->condition_num++] = condition;
	}
#line 2846 "yacc_sql.tab.c"
    break;

  case 155: /* sub_select_condition: ID DOT ID NOT IN sub_select  */
#line 1052 "yacc_sql.y"
                                      {
		RelAttr left_attr;
		relation_attr_init(&left_attr, (yyvsp[-5].string), (yyvsp[-3].string));

		Condition condition;
		subquery_condition_init(&condition, &left_attr, CONTEXT->sub_selects, NOT_IN);
		Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
		selects->conditions[selects->condition_num++] = condition;
	}
#line 2860 "yacc_sql.tab.c"
    break;

  case 156: /* sub_select_condition: sub_select comOp expr  */
#line 1097 "yacc_sql.y"
                               {
		CalExp *exp = (yyvsp[0].exp);
		Condition condition;
		if(exp->is_attr){
			subquery_condition_init(&condition, &exp->attr, CONTEXT->sub_selects, reverse(CONTEXT->comp[CONTEXT->selects_num]));
			Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
			selects->conditions[selects->condition_num++] = condition;
		}
	}
#line 2874 "yacc_sql.tab.c"
    break;

  case 157: /* sub_select_condition: expr comOp sub_select  */
#line 1106 "yacc_sql.y"
                               {
		CalExp *exp = (yyvsp[-2].exp);
		Condition condition;
		if(exp->is_attr){
			subquery_condition_init(&condition, &exp->attr, CONTEXT->sub_selects, CONTEXT->comp[CONTEXT->selects_num]);
			Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
			selects->conditions[selects->condition_num++] = condition;
		}
	}
#line 2888 "yacc_sql.tab.c"
    break;

  case 158: /* sub_select_condition: sub_select comOp sub_select  */
#line 1115 "yacc_sql.y"
                                      {
		Condition condition;
		condition.comp = CONTEXT->comp[CONTEXT->selects_num];
		condition.left_is_attr = 0;
		condition.right_is_attr = 0;
		condition.sub_selects = (yyvsp[0].selects);
		condition.sub_selects_left = (yyvsp[-2].selects);
		condition.left_exp = NULL;
		condition.right_exp = NULL;
		Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
		selects->conditions[selects->condition_num++] = condition;
	}
#line 2905 "yacc_sql.tab.c"
    break;

  case 159: /* comOp: EQ  */
#line 1130 "yacc_sql.y"
         { CONTEXT->comp[CONTEXT->selects_num] = EQUAL_TO; }
#line 2911 "yacc_sql.tab.c"
    break;

  case 160: /* comOp: LT  */
#line 1131 "yacc_sql.y"
         { CONTEXT->comp[CONTEXT->selects_num] = LESS_THAN; }
#line 2917 "yacc_sql.tab.c"
    break;

  case 161: /* comOp: GT  */
#line 1132 "yacc_sql.y"
         { CONTEXT->comp[CONTEXT->selects_num] = GREAT_THAN; }
#line 2923 "yacc_sql.tab.c"
    break;

  case 162: /* comOp: LE  */
#line 1133 "yacc_sql.y"
         { CONTEXT->comp[CONTEXT->selects_num] = LESS_EQUAL; }
#line 2929 "yacc_sql.tab.c"
    break;

  case 163: /* comOp: GE  */
#line 1134 "yacc_sql.y"
         { CONTEXT->comp[CONTEXT->selects_num] = GREAT_EQUAL; }
#line 2935 "yacc_sql.tab.c"
    break;

  case 164: /* comOp: NE  */
#line 1135 "yacc_sql.y"
         { CONTEXT->comp[CONTEXT->selects_num] = NOT_EQUAL; }
#line 2941 "yacc_sql.tab.c"
    break;

  case 165: /* load_data: LOAD DATA INFILE SSS INTO TABLE ID SEMICOLON  */
#line 1140 "yacc_sql.y"
                {
		  CONTEXT->ssql->flag = SCF_LOAD_DATA;
			load_data_init(&CONTEXT->ssql->sstr.load_data, (yyvsp[-1].string), (yyvsp[-4].string));
		}
#line 2950 "yacc_sql.tab.c"
    break;


#line 2954 "yacc_sql.tab.c"

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

#line 1145 "yacc_sql.y"

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
