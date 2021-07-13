
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
  int tuple_num;
  int tuple_length[MAX_NUM];
  Value tuples[MAX_NUM][MAX_NUM];
  Value values[MAX_NUM];
  Condition conditions[MAX_NUM];
  CompOp comp;
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

%}

%define api.pure full
%lex-param { yyscan_t scanner }
%parse-param { void *scanner }

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
        UNIQUE
        LBRACE
        RBRACE
        COMMA
        TRX_BEGIN
        TRX_COMMIT
        TRX_ROLLBACK
        INT_T
        STRING_T
		TEXT_T
        FLOAT_T
		DATE_T
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
				LOAD
				DATA
				INFILE
        EQ
        LT
        GT
        LE
        GE
        NE
		MAX
		MIN
		COUNT
		AVG
		INNER
		JOIN
		IS
		NOT
		NULL_T
		NULLABLE
		IN

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
%token <string> PATH
%token <string> SSS
%token <string> STAR
%token <string> STRING_V
%token <string> DATE
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
	| create_unique_index
	| drop_index
	| sync
	| begin
	| commit
	| rollback
	| load_data
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
				drop_table_init(&CONTEXT->ssql->sstr.drop_table, $3);
    };

show_tables:
    SHOW TABLES SEMICOLON {
      CONTEXT->ssql->flag = SCF_SHOW_TABLES;
    }
    ;

desc_table:
    DESC ID SEMICOLON {
      CONTEXT->ssql->flag = SCF_DESC_TABLE;
			desc_table_init(&CONTEXT->ssql->sstr.desc_table, $2);
    }
    ;

create_index:		/*create index 语句的语法解析树*/
    CREATE INDEX ID ON ID LBRACE index_attr_def index_attr_def_list RBRACE SEMICOLON 
		{
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, $3, $5);
		}
    ;
create_unique_index:
	CREATE UNIQUE INDEX ID ON ID LBRACE index_attr_def index_attr_def_list RBRACE SEMICOLON 
		{
			CONTEXT->ssql->flag = SCF_CREATE_UNIQUE_INDEX;//"create_unique_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, $4, $6);
		}
	;
drop_index:			/*drop index 语句的语法解析树*/
    DROP INDEX ID  SEMICOLON 
		{
			CONTEXT->ssql->flag=SCF_DROP_INDEX;//"drop_index";
			drop_index_init(&CONTEXT->ssql->sstr.drop_index, $3);
		}
    ;
create_table:		/*create table 语句的语法解析树*/
    CREATE TABLE ID lbrace attr_def attr_def_list rbrace SEMICOLON 
		{
			CONTEXT->ssql->flag=SCF_CREATE_TABLE;//"create_table";
			// CONTEXT->ssql->sstr.create_table.attribute_count = CONTEXT->value_length; // TODO
			create_table_init_name(&CONTEXT->ssql->sstr.create_table, $3);
			//临时变量清零	
			CONTEXT->value_length = 0;
		}
    ;
attr_def_list:
    /* empty */
    | COMMA attr_def attr_def_list {    }
    ;
    
attr_def:
    ID_get type lbrace number rbrace 
		{
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, $2, $4, 1);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name =(char*)malloc(sizeof(char)); // TODO FATAL
			// strcpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type = $2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length = $4;
			CONTEXT->value_length++;
		}
	|ID_get type lbrace number rbrace NOT NULL_T
		{
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, $2, $4, 0);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
		}
	|ID_get type lbrace number rbrace NULLABLE
		{
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, $2, $4, 1);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
		}
    |ID_get type
		{
			AttrInfo attribute;
			// text : 4B len + 4B page num or 8B ptr
			attr_info_init(&attribute, CONTEXT->id, $2, $2 == TEXTS ? 8 : 4, 1);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name=(char*)malloc(sizeof(char));
			// strcpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type=$2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length=4; // default attribute length
			CONTEXT->value_length++;
		}
	|ID_get type NOT NULL_T
		{
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, $2, $2 == TEXTS ? 8 : 4, 0);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
		}
	|ID_get type NULLABLE
		{
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, $2, $2 == TEXTS ? 8 : 4, 1);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			CONTEXT->value_length++;
		}
    ;

index_attr_def_list:
	/* empty */
	| COMMA index_attr_def index_attr_def_list {    }
index_attr_def:
	ID
		{
			create_index_append_attribute(&CONTEXT->ssql->sstr.create_index, $1);
			CONTEXT->value_length++;
		}
number:
		NUMBER {$$ = $1;}
		;
type:
	INT_T { $$=INTS; }
       | STRING_T { $$=CHARS; }
       | FLOAT_T { $$=FLOATS; }
	   | DATE_T { $$=DATES; }
	   | TEXT_T { $$=TEXTS; }
       ;
ID_get:
	ID 
	{
		char *temp=$1; 
		snprintf(CONTEXT->id, sizeof(CONTEXT->id), "%s", temp);
	}
	;

insert:				/*insert   语句的语法解析树*/
    INSERT INTO ID VALUES tuple tuple_list SEMICOLON 
		{
			CONTEXT->ssql->flag=SCF_INSERT;
			inserts_init(&CONTEXT->ssql->sstr.insertion, $3, CONTEXT->tuples, CONTEXT->tuple_num, CONTEXT->tuple_length);
      		//临时变量清零
      		for (int i = 0; i < CONTEXT->tuple_num; i++) {
				CONTEXT->tuple_length[i] = 0;
			}
			CONTEXT->tuple_num = 0;
    }
	;
tuple_list:
	| COMMA tuple tuple_list {
	}
	;
tuple:
	lbrace tuple_value value_list rbrace {
		CONTEXT->tuple_num++;
	}
	;
value_list:
    | COMMA tuple_value value_list  { 
	  	}
    ;
tuple_value:
    NUMBER {
  			value_init_integer(&CONTEXT->tuples[CONTEXT->tuple_num][CONTEXT->tuple_length[CONTEXT->tuple_num]++], $1);
		}
    |FLOAT {
  			value_init_float(&CONTEXT->tuples[CONTEXT->tuple_num][CONTEXT->tuple_length[CONTEXT->tuple_num]++], $1);
		}
    |SSS {
			$1 = substr($1,1,strlen($1)-2);
  			value_init_string(&CONTEXT->tuples[CONTEXT->tuple_num][CONTEXT->tuple_length[CONTEXT->tuple_num]++], $1);
		}
	|DATE {
			$1 = substr($1,1,strlen($1)-2);
			value_init_date(&CONTEXT->tuples[CONTEXT->tuple_num][CONTEXT->tuple_length[CONTEXT->tuple_num]++], $1);
		}
	|NULL_T {
			value_init_null(&CONTEXT->tuples[CONTEXT->tuple_num][CONTEXT->tuple_length[CONTEXT->tuple_num]++]);
		}
    ;

value:
    NUMBER{	
  		value_init_integer(&CONTEXT->values[CONTEXT->value_length++], $1);
		}
    |FLOAT{
  		value_init_float(&CONTEXT->values[CONTEXT->value_length++], $1);
		}
    |SSS {
			$1 = substr($1,1,strlen($1)-2);
  		value_init_string(&CONTEXT->values[CONTEXT->value_length++], $1);
		}
	|DATE {
			$1 = substr($1,1,strlen($1)-2);
			value_init_date(&CONTEXT->values[CONTEXT->value_length++], $1);
		}
    ;
    
delete:		/*  delete 语句的语法解析树*/
    DELETE FROM ID where SEMICOLON 
		{
			CONTEXT->ssql->flag = SCF_DELETE;//"delete";
			deletes_init_relation(&CONTEXT->ssql->sstr.deletion, $3);
			deletes_set_conditions(&CONTEXT->ssql->sstr.deletion, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;	
    }
    ;
update:			/*  update 语句的语法解析树*/
    UPDATE ID SET ID EQ value where SEMICOLON
		{
			CONTEXT->ssql->flag = SCF_UPDATE;//"update";
			Value *value = &CONTEXT->values[0];
			updates_init(&CONTEXT->ssql->sstr.update, $2, $4, value, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;
		}
	| UPDATE ID SET ID EQ NULL_T where SEMICOLON
		{
			CONTEXT->ssql->flag = SCF_UPDATE;//"update";
			Value *value = &CONTEXT->null_value;
			value_init_null(value);
			updates_init(&CONTEXT->ssql->sstr.update, $2, $4, value, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;
		}
    ;
select:				/*  select 语句的语法解析树*/
    SELECT select_attr FROM ID rel_list where SEMICOLON {
		CONTEXT->ssql->sstr.selection = CONTEXT->selects[CONTEXT->selects_num];
		selects_append_relation(&CONTEXT->ssql->sstr.selection, $4);

		CONTEXT->ssql->flag=SCF_SELECT;//"select";

		//临时变量清零
		CONTEXT->condition_length=0;
		CONTEXT->from_length=0;
		CONTEXT->select_length=0;
		CONTEXT->value_length = 0;
	}
	| SELECT select_attr FROM ID join_list where SEMICOLON {
		CONTEXT->ssql->sstr.selection = CONTEXT->selects[CONTEXT->selects_num];
		selects_append_relation(&CONTEXT->ssql->sstr.selection, $4);

		CONTEXT->ssql->flag=SCF_SELECT;//"select";

		//临时变量清零
		CONTEXT->condition_length=0;
		CONTEXT->from_length=0;
		CONTEXT->select_length=0;
		CONTEXT->value_length = 0;
	}
	;

sub_select:
    lbrace_select ID FROM ID where rbrace{
		Selects *sub_selects = (Selects *)malloc(sizeof(Selects));
		*sub_selects = CONTEXT->selects[CONTEXT->selects_num + 1];
		memset(&CONTEXT->selects[CONTEXT->selects_num + 1], 0, sizeof(Selects));
		
		RelAttr left_attr;
		relation_attr_init(&left_attr, $4, $2);
		selects_append_attribute(sub_selects, &left_attr);
		selects_append_relation(sub_selects, $4);

		CONTEXT->sub_selects = sub_selects;
    }
    | lbrace_select ID DOT ID FROM ID where rbrace {
		Selects *sub_selects = (Selects *)malloc(sizeof(Selects));
		*sub_selects = CONTEXT->selects[CONTEXT->selects_num + 1];
		memset(&CONTEXT->selects[CONTEXT->selects_num + 1], 0, sizeof(Selects));
		
		RelAttr left_attr;
		relation_attr_init(&left_attr, $2, $4);
		selects_append_attribute(sub_selects, &left_attr);
		selects_append_relation(sub_selects, $6);

		CONTEXT->sub_selects = sub_selects;
    }
	;

lbrace_select:
	lbrace SELECT {
		CONTEXT->selects_num ++;
		CONTEXT->lbrace_selects[CONTEXT->selects_num] = CONTEXT->lbrace_num;
	}
	;

lbrace:
	LBRACE {
		CONTEXT->lbrace_num ++;
	}
	;

rbrace:
	RBRACE {
		if (CONTEXT->lbrace_selects[CONTEXT->selects_num] == CONTEXT->lbrace_num && CONTEXT->selects_num) {
			CONTEXT->selects_num --;
		}
		CONTEXT->lbrace_num --;
	}
	;

join_list:
	inner_join ID ON condition condition_list join_list_ {
		selects_append_relation(&CONTEXT->ssql->sstr.selection, $2);
		selects_append_relation(&CONTEXT->selects[CONTEXT->selects_num], $2);
	}
	;
join_list_:
	| inner_join ID ON condition condition_list join_list_ {
		selects_append_relation(&CONTEXT->ssql->sstr.selection, $2);
		selects_append_relation(&CONTEXT->selects[CONTEXT->selects_num], $2);
	}
	;
inner_join:
	INNER JOIN {}
	| JOIN {}
	;

select_attr:
    STAR {  
			RelAttr attr;
			relation_attr_init(&attr, NULL, "*");
			selects_append_attribute(&CONTEXT->selects[CONTEXT->selects_num], &attr);
		}
	| ID DOT STAR attr_list {
			RelAttr attr;
			relation_attr_init(&attr, $1, "*");
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
    | ID attr_list {
			RelAttr attr;
			relation_attr_init(&attr, NULL, $1);
			selects_append_attribute(&CONTEXT->selects[CONTEXT->selects_num], &attr);
		}
  	| ID DOT ID attr_list {
			RelAttr attr;
			relation_attr_init(&attr, $1, $3);
			selects_append_attribute(&CONTEXT->selects[CONTEXT->selects_num], &attr);
		}
	| MAX lbrace ID rbrace attr_list {
			AggOp agg;
			aggregation_init_string(&agg, NULL, $3, MAX_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
	| MAX lbrace ID DOT ID rbrace attr_list {
			AggOp agg;
			aggregation_init_string(&agg, $3, $5, MAX_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
	| MIN lbrace ID rbrace attr_list {
			AggOp agg;
			aggregation_init_string(&agg, NULL, $3, MIN_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
	| MIN lbrace ID DOT ID rbrace attr_list {
			AggOp agg;
			aggregation_init_string(&agg, $3, $5, MIN_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
	| COUNT lbrace STAR rbrace attr_list {
			AggOp agg;
			aggregation_init_string(&agg, NULL, "*", COUNT_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
	| COUNT lbrace ID rbrace attr_list {
			AggOp agg;
			aggregation_init_string(&agg, NULL, $3, COUNT_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
	| COUNT lbrace ID DOT ID rbrace attr_list {
			AggOp agg;
			aggregation_init_string(&agg, $3, $5, COUNT_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
	| COUNT lbrace NUMBER rbrace attr_list {  //  TODO optimize count(n)
			AggOp agg;
			aggregation_init_integer(&agg, NULL, $3, COUNT_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
	| AVG lbrace ID rbrace attr_list {
			AggOp agg;
			aggregation_init_string(&agg, NULL, $3, AVG_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
	| AVG lbrace ID DOT ID rbrace attr_list {
			AggOp agg;
			aggregation_init_string(&agg, $3, $5, AVG_A);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
    ;
attr_list:
    /* empty */
	| COMMA ID DOT STAR attr_list {
			RelAttr attr;
			relation_attr_init(&attr, $2, "*");
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
    | COMMA ID attr_list {
			RelAttr attr;
			relation_attr_init(&attr, NULL, $2);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			selects_append_attribute(&CONTEXT->selects[CONTEXT->selects_num], &attr);
      }
    | COMMA ID DOT ID attr_list {
			RelAttr attr;
			relation_attr_init(&attr, $2, $4);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
			selects_append_attribute(&CONTEXT->selects[CONTEXT->selects_num], &attr);
  	  }
	| COMMA MAX lbrace ID rbrace attr_list {
			AggOp agg;
			aggregation_init_string(&agg, NULL, $4, MAX_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
	| COMMA MAX lbrace ID DOT ID rbrace attr_list {
			AggOp agg;
			aggregation_init_string(&agg, $4, $6, MAX_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
	| COMMA MIN lbrace ID rbrace attr_list {
			AggOp agg;
			aggregation_init_string(&agg, NULL, $4, MIN_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
	| COMMA MIN lbrace ID DOT ID rbrace attr_list {
			AggOp agg;
			aggregation_init_string(&agg, $4, $6, MIN_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
	| COMMA COUNT lbrace STAR rbrace attr_list { // count(*) 忽略null记录
			AggOp agg;
			aggregation_init_string(&agg, NULL, "*", COUNT_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
	| COMMA COUNT lbrace ID rbrace attr_list {
			AggOp agg;
			aggregation_init_string(&agg, NULL, $4, COUNT_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
	| COMMA COUNT lbrace ID DOT ID rbrace attr_list {
			AggOp agg;
			aggregation_init_string(&agg, $4, $6, COUNT_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
	| COMMA COUNT lbrace NUMBER rbrace attr_list {  //  TODO optimize count(n) // conut(n) 不忽略null记录
			AggOp agg;
			aggregation_init_integer(&agg, NULL, $4, COUNT_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
	| COMMA AVG lbrace ID rbrace attr_list {
			AggOp agg;
			aggregation_init_string(&agg, NULL, $4, AVG_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
	| COMMA AVG lbrace ID DOT ID rbrace attr_list {
			AggOp agg;
			aggregation_init_string(&agg, $4, $6, AVG_A);
			selects_append_aggregation(&CONTEXT->ssql->sstr.selection, &agg);
			selects_append_aggregation(&CONTEXT->selects[CONTEXT->selects_num], &agg);
		}
  	;

rel_list:
    /* empty */
    | COMMA ID rel_list {	
				selects_append_relation(&CONTEXT->selects[CONTEXT->selects_num], $2);
		  }
    ;
where:
    /* empty */ 
    | WHERE condition condition_list {}
    ;
condition_list:
    /* empty */
    | AND condition condition_list {}
    ;
condition:
    ID comOp value 
		{
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, $1);

			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
			selects->conditions[selects->condition_num++] = condition;
		}
	|value comOp value 
		{
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 2];
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
			selects->conditions[selects->condition_num++] = condition;
		}
	|ID comOp ID 
		{
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, $1);
			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, $3);

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
			selects->conditions[selects->condition_num++] = condition;
		}
    |value comOp ID
		{
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];
			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, $3);

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
			selects->conditions[selects->condition_num++] = condition;
		}
    |ID DOT ID comOp value
		{
			RelAttr left_attr;
			relation_attr_init(&left_attr, $1, $3);
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
			selects->conditions[selects->condition_num++] = condition;								
    }
    |value comOp ID DOT ID
		{
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];

			RelAttr right_attr;
			relation_attr_init(&right_attr, $3, $5);

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
			selects->conditions[selects->condition_num++] = condition;								
    }
    |ID DOT ID comOp ID DOT ID
		{
			RelAttr left_attr;
			relation_attr_init(&left_attr, $1, $3);
			RelAttr right_attr;
			relation_attr_init(&right_attr, $5, $7);

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
			selects->conditions[selects->condition_num++] = condition;
    }
	|ID IS NULL_T
		{
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, $1);
			
			Value *right_value = &CONTEXT->null_value;
			value_init_null(right_value);

			Condition condition;
			condition_init(&condition, EQUAL_TO, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		}
	|ID DOT ID IS NULL_T
		{
			RelAttr left_attr;
			relation_attr_init(&left_attr, $1, $3);

			Value *right_value = &CONTEXT->null_value;;
			value_init_null(right_value);

			Condition condition;
			condition_init(&condition, EQUAL_TO, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		}
	|ID IS NOT NULL_T
		{
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, $1);
			
			Value *right_value = &CONTEXT->null_value;;
			value_init_null(right_value);

			Condition condition;
			condition_init(&condition, NOT_EQUAL, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		}
	|ID DOT ID IS NOT NULL_T
		{
			RelAttr left_attr;
			relation_attr_init(&left_attr, $1, $3);

			Value *right_value = &CONTEXT->null_value;;
			value_init_null(right_value);

			Condition condition;
			condition_init(&condition, NOT_EQUAL, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
		}
	| sub_select_condition {}
    ;

sub_select_condition:
	ID IN sub_select {
		RelAttr left_attr;
		relation_attr_init(&left_attr, NULL, $1);

		Condition condition;
		subquery_condition_init(&condition, &left_attr, CONTEXT->sub_selects);
		Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
		selects->conditions[selects->condition_num++] = condition;
	}
	| ID DOT ID IN sub_select {
		RelAttr left_attr;
		relation_attr_init(&left_attr, $1, $3);

		Condition condition;
		subquery_condition_init(&condition, &left_attr, CONTEXT->sub_selects);
		Selects *selects = &CONTEXT->selects[CONTEXT->selects_num];
		selects->conditions[selects->condition_num++] = condition;
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

load_data:
		LOAD DATA INFILE SSS INTO TABLE ID SEMICOLON
		{
		  CONTEXT->ssql->flag = SCF_LOAD_DATA;
			load_data_init(&CONTEXT->ssql->sstr.load_data, $7, $4);
		}
		;
%%
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
