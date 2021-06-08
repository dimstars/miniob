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

//属性结构体
typedef struct {
  char *relation_name;  // relation name (may be NULL) 表名
  char *attribute_name; // attribute name              属性名
} RelAttr;

typedef enum {
  EQUAL_TO,  //"="			0
  LESS_EQUAL, //"<="          1
  NOT_EQUAL, //"<>"			2
  LESS_THAN,  //"<"			3
  GREAT_EQUAL, //">="			4
  GREAT_THAN, //">"           5
  NO_OP
} CompOp;

//属性值类型
typedef enum { 
  UNDEFINED,
  CHARS, 
  INTS, 
  FLOATS 
} AttrType;

//属性值
typedef struct _Value {
  AttrType type;    // type of value
  void *   data;    // value
} Value;

typedef struct _Condition {
  int     left_is_attr;  // TRUE if left-hand side is an attribute
                         // 1时，操作符左边是属性名，0时，是属性值
  Value   left_value;    // left-hand side value if left_is_attr = FALSE
  RelAttr left_attr;     // left-hand side attribute
  CompOp  comp;          // comparison operator
  int     right_is_attr; // TRUE if right-hand side is an attribute
                         // 1时，操作符右边是属性名，0时，是属性值
  RelAttr right_attr;    // right-hand side attribute if right_is_attr = TRUE 右边的属性
  Value   right_value;   // right-hand side value if right_is_attr = FALSE
} Condition;

// struct of select
typedef struct {
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
  int value_num;                   // Length of values
  Value values[MAX_NUM];           // values to insert
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
  char *attribute_name;            // Attribute name
}CreateIndex;

// struct of  drop_index
typedef struct  {
  const char *index_name;         // Index name
} DropIndex;

typedef struct {
  const char *relation_name;
} DescTable;

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
  SCF_CREATE_INDEX,
  SCF_DROP_INDEX,
  SCF_SYNC,
  SCF_SHOW_TABLES,
  SCF_DESC_TABLE,
  SCF_BEGIN,
  SCF_COMMIT,
  SCF_ROLLBACK,
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

void desc_table_init(DescTable *desc_table, const char *relation_name);
void desc_table_destroy(DescTable *desc_table);

void query_init(Query *query);
Query *query_create(); // create and init
void query_reset(Query *query);
void query_destroy(Query *query); // reset and delete

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __OBSERVER_SQL_PARSER_PARSE_DEFS_H__