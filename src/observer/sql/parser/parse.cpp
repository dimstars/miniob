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
#include "sql/parser/parse.h"
#include "rc.h"
#include "common/log/log.h"

Selects *getSelects();
Condition *getCondition();
RelAttr *getRelAttr();
Value *getValue();
union Queries *getSqls();
Query *getSqlstr();
RC parse(char *st, Query *sqln);

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
union Queries *getSqls() {
  union Queries *sql;
  sql = (union Queries *)malloc(sizeof(union Queries));
  (sql)->selection = *getSelects();
  (sql)->errors = (char *)malloc(sizeof(char) * MAX_ERROR_MESSAGE);
  memset(sql->errors, 0, sizeof(char) * MAX_ERROR_MESSAGE);
  return sql;
}
Query *getSqlstr() {
  Query *sq;
  sq = (Query *)malloc(sizeof(Query));
  (sq)->flag = SCF_ERROR;
  (sq)->sstr = *getSqls();
  return sq;
}

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
unsigned int get_maxDay_of_ym(unsigned int year, unsigned int month) {
  int res = 0;
  switch(month) {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12: {
      res = 31;
    }
      break;
    case 2: {
      if((year % 100 == 0 && year % 400 == 0) || (year % 100 != 0 && year % 4 == 0)) res = 29;
      else res = 28;
    }
      break;
    case 4:
    case 6:
    case 9:
    case 11: {
      res = 30;
    }
      break;
    default: 
      break;
  }
  return res;
}

int check_date_legality(const char *v) {
  // TODO 没有考虑大小端
  unsigned int data = *(unsigned int*)v;
  unsigned int year = data >> 11, month = (data & 0x7c0) >> 6, day = data & 0x3f;
  if(month < 1 || month > 12 || day < 1 || day > get_maxDay_of_ym(year, month) || year > 2038 \
    || (year == 2038 && month > 2)) {
      return 0;
  }
  return 1;
}

int cmp_date(char *left, char *right) {
  // TODO 没有考虑大小端
  unsigned int data_l = *(unsigned int*)left;
  unsigned int data_r = *(unsigned int*)right;
  
  return data_l - data_r;
}

void aggregation_init_string(AggOp *agg, const char *relation_name, const char *attribute_name, AggType type) {
  RelAttr attr;
  if(relation_name != nullptr) {
    attr.relation_name = strdup(relation_name);
  } else {
    attr.relation_name = nullptr;
  }
  attr.attribute_name = strdup(attribute_name);
  agg->attribute = attr;
  agg->type = type;
}

void aggregation_init_integer(AggOp *agg, const char *relation_name, int attribute_name, AggType type) {
  RelAttr attr;
  if(relation_name != nullptr) {
    attr.relation_name = strdup(relation_name);
  } else {
    attr.relation_name = nullptr;
  }
  attr.attribute_name = strdup(std::to_string(attribute_name).c_str());
  agg->attribute = attr;
  agg->type = type;
}

void aggregation_destroy(AggOp *agg) {
  if(agg->attribute.relation_name != nullptr) {
    free(agg->attribute.relation_name);
  }
  agg->attribute.relation_name = nullptr;
  if(agg->attribute.attribute_name != nullptr) {
    free(agg->attribute.attribute_name);
  }
  agg->attribute.attribute_name = nullptr;
}

void relation_attr_init(RelAttr *relation_attr, const char *relation_name, const char *attribute_name) {
  if (relation_name != nullptr) {
    relation_attr->relation_name = strdup(relation_name);
  } else {
    relation_attr->relation_name = nullptr;
  }
  relation_attr->attribute_name = strdup(attribute_name);
}

void relation_attr_destroy(RelAttr *relation_attr) {
  free(relation_attr->relation_name);
  free(relation_attr->attribute_name);
  relation_attr->relation_name = nullptr;
  relation_attr->attribute_name = nullptr;
}

void value_init_integer(Value *value, int v) {
  value->type = INTS;
  value->data = malloc(sizeof(v));
  memcpy(value->data, &v, sizeof(v));
}
void value_init_float(Value *value, float v) {
  value->type = FLOATS;
  value->data = malloc(sizeof(v));
  memcpy(value->data, &v, sizeof(v));
}
void value_init_string(Value *value, const char *v) {
  value->type = CHARS;
  if(strlen(v) > 4096) {
    value->data = malloc(4097);
    memcpy(value->data, v, 4096);
  } else {
    value->data = strdup(v);
  }
}
void value_init_date(Value *value, const char *v) {
  value->type = DATES;
  const char *delim = "-";
  char tstr[12];
  strcpy(tstr, v);
  char *token = strtok(tstr, delim);
  unsigned int data = atoi(token) << 11;
  token = strtok(NULL, delim);
  data |= atoi(token) << 6;
  token = strtok(NULL, delim);
  data |= atoi(token);
  value->data = malloc(sizeof(data));
  memcpy(value->data, &data, sizeof(data));
}
void value_init_null(Value *value) {
  value->type = NULLS;
  value->data = nullptr;
}
void value_destroy(Value *value) {
  value->type = UNDEFINED;
  if(value->data) free(value->data);
  value->data = nullptr;
}

void condition_init(Condition *condition, CompOp comp,
                    int left_is_attr, RelAttr *left_attr, Value *left_value,
                    int right_is_attr, RelAttr *right_attr, Value *right_value) {
  condition->comp = comp;
  condition->left_is_attr = left_is_attr;
  if (left_is_attr) {
    condition->left_attr = *left_attr;
  } else {
    condition->left_value = *left_value;
  }

  condition->right_is_attr = right_is_attr;
  if (right_is_attr) {
    condition->right_attr = *right_attr;
  } else {
    condition->right_value = *right_value;
  }
  condition->sub_selects = nullptr;
  condition->left_exp = nullptr;
  condition->right_exp = nullptr;
}

void subquery_condition_init(Condition *condition, RelAttr *left_attr, Selects *selects, CompOp com_op) {
  condition->left_is_attr = 1;
  condition->right_is_attr = 0;
  condition->left_attr = *left_attr;
  condition->comp = com_op;
  condition->sub_selects = selects;
  condition->sub_selects_left = nullptr;
  condition->left_exp = nullptr;
  condition->right_exp = nullptr;
}

void expression_condition_init(Condition *condition, CompOp comp, CalExp *left_exp, CalExp *right_exp) {
  condition->left_is_attr = 0;
  condition->right_is_attr = 0;
  condition->comp = comp;
  condition->left_exp = left_exp;
  condition->right_exp = right_exp;
  condition->sub_selects = nullptr;
}

CalExp * expression_create(RelAttr *attr, Value *value, CalExp *left_exp, CalExp *right_exp, CalOp cal_op) {
  CalExp *exp = (CalExp *)malloc(sizeof(CalExp));
  memset(exp, 0, sizeof(CalExp));
  if (attr != nullptr) {
    exp->is_attr = 1;
    exp->attr = *attr;
  }
  else if (value != nullptr) {
    exp->value = *value;
  }
  else {
    exp->left_exp = left_exp;
    exp->right_exp = right_exp;
  }
  exp->cal_op = cal_op;
  return exp;
}

CompOp reverse(CompOp comp) {
  switch (comp)
  {
  case EQUAL_TO:
    return EQUAL_TO;
  case LESS_EQUAL:
    return GREAT_EQUAL;
  case NOT_EQUAL:
    return NOT_EQUAL;
  case LESS_THAN:
    return GREAT_THAN;
  case GREAT_EQUAL:
    return LESS_EQUAL;
  case GREAT_THAN:
    return LESS_THAN;
  default:
    return NO_OP;
  }
}

void condition_destroy(Condition *condition) {
  if (condition->left_is_attr) {
    relation_attr_destroy(&condition->left_attr);
  } else {
    value_destroy(&condition->left_value);
  }
  if (condition->right_is_attr) {
    relation_attr_destroy(&condition->right_attr);
  } else {
    value_destroy(&condition->right_value);
  }
}

void attr_info_init(AttrInfo *attr_info, const char *name, AttrType type, int length, int nullable) {
  attr_info->name = strdup(name);
  attr_info->type = type;
  attr_info->length = length;
  attr_info->nullable = nullable;
}
void attr_info_destroy(AttrInfo *attr_info) {
  free(attr_info->name);
  attr_info->name = nullptr;
}

void selects_init(Selects *selects, ...);
void selects_append_aggregation(Selects *selects, AggOp *agg) {
  selects->aggregations[selects->aggr_num++] = *agg;
}
void selects_append_attribute(Selects *selects, RelAttr *rel_attr) {
  selects->attributes[selects->attr_num++] = *rel_attr;
}
void selects_append_relation(Selects *selects, const char *relation_name) {
  selects->relations[selects->relation_num++] = strdup(relation_name);
}

void selects_append_conditions(Selects *selects, Condition conditions[], int condition_num) {
  assert(condition_num <= sizeof(selects->conditions)/sizeof(selects->conditions[0]));
  for (int i = 0; i < condition_num; i++) {
    selects->conditions[i] = conditions[i];
  }
  selects->condition_num = condition_num;
}

void selects_destroy(Selects *selects) {
  for (int i = 0; i < selects->aggr_num; i++) {
    aggregation_destroy(&selects->aggregations[i]);
  }
  selects->aggr_num = 0;
  
  for (int i = 0; i < selects->attr_num; i++) {
    relation_attr_destroy(&selects->attributes[i]);
  }
  selects->attr_num = 0;

  for (int i = 0; i < selects->relation_num; i++) {
    free(selects->relations[i]);
    selects->relations[i] = NULL;
  }
  selects->relation_num = 0;

  for (int i = 0; i < selects->condition_num; i++) {
    condition_destroy(&selects->conditions[i]);
  }
  selects->condition_num = 0;
}

void inserts_init(Inserts *inserts, const char *relation_name, Value values[][MAX_NUM], int tuple_num, int value_nums[]) {
  assert(tuple_num <= sizeof(inserts->values)/sizeof(inserts->values[0]));

  inserts->relation_name = strdup(relation_name);
  for (int i = 0; i < tuple_num; i++) {
    for (int j = 0; j < value_nums[i]; j++) {
      inserts->values[i][j] = values[i][j];
    }
    inserts->value_nums[i] = value_nums[i];
  }
  inserts->tuple_num = tuple_num;
}
void inserts_destroy(Inserts *inserts) {
  free(inserts->relation_name);
  inserts->relation_name = nullptr;

  for (int i = 0; i < inserts->tuple_num; i++) {
    for (int j = 0; j < inserts->value_nums[i]; j++) {
      value_destroy(&inserts->values[i][j]);
    }
    inserts->value_nums[i] = 0;
  }
  inserts->tuple_num = 0;
}

void deletes_init_relation(Deletes *deletes, const char *relation_name) {
  deletes->relation_name = strdup(relation_name);
}

void deletes_set_conditions(Deletes *deletes, Condition conditions[], int condition_num) {
  assert(condition_num <= sizeof(deletes->conditions)/sizeof(deletes->conditions[0]));
  for (int i = 0; i < condition_num; i++) {
    deletes->conditions[i] = conditions[i];
  }
  deletes->condition_num = condition_num;
}
void deletes_destroy(Deletes *deletes) {
  for (int i = 0; i < deletes->condition_num; i++) {
    condition_destroy(&deletes->conditions[i]);
  }
  deletes->condition_num = 0;
  free(deletes->relation_name);
  deletes->relation_name = nullptr;
}

void updates_init(Updates *updates, const char *relation_name, const char *attribute_name,
                  Value *value, Condition conditions[], int condition_num) {
  updates->relation_name = strdup(relation_name);
  updates->attribute_name = strdup(attribute_name);
  updates->value = *value;

  assert(condition_num <= sizeof(updates->conditions)/sizeof(updates->conditions[0]));
  for (int i = 0; i < condition_num; i++) {
    updates->conditions[i] = conditions[i];
  }
  updates->condition_num = condition_num;
}

void updates_destroy(Updates *updates) {
  free(updates->relation_name);
  free(updates->attribute_name);
  updates->relation_name = nullptr;
  updates->attribute_name = nullptr;

  value_destroy(&updates->value);

  for (int i = 0; i < updates->condition_num; i++) {
    condition_destroy(&updates->conditions[i]);
  }
  updates->condition_num = 0;
}

void create_table_append_attribute(CreateTable *create_table, AttrInfo *attr_info) {
  create_table->attributes[create_table->attribute_count++] = *attr_info;
}
void create_table_init_name(CreateTable *create_table, const char *relation_name) {
  create_table->relation_name = strdup(relation_name);
}
void create_table_destroy(CreateTable *create_table) {
  for (int i = 0; i < create_table->attribute_count; i++) {
    attr_info_destroy(&create_table->attributes[i]);
  }
  create_table->attribute_count = 0;
  free(create_table->relation_name);
  create_table->relation_name = nullptr;
}

void drop_table_init(DropTable *drop_table, const char *relation_name) {
  drop_table->relation_name = strdup(relation_name);
}
void drop_table_destroy(DropTable *drop_table) {
  free(drop_table->relation_name);
  drop_table->relation_name = nullptr;
}

void create_index_init(CreateIndex *create_index, const char *index_name, 
                       const char *relation_name) {
  create_index->index_name = strdup(index_name);
  create_index->relation_name = strdup(relation_name);
}

void create_index_append_attribute(CreateIndex *create_index, char *attr_name) {
  create_index->attribute_names[create_index->attribute_count++] = strdup(attr_name);
}

void create_index_destroy(CreateIndex *create_index) {
  free(create_index->index_name);
  free(create_index->relation_name);
  create_index->index_name = nullptr;
  create_index->relation_name = nullptr;
  for(int i = 0; i < create_index->attribute_count; i++){
    free(create_index->attribute_names[i]);
    create_index->attribute_names[i] = nullptr;
  }
  create_index->attribute_count = 0;
}

void drop_index_init(DropIndex *drop_index, const char *index_name) {
  drop_index->index_name = strdup(index_name);
}
void drop_index_destroy(DropIndex *drop_index) {
  free((char *)drop_index->index_name);
  drop_index->index_name = nullptr;
}

void desc_table_init(DescTable *desc_table, const char *relation_name) {
  desc_table->relation_name = strdup(relation_name);
}

void desc_table_destroy(DescTable *desc_table) {
  free((char *)desc_table->relation_name);
  desc_table->relation_name = nullptr;
}

void load_data_init(LoadData *load_data, const char *relation_name, const char *file_name) {
  load_data->relation_name = strdup(relation_name);

  if (file_name[0] == '\'' || file_name[0] == '\"') {
    file_name++;
  }
  char *dup_file_name = strdup(file_name);
  int len = strlen(dup_file_name);
  if (dup_file_name[len - 1] == '\'' || dup_file_name[len - 1] == '\"') {
    dup_file_name[len - 1] = 0;
  }
  load_data->file_name = dup_file_name;
}

void load_data_destroy(LoadData *load_data) {
  free((char *)load_data->relation_name);
  free((char *)load_data->file_name);
  load_data->relation_name = nullptr;
  load_data->file_name = nullptr;
}

void query_init(Query *query) {
  query->flag = SCF_ERROR;
  memset(&query->sstr, 0, sizeof(query->sstr));
}

Query *query_create() {
  Query *query = (Query *)malloc(sizeof(Query));
  if (nullptr == query) {
    LOG_ERROR("Failed to alloc memroy for query. size=%d", sizeof(Query));
    return nullptr;
  }

  query_init(query);
  return query;
}

void query_reset(Query *query) {
  switch (query->flag) {
    case SCF_SELECT: {
      selects_destroy(&query->sstr.selection);
    }
    break;
    case SCF_INSERT: {
      inserts_destroy(&query->sstr.insertion);
    }
    break;
    case SCF_DELETE: {
      deletes_destroy(&query->sstr.deletion);
    }
    break;
    case SCF_UPDATE: {
      updates_destroy(&query->sstr.update);
    }
    break;
    case SCF_CREATE_TABLE: {
      create_table_destroy(&query->sstr.create_table);
    }
    break;
    case SCF_DROP_TABLE: {
      drop_table_destroy(&query->sstr.drop_table);
    }
    break;
    case SCF_CREATE_UNIQUE_INDEX:
    case SCF_CREATE_INDEX: {
      create_index_destroy(&query->sstr.create_index);
    }
    break;
    case SCF_DROP_INDEX: {
      drop_index_destroy(&query->sstr.drop_index);
    }
    break;
    case SCF_SYNC: {

    }
    break;
    case SCF_SHOW_TABLES:
    break;

    case SCF_DESC_TABLE: {
      desc_table_destroy(&query->sstr.desc_table);
    }
    break;

    case SCF_LOAD_DATA: {
      load_data_destroy(&query->sstr.load_data);
    }
    break;
    case SCF_BEGIN:
    case SCF_COMMIT:
    case SCF_ROLLBACK:
    case SCF_HELP:
    case SCF_EXIT:
    case SCF_ERROR:
    break;
  }
}

void query_destroy(Query *query) {
  query_reset(query);
  free(query);
}
#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////

extern "C" int sql_parse(const char *st, Query  *sqls);

RC parse(const char *st, Query *sqln) {
  sql_parse(st, sqln);

  if (sqln->flag == SCF_ERROR)
    return SQL_SYNTAX;
  else
    return SUCCESS;
}