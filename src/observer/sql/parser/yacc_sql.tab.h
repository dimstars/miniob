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




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 101 "yacc_sql.y"
{
    struct _Attr *attr;
	  struct _Condition *condition1;
  	struct _Value *value1;
    char *string;
  //  char *ssss;
    int number;
    float floats;
}
/* Line 1529 of yacc.c.  */
#line 151 "yacc_sql.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

