
%{

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
  Value values[MAX_NUM];
  Condition conditions[MAX_NUM];
  CompOp comp;
	char id[MAX_NUM];
	yyscan_t scanner;
} ParserContext;

#define YYPARSE_PARAM _context
#define YYLEX_PARAM &yylval, ((ParserContext *)_context)->scanner
#define yyerror(msg) my_yyerror(msg, YYPARSE_PARAM)
#define CONTEXT context(_context)

ParserContext *context(void *_context) {
	return (ParserContext *)_context;
}

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

void my_yyerror(const char *str, ParserContext *context) {

	context->ssql->flag=0;
	context->condition_length = 0;
	context->from_length = 0;
	context->select_length = 0;
	context->value_length = 0;
  context->ssql->sstr.insertion.value_num = 0;
	context->ssql->sstr.errors = position;
	printf("parse sql failed. error=%s", str);
}

%}
//标识tokens
%token  SEMICOLON
        CREATE
        DROP
        TABLE
        TABLES
        INDEX
        SELECT
        DESC
        SHOW
        SYNC
        INSERT
        DELETE
        UPDATE
        LBRACE
        RBRACE
        COMMA
        TRX_BEGIN
        TRX_COMMIT
        TRX_ROLLBACK
        INT_T
        STRING_T
        FLOAT_T
        HELP
        EXIT
        DOT //QUOTE
        INTO
        VALUES
        FROM
        WHERE
        AND
        SET
        ON
        EQ
        LT
        GT
        LE
        GE
        NE

%union {
    struct _Attr *attr;
	  struct _Condition *condition1;
  	struct _Value *value1;
    char *string;
  //  char *ssss;
    int number;
    float floats;
}

%token <number> NUMBER
%token <floats> FLOAT 
%token <string> ID
%token <string> SSS
%token <string>  STAR
%token <string> STRING_V
//非终结符

%type <number> type;
%type <condition1> condition;
%type <value1> value;
%type <number> number;

%%

commands:		//commands or sqls. parser starts here.
    /* empty */
    | commands command
    ;

command:
	  select  
	| insert
	| update
	| delete
	| create_table
	| drop_table
	| show_tables
	| desc_table
	| create_index	
	| drop_index
	| sync
	| begin
	| commit
	| rollback
	| help
	| exit
    ;

exit:			
    EXIT SEMICOLON {
        CONTEXT->ssql->flag=SCF_EXIT;//"exit";
    };

help:
    HELP SEMICOLON {
        CONTEXT->ssql->flag=SCF_HELP;//"help";
    };

sync:
    SYNC SEMICOLON {
      CONTEXT->ssql->flag = SCF_SYNC;
    }
    ;

begin:
    TRX_BEGIN SEMICOLON {
      CONTEXT->ssql->flag = SCF_BEGIN;
    }
    ;

commit:
    TRX_COMMIT SEMICOLON {
      CONTEXT->ssql->flag = SCF_COMMIT;
    }
    ;

rollback:
    TRX_ROLLBACK SEMICOLON {
      CONTEXT->ssql->flag = SCF_ROLLBACK;
    }
    ;

drop_table:		/*drop table 语句的语法解析树*/
    DROP TABLE ID SEMICOLON {
        CONTEXT->ssql->flag = SCF_DROP_TABLE;//"drop_table";
        CONTEXT->ssql->sstr.drop_table.relation_name = $3;
    };

show_tables:
    SHOW TABLES SEMICOLON {
      CONTEXT->ssql->flag = SCF_SHOW_TABLES;
    }
    ;

desc_table:
    DESC ID SEMICOLON {
      CONTEXT->ssql->flag = SCF_DESC_TABLE;
      CONTEXT->ssql->sstr.desc_table.relation_name = $2;
    }
    ;

create_index:		/*create index 语句的语法解析树*/
    CREATE INDEX ID ON ID LBRACE ID RBRACE SEMICOLON 
		{
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			CONTEXT->ssql->sstr.create_index.index_name =$3;
			CONTEXT->ssql->sstr.create_index.relation_name =$5;
			CONTEXT->ssql->sstr.create_index.attribute_name =$7;
		}
    ;

drop_index:			/*drop index 语句的语法解析树*/
    DROP INDEX ID  SEMICOLON 
		{
			CONTEXT->ssql->flag=SCF_DROP_INDEX;//"drop_index";
			CONTEXT->ssql->sstr.drop_index.index_name =$3;
		}
    ;
create_table:		/*create table 语句的语法解析树*/
    CREATE TABLE ID LBRACE attr_def attr_def_list RBRACE SEMICOLON 
		{
	
			CONTEXT->ssql->flag=SCF_CREATE_TABLE;//"create_table";
			CONTEXT->ssql->sstr.create_table.relation_name =$3;
			CONTEXT->ssql->sstr.create_table.attribute_count = CONTEXT->value_length;
			//临时变量清零	
			CONTEXT->value_length = 0;
	
		}
    ;
attr_def_list:
    /* empty */
    | COMMA attr_def attr_def_list {    }
    ;
    
attr_def:
    ID_get type LBRACE number RBRACE 
		{
			CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name =(char*)malloc(sizeof(char)); // TODO FATAL
			strcpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type = $2;  
			CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length = $4;
			CONTEXT->value_length++;
		}
    |ID_get type
		{
			CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name=(char*)malloc(sizeof(char));
			strcpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type=$2;  
			CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length=4; // default attribute length
			CONTEXT->value_length++;
		}
    ;
number:
		NUMBER {$$ = $1;}
		;
type:
	INT_T { $$=INTS; }
       | STRING_T { $$=CHARS; }
       | FLOAT_T { $$=FLOATS; }
       ;
ID_get:
	ID 
	{
		char *temp=$1; 
		snprintf(CONTEXT->id, sizeof(CONTEXT->id), "%s", temp);
	}
	;

	
insert:				/*insert   语句的语法解析树*/
    INSERT INTO ID VALUES LBRACE value value_list RBRACE SEMICOLON 
		{
			int i;
			CONTEXT->values[CONTEXT->value_length++] = *$6;

			CONTEXT->ssql->flag=SCF_INSERT;//"insert";
			CONTEXT->ssql->sstr.insertion.relation_name = $3;
			CONTEXT->ssql->sstr.insertion.value_num = CONTEXT->value_length;
			for(i = 0; i < CONTEXT->value_length; i++){
				CONTEXT->ssql->sstr.insertion.values[i] = CONTEXT->values[i];
      }

      //临时变量清零
      CONTEXT->value_length=0;
    }

value_list:
    /* empty */
    | COMMA value value_list  { 
  		CONTEXT->values[CONTEXT->value_length++] = *$2;
	  }
    ;
value:
    NUMBER{	
					$$ = (Value*)malloc(sizeof(Value));
					$$->data=(int*)malloc((sizeof(int)));
					$$->type =INTS; 					                      // "ints";
					*((int*)($$->data)) = $1;
			}
    |FLOAT{								
					$$=(Value*)malloc(sizeof(Value));
					$$->data=malloc(MAX_NUM*(sizeof(char)));
					$$->type =FLOATS; 					                     	//"floats"
					*((float*)($$->data)) = $1;
			}
    |SSS {																		
					$$=(Value*)malloc(sizeof(Value));				
					$$->data=malloc(MAX_NUM*(sizeof(char)));		
    			$$->type = CHARS;					// "chars";
    			$1 = substr($1,1,strlen($1)-2); // TODO snprintf
    			sprintf((char*)($$->data), "%s", $1); 			
    	}
    ;
    
delete:		/*  delete 语句的语法解析树*/
    DELETE FROM ID where SEMICOLON 
		{
		  int i;
			CONTEXT->ssql->flag = SCF_DELETE;//"delete";
			CONTEXT->ssql->sstr.deletion.relation_name = $3;
			for(i = 0; i < CONTEXT->condition_length; i++){
				CONTEXT->ssql->sstr.deletion.conditions[i] = CONTEXT->conditions[i];
			}
			CONTEXT->ssql->sstr.deletion.condition_num = CONTEXT->condition_length;
			CONTEXT->condition_length = 0;	
    }
    ;
update:			/*  update 语句的语法解析树*/
    UPDATE ID SET ID EQ value where SEMICOLON
		{
			int i;
			CONTEXT->ssql->flag = SCF_UPDATE;//"update";
			CONTEXT->ssql->sstr.update.relation_name = $2;
			CONTEXT->ssql->sstr.update.value = *$6;
			CONTEXT->ssql->sstr.update.attribute_name = $4;
			for(i = 0; i < CONTEXT->condition_length; i++){
				CONTEXT->ssql->sstr.update.conditions[i] = CONTEXT->conditions[i];
			}
			CONTEXT->ssql->sstr.update.condition_num = CONTEXT->condition_length;
			CONTEXT->condition_length = 0;
		}
    ;
select:				/*  select 语句的语法解析树*/
    SELECT select_attr FROM ID rel_list where SEMICOLON 
		{
			int i;
			CONTEXT->ssql->sstr.selection.relations[CONTEXT->from_length++]=$4;

			for(i =0; i < CONTEXT->condition_length; i++){
				CONTEXT->ssql->sstr.selection.conditions[i] = CONTEXT->conditions[i];
			}

			CONTEXT->ssql->flag=SCF_SELECT;//"select";
			CONTEXT->ssql->sstr.selection.attr_num = CONTEXT->select_length;
			CONTEXT->ssql->sstr.selection.relation_num = CONTEXT->from_length;
			CONTEXT->ssql->sstr.selection.condition_num = CONTEXT->condition_length;

			//临时变量清零
			CONTEXT->condition_length=0;
			CONTEXT->from_length=0;
			CONTEXT->select_length=0;
	}
	;

select_attr:
    STAR {  // TODO 处理这里的内存泄露问题
         CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name ="*";
         CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name =NULL;
       }
    | ID attr_list {
        CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$1;
        CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=NULL;
      }
  	| ID DOT ID attr_list {
        CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$3;
        CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$1;
      }
    ;
attr_list:
    /* empty */
    | COMMA ID attr_list {
     	  CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
    | COMMA ID DOT ID attr_list {
        CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
        CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
  	  }
  	;

rel_list:
    /* empty */
    | COMMA ID rel_list {	
				CONTEXT->ssql->sstr.selection.relations[CONTEXT->from_length++]=$2;
		  }
    ;
where:
    /* empty */ 
    | WHERE condition condition_list {	
				CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
    ;
condition_list:
    /* empty */
    | AND condition condition_list {
				CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
    ;
condition:
    ID comOp value 
		{
			$$ = ( Condition *)malloc(sizeof( Condition));
			$$->left_is_attr = 1;
			$$->left_attr.relation_name = NULL;
			$$->left_attr.attribute_name= $1;
			$$->comp = CONTEXT->comp;
			$$->right_is_attr = 0;
			$$->right_attr.relation_name = NULL;
			$$->right_attr.attribute_name = NULL;
			$$->right_value = *$3;

		}
		|value comOp value 
		{
			$$ = ( Condition *)malloc(sizeof( Condition));
			$$->left_is_attr = 0;
			$$->left_attr.relation_name=NULL;
			$$->left_attr.attribute_name=NULL;
			$$->left_value = *$1;
			$$->comp = CONTEXT->comp;
			$$->right_is_attr = 0;
			$$->right_attr.relation_name = NULL;
			$$->right_attr.attribute_name = NULL;
			$$->right_value = *$3;

		}
		|ID comOp ID 
		{
			$$=( Condition *)malloc(sizeof( Condition));
			$$->left_is_attr = 1;
			$$->left_attr.relation_name=NULL;
			$$->left_attr.attribute_name=$1;
			$$->comp = CONTEXT->comp;
			$$->right_is_attr = 1;
			$$->right_attr.relation_name=NULL;
			$$->right_attr.attribute_name=$3;

		}
    |value comOp ID
		{
			$$=( Condition *)malloc(sizeof( Condition));
			$$->left_is_attr = 0;
			$$->left_attr.relation_name=NULL;
			$$->left_attr.attribute_name=NULL;
			$$->left_value = *$1;
			$$->comp=CONTEXT->comp;
			
			$$->right_is_attr = 1;
			$$->right_attr.relation_name=NULL;
			$$->right_attr.attribute_name=$3;
		
		}
    |ID DOT ID comOp value
		{
			$$=( Condition *)malloc(sizeof( Condition));
			$$->left_is_attr = 1;
			$$->left_attr.relation_name=$1;
			$$->left_attr.attribute_name=$3;
			$$->comp=CONTEXT->comp;
			$$->right_is_attr = 0;   //属性值
			$$->right_attr.relation_name=NULL;
			$$->right_attr.attribute_name=NULL;
			$$->right_value =*$5;			
							
    }
    |value comOp ID DOT ID
		{
			$$=( Condition *)malloc(sizeof( Condition));
			$$->left_is_attr = 0;//属性值
			$$->left_attr.relation_name=NULL;
			$$->left_attr.attribute_name=NULL;
			$$->left_value = *$1;
			$$->comp =CONTEXT->comp;
			$$->right_is_attr = 1;//属性
			$$->right_attr.relation_name = $3;
			$$->right_attr.attribute_name = $5;
									
    }
    |ID DOT ID comOp ID DOT ID
		{
			$$=( Condition *)malloc(sizeof( Condition));
			$$->left_is_attr = 1;		//属性
			$$->left_attr.relation_name=$1;
			$$->left_attr.attribute_name=$3;
			$$->comp =CONTEXT->comp;
			$$->right_is_attr = 1;		//属性
			$$->right_attr.relation_name=$5;
			$$->right_attr.attribute_name=$7;								
    }
    ;

comOp:
  	  EQ { CONTEXT->comp = EQUAL_TO; }
    | LT { CONTEXT->comp = LESS_THAN; }
    | GT { CONTEXT->comp = GREAT_THAN; }
    | LE { CONTEXT->comp = LESS_EQUAL; }
    | GE { CONTEXT->comp = GREAT_EQUAL; }
    | NE { CONTEXT->comp = NOT_EQUAL; }
    ;

%%
//_____________________________________________________________________
extern void scan_string(const char *str, yyscan_t scanner);

int sql_parse(const char *s, Query *sqls){
	ParserContext context;
	memset(&context, 0, sizeof(context));
	yylex_init(&context.scanner);
	context.ssql = sqls;
	scan_string(s, context.scanner);
	int result = yyparse(&context);
	yylex_destroy(context.scanner);
	return result;
}
