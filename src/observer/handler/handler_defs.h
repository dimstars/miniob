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
// Created by Longda on 2021/4/14.
//

#ifndef __OBSERVER_HANDLER_HANDLER_DEFS_H__
#define __OBSERVER_HANDLER_HANDLER_DEFS_H__

#include <stdlib.h>
#include <string.h>

#define MAX_NUM 20
#define MAX_REL_NAME 20
#define MAX_ATTR_NAME 20
#define MAX_ERROR_MESSAGE 20
#define MAX_DATA 50

//属性结构体
typedef struct {
  char *relName;  // relation name (may be NULL) 表名
  char *attrName; // attribute name              属性名
} RelAttr;

typedef enum {
  EQual,  //"="			0
  LEqual, //"<="          1
  NEqual, //"<>"			2
  LessT,  //"<"			3
  GEqual, //">="			4
  GreatT, //">"           5
  NO_OP
} CompOp;

//属性值类型
typedef enum { 
  undefined,
  chars, 
  ints, 
  floats 
} AttrType;

//属性值
typedef struct _Value {
  AttrType type; // type of value
  void *data;    // value
} Value;

typedef struct _Condition {
  int bLhsIsAttr;  // TRUE if left-hand side is an attribute
                   // 1时，操作符左边是属性名，0时，是属性值
  Value lhsValue;  // left-hand side value if bLhsIsAttr = FALSE
  RelAttr lhsAttr; // left-hand side attribute
  CompOp op;       // comparison operator
  int bRhsIsAttr;  // TRUE if right-hand side is an attribute
                   // 1时，操作符右边是属性名，0时，是属性值
  //   and not a value
  RelAttr rhsAttr; // right-hand side attribute if bRhsIsAttr = TRUE 右边的属性
  Value rhsValue; // right-hand side value if bRhsIsAttr = FALSE
} Condition;

// struct of select
typedef struct {
  int nSelAttrs;                 // Length of attrs in Select clause
  RelAttr selAttrs[MAX_NUM];     // attrs in Select clause
  int nRelations;                // Length of relations in Fro clause
  char *relations[MAX_NUM];      // relations in From clause
  int nConditions;               // Length of conditions in Where clause
  Condition conditions[MAX_NUM]; // conditions in Where clause
} Selects;

// struct of insert
typedef struct {
  char *relName;         // Relation to insert into
  int nValues;           // Length of values
  Value values[MAX_NUM]; // values to insert
}Inserts;

// struct of delete
typedef struct {
  char *relName;                 // Relation to delete from
  int nConditions;               // Length of conditions in Where clause
  Condition conditions[MAX_NUM]; // conditions in Where clause
} Deletes;

// struct of update
typedef struct {
  char *relName;                 // Relation to update
  char *attrName;                // Attribute to update
  Value value;                   // update value
  int nConditions;               // Length of conditions in Where clause
  Condition conditions[MAX_NUM]; // conditions in Where clause
} Updates;

typedef struct  {
  char *attrName;    // Attribute name
  AttrType attrType; // Type of attribute
  int attrLength;    // Length of attribute
} AttrInfo;

// struct of craete_table
typedef struct {
  char *relName;                // Relation name
  int attrCount;                // Length of attribute
  AttrInfo attributes[MAX_NUM]; // attributes
} CreateTable;

// struct of drop_table
typedef struct {
  char *relName; // Relation name
}DropTable;

// struct of create_index
typedef struct  {
  char *indexName; // Index name
  char *relName;   // Relation name
  char *attrName;  // Attribute name
}CreateIndex;

// struct of  drop_index
typedef struct  {
  const char *indexName; // Index name
} DropIndex;

typedef struct {
  const char *table_name;
} DescTable;

// union of sql_structs
union sqls {
  Selects sel;
  Inserts ins;
  Deletes del;
  Updates upd;
  CreateTable cret;
  DropTable drt;
  CreateIndex crei;
  DropIndex dri;
  DescTable desc_table;
  char *errors;
};

// TODO 修改yacc中相关数字编码为宏定义
enum SqlCommandFlag {
  SCF_ERROR = 0,     // 这里的编码与yacc中需要匹配起来。目前yacc中直接使用的数字，而不是宏定义
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
typedef struct _sqlstr {
  enum SqlCommandFlag flag;
  union sqls sstr;
}sqlstr;

#endif //__OBSERVER_HANDLER_HANDLER_DEFS_H__
