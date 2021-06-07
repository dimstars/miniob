
%{

#include "handler/handler_defs.h"
// #include "common/log/log.h" // TODO 包含C++中的头文件

extern char * position;
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
extern int yylex(void);

typedef struct ParserContext {
  sqlstr * sqlstr;
  int where_length;
  int from_length;
  int value_length;
  Value value[MAX_NUM];
  CompOp comp;
} ParserContext;

#define YYPARSE_PARAM context

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
  ssql->sstr.insertion.value_num = 0;
	ssql->sstr.errors=position;
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

/*match to the flag of sqls 
		0--error;
		1--select;
		2--insert;
		3--update;
		4--delete;
		5--create table;
		6--drop table;
		7--create index;
		8--drop index;
		9--help;
		10--exit;
*/

exit:			
    EXIT SEMICOLON {
        ssql->flag=SCF_EXIT;//"exit";
    };

help:
    HELP SEMICOLON {
        ssql->flag=SCF_HELP;//"help";
    };

sync:
    SYNC SEMICOLON {
      ssql->flag = SCF_SYNC;
    }
    ;

begin:
    TRX_BEGIN SEMICOLON {
      ssql->flag = SCF_BEGIN;
    }
    ;

commit:
    TRX_COMMIT SEMICOLON {
      ssql->flag = SCF_COMMIT;
    }
    ;

rollback:
    TRX_ROLLBACK SEMICOLON {
      ssql->flag = SCF_ROLLBACK;
    }
    ;

drop_table:		/*drop table 语句的语法解析树*/
    DROP TABLE ID SEMICOLON {
        ssql->flag=SCF_DROP_TABLE;//"drop_table";
        ssql->sstr.drop_table.relation_name =$3;
    };

show_tables:
    SHOW TABLES SEMICOLON {
      ssql->flag=SCF_SHOW_TABLES;
    }
    ;

desc_table:
    DESC ID SEMICOLON {
      ssql->flag = SCF_DESC_TABLE;
      ssql->sstr.desc_table.relation_name = $2;
    }
    ;

create_index:		/*create index 语句的语法解析树*/
    CREATE INDEX ID ON ID LBRACE ID RBRACE SEMICOLON {
																							        ssql->flag=SCF_CREATE_INDEX;//"create_index";
																							        ssql->sstr.create_index.index_name =$3;
																							        ssql->sstr.create_index.relation_name =$5;
																							        ssql->sstr.create_index.attribute_name =$7;
																							    }
    ;

drop_index:			/*drop index 语句的语法解析树*/
    DROP INDEX ID  SEMICOLON {
											        ssql->flag=SCF_DROP_INDEX;//"drop_index";
											        ssql->sstr.drop_index.index_name =$3;
											    }
    ;
create_table:		/*create table 语句的语法解析树*/
    CREATE TABLE ID LBRACE attr_def attr_def_list RBRACE SEMICOLON {
	
																																		ssql->flag=SCF_CREATE_TABLE;//"create_table";
																																		ssql->sstr.create_table.relation_name =$3;
																																		ssql->sstr.create_table.attribute_count =valueleng;
																																		//临时变量清零	
																																		valueleng=0;
																																		
																																	    }
    ;
attr_def_list:
    /* empty */
    | COMMA attr_def attr_def_list {    }
    ;
    
attr_def:
    ID_get type LBRACE number RBRACE {
																ssql->sstr.create_table.attributes[valueleng].name =(char*)malloc(sizeof(char)); // TODO FATAL
																strcpy(ssql->sstr.create_table.attributes[valueleng].name,get_id); 
																ssql->sstr.create_table.attributes[valueleng].type = $2;  
																ssql->sstr.create_table.attributes[valueleng].length = $4;
																valueleng++;
															    }
    |ID_get type{
															ssql->sstr.create_table.attributes[valueleng].name=(char*)malloc(sizeof(char));
															strcpy(ssql->sstr.create_table.attributes[valueleng].name,get_id); 
															ssql->sstr.create_table.attributes[valueleng].type=$2;  
															ssql->sstr.create_table.attributes[valueleng].length=4;
															valueleng++;
														    }
    ;
number:
		NUMBER {$$ = $1;}
		;
type:
	INT_T { $$=2; }
       | STRING_T { $$=1; }
       | FLOAT_T { $$=3; }
       ;
ID_get:
	ID {
				 temp=$1; 
				for(i=0;i<MAX_NUM;i++){
				get_id[i]=*(temp+i);
				}
			}
	;

	
insert:				/*insert   语句的语法解析树*/
    INSERT INTO ID VALUES LBRACE value value_list RBRACE SEMICOLON {
        valueT[valueleng++] = *$6;

        ssql->flag=SCF_INSERT;//"insert";
        ssql->sstr.insertion.relation_name = $3;
        ssql->sstr.insertion.value_num = valueleng;
        for(i=0;i<valueleng;i++){
        ssql->sstr.insertion.values[i] = valueT[i];
      }

      //临时变量清零
      valueleng=0;
      //	free(valueT);
    }

value_list:
    /* empty */
    | COMMA value value_list  { 
  			 valueT[valueleng++] = *$2;
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
    DELETE FROM ID where SEMICOLON {
	
	ssql->flag=SCF_DELETE;//"delete";
	ssql->sstr.deletion.relation_name = $3;
	for(i=0;i<whereleng;i++){
	ssql->sstr.deletion.conditions[i]=wherecondi[i];
	}
	ssql->sstr.deletion.condition_num = whereleng;
	whereleng=0;	
    }
    ;
update:			/*  update 语句的语法解析树*/
    UPDATE ID SET ID EQ value where SEMICOLON{
	
											ssql->flag=SCF_UPDATE;//"update";
											ssql->sstr.update.relation_name = $2;
											ssql->sstr.update.value = *$6;
											ssql->sstr.update.attribute_name = $4;
											for(i=0;i<whereleng;i++){
											  ssql->sstr.update.conditions[i]=wherecondi[i];
											}
											ssql->sstr.update.condition_num =whereleng;
											whereleng=0;
										    }
    ;
select:				/*  select 语句的语法解析树*/
    SELECT select_attr FROM ID rel_list where SEMICOLON {
      
            ssql->sstr.selection.relations[fromleng++]=$4;

            for(i=0;i<whereleng;i++){
              ssql->sstr.selection.conditions[i]=wherecondi[i];
            }

            ssql->flag=SCF_SELECT;//"select";
            ssql->sstr.selection.attr_num = selectleng;
            ssql->sstr.selection.relation_num = fromleng;
            ssql->sstr.selection.condition_num =whereleng;

            //临时变量清零
            whereleng=0;
            fromleng=0;
            selectleng=0;
        }
        ;

select_attr:
    STAR {  // TODO 处理这里的内存泄露问题
         ssql->sstr.selection.attributes[selectleng].attribute_name ="*";
         ssql->sstr.selection.attributes[selectleng++].relation_name =NULL;
       }
    | ID attr_list {
        ssql->sstr.selection.attributes[selectleng].attribute_name=$1;
        ssql->sstr.selection.attributes[selectleng++].relation_name=NULL;
      }
  	| ID DOT ID attr_list {
        ssql->sstr.selection.attributes[selectleng].attribute_name=$3;
        ssql->sstr.selection.attributes[selectleng++].relation_name=$1;
      }
    ;
attr_list:
    /* empty */
    | COMMA ID attr_list {
     	  ssql->sstr.selection.attributes[selectleng].relation_name = NULL;
        ssql->sstr.selection.attributes[selectleng++].attribute_name=$2;
      }
    | COMMA ID DOT ID attr_list {
        ssql->sstr.selection.attributes[selectleng].attribute_name=$4;
        ssql->sstr.selection.attributes[selectleng++].relation_name=$2;
  	  }
  	;

rel_list:
    /* empty */
    | COMMA ID rel_list {	
				ssql->sstr.selection.relations[fromleng++]=$2;
		  }
    ;
where:
    /* empty */ 
    | WHERE condition condition_list {	
				wherecondi[whereleng++]=*$2;
			}
    ;
condition_list:
    /* empty */
    | AND condition condition_list {
				wherecondi[whereleng++]=*$2;
			}
    ;
condition:
    ID comOp value {
									$$ = ( Condition *)malloc(sizeof( Condition));
									$$->left_is_attr = 1;
									$$->left_attr.relation_name = NULL;
									$$->left_attr.attribute_name= $1;
									$$->comp = compOpT;
									$$->right_is_attr = 0;
									$$->right_attr.relation_name = NULL;
									$$->right_attr.attribute_name = NULL;
									$$->right_value = *$3;

								    }
		|value comOp value {
									$$ = ( Condition *)malloc(sizeof( Condition));
									$$->left_is_attr = 0;
									$$->left_attr.relation_name=NULL;
									$$->left_attr.attribute_name=NULL;
									$$->left_value = *$1;
									$$->comp = compOpT;
									$$->right_is_attr = 0;
									$$->right_attr.relation_name = NULL;
									$$->right_attr.attribute_name = NULL;
									$$->right_value = *$3;

								    }
		|ID comOp ID {
									$$=( Condition *)malloc(sizeof( Condition));
									$$->left_is_attr = 1;
									$$->left_attr.relation_name=NULL;
									$$->left_attr.attribute_name=$1;
									$$->comp = compOpT;
									$$->right_is_attr = 1;
									$$->right_attr.relation_name=NULL;
									$$->right_attr.attribute_name=$3;

								    }
    |value comOp ID{
									$$=( Condition *)malloc(sizeof( Condition));
									$$->left_is_attr = 0;
									$$->left_attr.relation_name=NULL;
									$$->left_attr.attribute_name=NULL;
									$$->left_value = *$1;
									$$->comp=compOpT;
									
									$$->right_is_attr = 1;
									$$->right_attr.relation_name=NULL;
									$$->right_attr.attribute_name=$3;
								
								    }
    |ID DOT ID comOp value{
									$$=( Condition *)malloc(sizeof( Condition));
									$$->left_is_attr = 1;
									$$->left_attr.relation_name=$1;
									$$->left_attr.attribute_name=$3;
									$$->comp=compOpT;
									$$->right_is_attr = 0;   //属性值
									$$->right_attr.relation_name=NULL;
									$$->right_attr.attribute_name=NULL;
									$$->right_value =*$5;			
							
    											}
    |value comOp ID DOT ID{
									$$=( Condition *)malloc(sizeof( Condition));
									$$->left_is_attr = 0;//属性值
									$$->left_attr.relation_name=NULL;
									$$->left_attr.attribute_name=NULL;
									$$->left_value = *$1;
									$$->comp =compOpT;
									$$->right_is_attr = 1;//属性
									$$->right_attr.relation_name = $3;
									$$->right_attr.attribute_name = $5;
									
    						}
    |ID DOT ID comOp ID DOT ID{
									$$=( Condition *)malloc(sizeof( Condition));
									$$->left_is_attr = 1;		//属性
									$$->left_attr.relation_name=$1;
									$$->left_attr.attribute_name=$3;
									$$->comp =compOpT;
									$$->right_is_attr = 1;		//属性
									$$->right_attr.relation_name=$5;
									$$->right_attr.attribute_name=$7;								
    											}
    ;

comOp:
  	  EQ { compOpT = EQUAL_TO; }
    | LT { compOpT = LESS_THAN; }
    | GT { compOpT = GREAT_THAN; }
    | LE { compOpT = LESS_EQUAL; }
    | GE { compOpT = GREAT_EQUAL; }
    | NE { compOpT = NOT_EQUAL; }
    ;

%%
//_____________________________________________________________________
int hust_parse( sqlstr *sqls){
	ssql=sqls;
	return yyparse();
}
