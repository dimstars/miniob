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
// Created by Longda on 2021/4/13.
//

#include <mutex>
#include "handler/handler_defs.h"
#include "yacc_sql.tab.h"
#include "rc.h"

Selects *getSelects();
Condition *getCondition();
RelAttr *getRelAttr();
Value *getValue();
union sqls *getSqls();
sqlstr *getSqlstr();
RC parse(char *st, sqlstr *sqln);

int i;
// allocate struct of condition
Condition *getCondition() {
  Condition *condition;
  condition = (Condition *)malloc(sizeof(Condition));
  condition->left_attr = *getRelAttr();
  condition->right_attr = *getRelAttr();
  condition->left_value = *getValue();
  condition->right_value = *getValue();
  condition->left_is_attr = -1;
  condition->right_is_attr = -1;
  return condition;
}
// allocate struct of relattr
RelAttr *getRelAttr() {
  RelAttr *relAttr;
  relAttr = (RelAttr *)malloc(sizeof(RelAttr));
  relAttr->relation_name = (char *)malloc(sizeof(char) * MAX_REL_NAME);
  memset(relAttr->relation_name, 0, sizeof(char) * MAX_REL_NAME);
  relAttr->attribute_name = (char *)malloc(sizeof(char) * MAX_ATTR_NAME);
  memset(relAttr->attribute_name, 0, sizeof(char) * MAX_ATTR_NAME);
  return relAttr;
}
// allocate struct of value,the default type is int
Value *getValue() {
  Value *value = (Value *)malloc(sizeof(Value));
  value->data = (void *)malloc(sizeof(char) * MAX_DATA);
  memset(value->data, 0, sizeof(char) * MAX_DATA);
  value->type = INTS;
  return value;
}
// allocate struct of select
Selects *getSelects() {
  Selects *se;
  se = (Selects *)malloc(sizeof(Selects));
  (se)->attr_num = 0;
  /*
  for (i = 0; i < MAX_NUM; i++) {
    (se)->selAttrs[i] = getRelAttr();
  }
   */
  (se)->relation_num = 0;
  for (i = 0; i < MAX_NUM; i++) {
    (se)->relations[i] = (char *)malloc(sizeof(char));
    memset(se->relations[i], 0, sizeof(char));
  }
  (se)->condition_num = 0;
  for (i = 0; i < MAX_NUM; i++) {
    (se)->conditions[i] = *getCondition();
  }
  return se;
}
// allocate union of sqls
union sqls *getSqls() {
  union sqls *sql;
  sql = (union sqls *)malloc(sizeof(union sqls));
  (sql)->selection = *getSelects();
  (sql)->errors = (char *)malloc(sizeof(char) * MAX_ERROR_MESSAGE);
  memset(sql->errors, 0, sizeof(char) * MAX_ERROR_MESSAGE);
  return sql;
}
sqlstr *getSqlstr() {
  sqlstr *sq;
  sq = (sqlstr *)malloc(sizeof(sqlstr));
  (sq)->flag = SCF_ERROR;
  (sq)->sstr = *getSqls();
  return sq;
}

extern "C" int hust_parse(sqlstr *sqls);
extern "C" void scan_string(const char *str);

RC parse(const char *st, sqlstr *sqln) {

  scan_string(st);

  if (sqln->flag == -1) {
    sqln->sstr = *getSqls();
  }

  static std::mutex lock;
  lock.lock();
  hust_parse(sqln);
  lock.unlock();

  if (sqln->flag == 0)
    return SQL_SYNTAX;
  else
    return SUCCESS;
}