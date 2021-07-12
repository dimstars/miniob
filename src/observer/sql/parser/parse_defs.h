/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   51 Franklin Street, Suite 500, Boston, MA 02110-1335 USA */

//
// Created by wangyunlai.wyl on 2021/6/7.
//

#ifndef __OBSERVER_SQL_PARSER_PARSE_DEFS_H__
#define __OBSERVER_SQL_PARSER_PARSE_DEFS_H__

#define MAX_NUM 20
#define MAX_REL_NAME 20
#define MAX_ATTR_NAME 20
#define MAX_ERROR_MESSAGE 20
#define MAX_DATA 50
#define RECORD_BITMAP 4 // 4字节，32bit的bitmap
#define RECORD_BITMAP_BITS RECORD_BITMAP * 8
//属性结构体
typedef struct {
  char *relation_name;  // relation name (may be NULL) 表名
  char *attribute_name; // attribute name              属性（列）名
} RelAttr;

typedef enum {
  EQUAL_TO,  //"=" or is null         0 
  LESS_EQUAL, //"<="                  1
  NOT_EQUAL, //"<>" or is not null    2
  LESS_THAN,  //"<"                   3
  GREAT_EQUAL, //">="                 4
  GREAT_THAN, //">"                   5
  WHERE_IN, //"in"                    6
  IS_NULL,
  IS_NOT_NULL,
  NO_OP
} CompOp;

typedef enum {
  PLUS,
  MINUS,
  MULT,
  DIV,
  NO_CAL_OP
} CalOp;

//属性值类型
typedef enum { 
  UNDEFINED,
  CHARS, 
  INTS, 
  FLOATS,
  TEXTS,
  DATES, // 0x(yyyyyyyyyyyyyymmmmmdddddd) // 14bit|5bit|6bit // 9999-19-39
  NULLS
} AttrType;

//聚合运算类型
typedef enum {
  MAX_A,
  MIN_A,
  COUNT_A,
  AVG_A,
  NOTHING_A
} AggType;

//聚合运算符(带属性)
typedef struct _AggOp {
   AggType type;
   RelAttr attribute;
} AggOp;

//属性值
typedef struct _Value {
  AttrType type;    // type of value
  void *   data;    // value
} Value;

typedef struct CalExp {
  int            is_attr;
  RelAttr        attr;
  Value          value;
  struct CalExp *left_exp;
  struct CalExp *right_exp;
  CalOp          cal_op;
} CalExp;

typedef struct _Condition {
  int     left_is_attr;  // TRUE if left-hand side is an attribute
                         // 1时，操作符左边是属性名，0时，是属性值
  Value   left_value;    // left-hand side value if left_is_attr = FALSE
  RelAttr left_attr;     // left-hand side attribute
  CalExp *left_exp;
  CompOp  comp;          // comparison operator
  int     right_is_attr; // TRUE if right-hand side is an attribute
                         // 1时，操作符右边是属性名，0时，是属性值
  RelAttr right_attr;    // right-hand side attribute if right_is_attr = TRUE 右边的属性
  Value   right_value;   // right-hand side value if right_is_attr = FALSE
  CalExp *right_exp;
  struct Selects *sub_selects;  // subquery if not null
} Condition;

// struct of select
typedef struct Selects {
  int   aggr_num;                  // Size of aggrs in Select clause
  AggOp aggregations[MAX_NUM];     // aggregations operator
  int attr_num;                    // Length of attrs in Select clause
  RelAttr attributes[MAX_NUM];     // attrs in Select clause
  int relation_num;                // Length of relations in Fro clause
  char *relations[MAX_NUM];        // relations in From clause
  int condition_num;               // Length of conditions in Where clause
  Condition conditions[MAX_NUM];   // conditions in Where clause
} Selects;

// struct of insert
typedef struct {
  char *relation_name;             // Relation to insert into
  int tuple_num;
  int value_nums[MAX_NUM];                  // Length of values
  Value values[MAX_NUM][MAX_NUM];           // values to insert
}Inserts;

// struct of delete
typedef struct {
  char *relation_name;             // Relation to delete from
  int condition_num;               // Length of conditions in Where clause
  Condition conditions[MAX_NUM];   // conditions in Where clause
} Deletes;

// struct of update
typedef struct {
  char *relation_name;             // Relation to update
  char *attribute_name;            // Attribute to update
  Value value;                     // update value
  int condition_num;               // Length of conditions in Where clause
  Condition conditions[MAX_NUM];   // conditions in Where clause
} Updates;

typedef struct  {
  char *name;                      // Attribute name
  AttrType type;                   // Type of attribute
  int length;                      // Length of attribute
  int nullable;                    // whether support null
} AttrInfo;

// struct of craete_table
typedef struct {
  char *relation_name;             // Relation name
  int attribute_count;             // Length of attribute
  AttrInfo attributes[MAX_NUM];    // attributes
} CreateTable;

// struct of drop_table
typedef struct {
  char *relation_name;             // Relation name
}DropTable;

// struct of create_index
typedef struct  {
  char *index_name;                // Index name
  char *relation_name;             // Relation name
  int attribute_count;             // Length of attribute
  char *attribute_names[MAX_NUM];            // Attribute name
}CreateIndex;

// struct of  drop_index
typedef struct  {
  const char *index_name;         // Index name
} DropIndex;

typedef struct {
  const char *relation_name;
} DescTable;

typedef struct {
  const char *relation_name;
  const char *file_name;
} LoadData;

union Queries {
  Selects selection;
  Inserts insertion;
  Deletes deletion;
  Updates update;
  CreateTable create_table;
  DropTable drop_table;
  CreateIndex create_index;
  DropIndex drop_index;
  DescTable desc_table;
  LoadData load_data;
  char *errors;
};

// TODO 修改yacc中相关数字编码为宏定义
enum SqlCommandFlag {
  SCF_ERROR = 0,
  SCF_SELECT,
  SCF_INSERT,
  SCF_UPDATE,
  SCF_DELETE,
  SCF_CREATE_TABLE,
  SCF_DROP_TABLE,
  SCF_CREATE_UNIQUE_INDEX,
  SCF_CREATE_INDEX,
  SCF_DROP_INDEX,
  SCF_SYNC,
  SCF_SHOW_TABLES,
  SCF_DESC_TABLE,
  SCF_BEGIN,
  SCF_COMMIT,
  SCF_ROLLBACK,
  SCF_LOAD_DATA,
  SCF_HELP,
  SCF_EXIT
};
// struct of flag and sql_struct
typedef struct Query {
  enum SqlCommandFlag flag;
  union Queries sstr;
} Query;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
unsigned int get_maxDay_of_ym(unsigned int year, unsigned int month);
int check_date_legality(const char *v);
int cmp_date(char *left, char *right);

void aggregation_init_string(AggOp *agg, const char *relation_name, const char *attribute_name, AggType type);
void aggregation_init_integer(AggOp *agg, const char *relation_name, int attribute_name, AggType type);
void aggregation_destroy(AggOp *agg);

void relation_attr_init(RelAttr *relation_attr, const char *relation_name, const char *attribute_name);
void relation_attr_destroy(RelAttr *relation_attr);

void value_init_integer(Value *value, int v);
void value_init_float(Value *value, float v);
void value_init_string(Value *value, const char *v);
void value_init_date(Value *value, const char *v);
void value_init_null(Value *value);
void value_destroy(Value *value);

void condition_init(Condition *condition, CompOp comp, int left_is_attr, RelAttr *left_attr, Value *left_value,
                    int right_is_attr, RelAttr *right_attr, Value *right_value);
void subquery_condition_init(Condition *condition, RelAttr *left_attr, Selects *selects);
void condition_destroy(Condition *condition);
void expression_condition_init(Condition *condition, CompOp comp, CalExp *left_exp, CalExp *right_exp);
CalExp * expression_create(RelAttr *attr, Value *value, CalExp *left_exp, CalExp *right_exp, CalOp cal_op);

void attr_info_init(AttrInfo *attr_info, const char *name, AttrType type, int length, int nullable);
void attr_info_destroy(AttrInfo *attr_info);

void selects_init(Selects *selects, ...);
void selects_append_aggregation(Selects *selects, AggOp *agg);
void selects_append_attribute(Selects *selects, RelAttr *rel_attr);
void selects_append_relation(Selects *selects, const char *relation_name);
void selects_append_conditions(Selects *selects, Condition conditions[], int condition_num);
void selects_destroy(Selects *selects);

void inserts_init(Inserts *inserts, const char *relation_name, Value values[][MAX_NUM], int tuple_num, int value_nums[]);
void inserts_destroy(Inserts *inserts);

void deletes_init_relation(Deletes *deletes, const char *relation_name);
void deletes_set_conditions(Deletes *deletes, Condition conditions[], int condition_num);
void deletes_destroy(Deletes *deletes);

void updates_init(Updates *updates, const char *relation_name, const char *attribute_name,
                  Value *value, Condition conditions[], int condition_num);
void updates_destroy(Updates *updates);

void create_table_append_attribute(CreateTable *create_table, AttrInfo *attr_info);
void create_table_init_name(CreateTable *create_table, const char *relation_name);
void create_table_destroy(CreateTable *create_table);

void drop_table_init(DropTable *drop_table, const char *relation_name);
void drop_table_destroy(DropTable *drop_table);

void create_index_init(CreateIndex *create_index, const char *index_name, 
                       const char *relation_name);
void create_index_append_attribute(CreateIndex *create_index, char *attr_name);

void create_index_destroy(CreateIndex *create_index);

void drop_index_init(DropIndex *drop_index, const char *index_name);
void drop_index_destroy(DropIndex *drop_index);

void desc_table_init(DescTable *desc_table, const char *relation_name);
void desc_table_destroy(DescTable *desc_table);

void load_data_init(LoadData *load_data, const char *relation_name, const char *file_name);
void load_data_destroy(LoadData *load_data);

void query_init(Query *query);
Query *query_create(); // create and init
void query_reset(Query *query);
void query_destroy(Query *query); // reset and delete

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __OBSERVER_SQL_PARSER_PARSE_DEFS_H__