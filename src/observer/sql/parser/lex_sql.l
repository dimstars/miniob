%{
#include<string.h>
#include<stdio.h>

struct ParserContext;

#include "yacc_sql.tab.h"
extern int atoi();
extern double atof();
char * position = "";

#if YYDEBUG > 0
#define debug_printf  printf
#else
#define debug_printf(...)
#endif // YYDEBUG

#define RETURN_TOKEN(token) debug_printf("%s\n",#token);return token
%}

/* Prevent the need for linking with -lfl */
%option noyywrap
%option bison-bridge
%option reentrant

WS      [\ \t\b\f]
DIGIT   [0-9]+

ID      [A-Za-z_]+[A-Za-z0-9_]*
DOT     \.
QUOTE   [\'\"]
%x STR

%%

{WS}                    /* ignore whitespace */;
\n																						 ;			

[\-]?{DIGIT}+					                  { yylval->number=atoi(yytext); position=strdup(yytext); RETURN_TOKEN(NUMBER);}
[\-]?{DIGIT}+{DOT}{DIGIT}+				      { yylval->floats=(float)(atof(yytext)); position=strdup(yytext); RETURN_TOKEN(FLOAT);}

";"                 	 				           RETURN_TOKEN(SEMICOLON);
{DOT}                 					         RETURN_TOKEN(DOT);
"*"                   					         position=strdup(yytext);RETURN_TOKEN(STAR);
[Ee][Xx][Ii][Tt]						             position=strdup(yytext); RETURN_TOKEN(EXIT);
[Hh][Ee][Ll][Pp]                    	   position=strdup(yytext); RETURN_TOKEN(HELP);
[Dd][Ee][Ss][Cc]                         position=strdup(yytext); RETURN_TOKEN(DESC);
[Cc][Rr][Ee][Aa][Tt][Ee]                 position=strdup(yytext);RETURN_TOKEN(CREATE);
[Dd][Rr][Oo][Pp]                    	   position=strdup(yytext);RETURN_TOKEN(DROP);
[Tt][Aa][Bb][Ll][Ee]					           position=strdup(yytext);RETURN_TOKEN(TABLE);
[Tt][Aa][Bb][Ll][Ee][Ss]			           position=strdup(yytext);RETURN_TOKEN(TABLES);
[Ii][Nn][Dd][Ee][Xx]                  	 position=strdup(yytext);RETURN_TOKEN(INDEX);
[Oo][Nn]								                 position=strdup(yytext);RETURN_TOKEN(ON);
[Ss][Hh][Oo][Ww]                         position=strdup(yytext);RETURN_TOKEN(SHOW);
[Ss][Yy][Nn][Cc]                         position=strdup(yytext);RETURN_TOKEN(SYNC);
[Ss][Ee][Ll][Ee][Cc][Tt]                 position=strdup(yytext);RETURN_TOKEN(SELECT);
[Ff][Rr][Oo][Mm]                      	 position=strdup(yytext);RETURN_TOKEN(FROM);
[Ww][Hh][Ee][Rr][Ee]                  	 position=strdup(yytext);RETURN_TOKEN(WHERE);
[Aa][Nn][Dd]                             position=strdup(yytext);RETURN_TOKEN(AND);
[Ii][Nn][Ss][Ee][rR][tT]                 position=strdup(yytext);RETURN_TOKEN(INSERT);
[Ii][Nn][Tt][Oo]					 	             position=strdup(yytext);RETURN_TOKEN(INTO);
[Vv][Aa][Ll][Uu][Ee][Ss]                 position=strdup(yytext);RETURN_TOKEN(VALUES);
[Dd][Ee][Ll][Ee][Tt][Ee]                 position=strdup(yytext);RETURN_TOKEN(DELETE);
[Uu][Pp][Dd][Aa][Tt][Ee]                 position=strdup(yytext);RETURN_TOKEN(UPDATE);
[Ss][Ee][Tt]							               position=strdup(yytext);RETURN_TOKEN(SET);
[Bb][Ee][Gg][Ii][Nn]                     { position=strdup(yytext); RETURN_TOKEN(TRX_BEGIN);}
[Cc][Oo][Mm][Mm][Ii][Tt]                 { position=strdup(yytext); RETURN_TOKEN(TRX_COMMIT);}
[Rr][Oo][Ll][Ll][Bb][Aa][Cc][Kk]         { position=strdup(yytext); RETURN_TOKEN(TRX_ROLLBACK);}
[Ii][Nn][Tt]					                   { position=strdup(yytext);RETURN_TOKEN(INT_T);}
[Cc][Hh][Aa][Rr]				                 { position=strdup(yytext);RETURN_TOKEN(STRING_T);}
[Ff][Ll][Oo][Aa][Tt]			               { position=strdup(yytext);RETURN_TOKEN(FLOAT_T);}
[Ll][Oo][Aa][Dd]			                   { position=strdup(yytext);RETURN_TOKEN(LOAD);}
[Dd][Aa][Tt][Aa]	                       { position=strdup(yytext);RETURN_TOKEN(DATA);}
[Ii][Nn][Ff][Ii][Ll][Ee]                 { position=strdup(yytext);RETURN_TOKEN(INFILE);}
{ID}							                       { yylval->string=strdup(yytext);position=strdup(yytext);RETURN_TOKEN(ID);}
"("								                       { position=strdup(yytext);RETURN_TOKEN(LBRACE);}
")"								                       position=strdup(yytext);RETURN_TOKEN(RBRACE);

","                                      position=strdup(yytext);RETURN_TOKEN(COMMA);
"="                                      position=strdup(yytext);RETURN_TOKEN(EQ);
"<="                                     position=strdup(yytext);RETURN_TOKEN(LE);
"<>"                                     position=strdup(yytext);RETURN_TOKEN(NE);
"<"                                      position=strdup(yytext);RETURN_TOKEN(LT);
">="                                     position=strdup(yytext);RETURN_TOKEN(GE);
">"                                      position=strdup(yytext);RETURN_TOKEN(GT);
{QUOTE}[\40\42\47A-Za-z0-9_/\.\-]*{QUOTE}	     {yylval->string=strdup(yytext);position = strdup(yytext);RETURN_TOKEN(SSS);}

.						                 { printf("Unknown character [%c]\n",yytext[0]);  return yytext[0];    }
%%

void scan_string(const char *str, yyscan_t scanner) {
	yy_switch_to_buffer(yy_scan_string(str, scanner), scanner);
}