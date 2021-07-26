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

#include <string>
#include <sstream>

#include "execute_stage.h"

#include "common/io/io.h"
#include "common/log/log.h"
#include "common/seda/timer_stage.h"
#include "common/lang/string.h"
#include "session/session.h"
#include "event/storage_event.h"
#include "event/sql_event.h"
#include "event/session_event.h"
#include "event/execution_plan_event.h"
#include "sql/executor/execution_node.h"
#include "sql/executor/tuple.h"
#include "storage/common/table.h"
#include "storage/default/default_handler.h"
#include "storage/common/condition_filter.h"
#include "storage/trx/trx.h"

using namespace common;

RC create_selection_executor(Trx *trx, const Selects &selects, const char *db, const char *table_name, int table_num, std::vector<TupleSet> &sub_tuple_sets, SelectExeNode &select_node);
RC do_sub_select(const char *db, const Selects &selects, SessionEvent *session_event, TupleSet &tuple_set, bool is_complex);

//! Constructor
ExecuteStage::ExecuteStage(const char *tag) : Stage(tag) {}

//! Destructor
ExecuteStage::~ExecuteStage() {}

//! Parse properties, instantiate a stage object
Stage *ExecuteStage::make_stage(const std::string &tag) {
  ExecuteStage *stage = new (std::nothrow) ExecuteStage(tag.c_str());
  if (stage == nullptr) {
    LOG_ERROR("new ExecuteStage failed");
    return nullptr;
  }
  stage->set_properties();
  return stage;
}

//! Set properties for this object set in stage specific properties
bool ExecuteStage::set_properties() {
  //  std::string stageNameStr(stageName);
  //  std::map<std::string, std::string> section = theGlobalProperties()->get(
  //    stageNameStr);
  //
  //  std::map<std::string, std::string>::iterator it;
  //
  //  std::string key;

  return true;
}

//! Initialize stage params and validate outputs
bool ExecuteStage::initialize() {
  LOG_TRACE("Enter");

  std::list<Stage *>::iterator stgp = next_stage_list_.begin();
  default_storage_stage_ = *(stgp++);
  mem_storage_stage_ = *(stgp++);

  LOG_TRACE("Exit");
  return true;
}

//! Cleanup after disconnection
void ExecuteStage::cleanup() {
  LOG_TRACE("Enter");

  LOG_TRACE("Exit");
}

void ExecuteStage::handle_event(StageEvent *event) {
  LOG_TRACE("Enter\n");

  handle_request(event);

  LOG_TRACE("Exit\n");
  return;
}

void ExecuteStage::callback_event(StageEvent *event, CallbackContext *context) {
  LOG_TRACE("Enter\n");

  // TODO, here finish read all data from disk or network, but do nothing here.
  ExecutionPlanEvent *exe_event = static_cast<ExecutionPlanEvent *>(event);
  SQLStageEvent *sql_event = exe_event->sql_event();
  sql_event->done_immediate();

  LOG_TRACE("Exit\n");
  return;
}

void ExecuteStage::handle_request(common::StageEvent *event) {
  ExecutionPlanEvent *exe_event = static_cast<ExecutionPlanEvent *>(event);
  SessionEvent *session_event = exe_event->sql_event()->session_event();
  Query *sql = exe_event->sqls();
  const char *current_db = session_event->get_client()->session->get_current_db().c_str();

  CompletionCallback *cb = new (std::nothrow) CompletionCallback(this, nullptr);
  if (cb == nullptr) {
    LOG_ERROR("Failed to new callback for ExecutionPlanEvent");
    exe_event->done_immediate();
    return;
  }
  exe_event->push_callback(cb);

  switch (sql->flag) {
    case SCF_SELECT: { // select
      RC rc = do_select(current_db, sql, exe_event->sql_event()->session_event());
      if (rc != RC::SUCCESS) {
        char response[256];
        snprintf(response, sizeof(response), "%s\n", "FAILURE");
        session_event->set_response(strrc(rc));
      }
      exe_event->done_immediate();
    }
    break;

    case SCF_INSERT:
    case SCF_UPDATE:
    case SCF_DELETE:
    case SCF_CREATE_TABLE:
    case SCF_SHOW_TABLES:
    case SCF_DESC_TABLE:
    case SCF_DROP_TABLE:
    case SCF_CREATE_INDEX:
    case SCF_CREATE_UNIQUE_INDEX:
    case SCF_DROP_INDEX: 
    case SCF_LOAD_DATA: {
      StorageEvent *storage_event = new (std::nothrow) StorageEvent(exe_event);
      if (storage_event == nullptr) {
        LOG_ERROR("Failed to new StorageEvent");
        event->done_immediate();
        return;
      }

      default_storage_stage_->handle_event(storage_event);
    }
    break;
    case SCF_SYNC: {
      RC rc = DefaultHandler::get_default().sync();
      session_event->set_response(strrc(rc));
      exe_event->done_immediate();
    }
    break;
    case SCF_BEGIN: {
      session_event->get_client()->session->set_trx_multi_operation_mode(true);
      session_event->set_response(strrc(RC::SUCCESS));
      exe_event->done_immediate();
    }
    break;
    case SCF_COMMIT: {
      Trx *trx = session_event->get_client()->session->current_trx();
      RC rc = trx->commit();
      session_event->get_client()->session->set_trx_multi_operation_mode(false);
      session_event->set_response(strrc(rc));
      exe_event->done_immediate();
    }
    break;
    case SCF_ROLLBACK: {
      Trx *trx = session_event->get_client()->session->current_trx();
      RC rc = trx->rollback();
      session_event->get_client()->session->set_trx_multi_operation_mode(false);
      session_event->set_response(strrc(rc));
      exe_event->done_immediate();
    }
    break;
    case SCF_HELP: {
      const char *response = "show tables;\n"
          "desc `table name`;\n"
          "create table `table name` (`column name` `column type`, ...);\n"
          "create index `index name` on `table` (`column`);\n"
          "insert into `table` values(`value1`,`value2`);\n"
          "update `table` set column=value [where `column`=`value`];\n"
          "delete from `table` [where `column`=`value`];\n"
          "select [ * | `columns` ] from `table`;\n";
      session_event->set_response(response);
      exe_event->done_immediate();
    }
    break;
    case SCF_EXIT: {
      // do nothing
      const char *response = "Unsupported\n";
      session_event->set_response(response);
      exe_event->done_immediate();
    }
    break;
    default: {
      exe_event->done_immediate();
      LOG_ERROR("Unsupported command=%d\n", sql->flag);
    }
  }
}

void end_trx_if_need(Session *session, Trx *trx, bool all_right) {
  if (!session->is_trx_multi_operation_mode()) {
    if (all_right) {
      trx->commit();
    } else {
      trx->rollback();
    }
  }
}

RC join_tables(std::vector<TupleSet> &tuple_sets, std::vector<JoinConditionFilter *> &join_condition_filters, TupleSet &join_tuple_set) {
  // 构建schema
  std::vector<int> tuple_sizes;
  TupleSchema schema;
  int cycles = 1;
  for (TupleSet &tuple_set: tuple_sets) {
    cycles *= tuple_set.size();
    tuple_sizes.push_back(tuple_set.size());
    schema.append(tuple_set.schema());
  }
  join_tuple_set.set_schema(schema);

  // 若任意TupleSet为空，那么笛卡尔积为空
  for (TupleSet &tuple_set: tuple_sets) {
    if (tuple_set.size() == 0) {
      return RC::SUCCESS;
    }
  }

  // 构建笛卡尔积
  std::vector<int> tuple_nums(tuple_sets.size(), 0);
  for (int i = 0; i < cycles; i++) {
    Tuple tuple;
    for (int j = 0; j < tuple_sets.size(); j++) {
      TupleSet &tuple_set = tuple_sets[j];
      const Tuple &tuple_tmp = tuple_set.get(tuple_nums[j]);
      for (std::shared_ptr<TupleValue> value: tuple_tmp.values()) {
        tuple.add(value);
      }
    }
    tuple_nums[tuple_sets.size() - 1] += 1;
    for (int j = tuple_sets.size() - 1; j > 0; j--) {
      if (tuple_nums[j] >= tuple_sizes[j]) {
        tuple_nums[j-1] += 1;
        tuple_nums[j] = 0;
      }
    }
    // 根据条件进行筛选
    bool flag = true;
    for (JoinConditionFilter *filter: join_condition_filters) {
      if (!filter->filter(schema, tuple)) {
        flag = false;
        break;
      }
    }
    if (flag) {
      join_tuple_set.add(std::move(tuple));
    }
    if (tuple_nums[0] >= tuple_sizes[0]) {
      break;
    }
  }

  return RC::SUCCESS;
}

RC create_join_condition_filter(const char * db, const Selects &selects, std::vector<JoinConditionFilter*> &join_condition_filters) {
  for (int i = 0; i < selects.condition_num; i++) {
    const Condition &condition = selects.conditions[i];
    // 两边都是表名加属性名，是多表之间比较的条件表达式
    if (condition.left_is_attr == 1 && condition.right_is_attr == 1) {
      Table * left_table = DefaultHandler::get_default().find_table(db, condition.left_attr.relation_name);
      Table * right_table = DefaultHandler::get_default().find_table(db, condition.right_attr.relation_name);
      if (left_table == nullptr || right_table == nullptr) {
        return RC::SCHEMA_TABLE_NOT_EXIST;
      }
      JoinConditionFilter *join_condition_filter = new JoinConditionFilter();
      RC rc = join_condition_filter->init(left_table, right_table, condition);
      if (rc != RC::SUCCESS) {
        delete join_condition_filter;
        return rc;
      }
      join_condition_filters.push_back(join_condition_filter);
    }
    else if (condition.right_exp != nullptr) {
      LOG_INFO("complex expr join filter init");
      JoinConditionFilter *join_condition_filter = new JoinConditionFilter();
      RC rc = join_condition_filter->init(nullptr, nullptr, condition);
      if (rc != RC::SUCCESS) {
        delete join_condition_filter;
        return rc;
      }
      join_condition_filters.push_back(join_condition_filter);
    }
  }
  return RC::SUCCESS;
}

RC check_attr_in_table(std::vector<Table *> *tables, const RelAttr &attr, AttrType &type) {
  // 无表名，要么是“*”，要么是单表查询
  if (attr.relation_name == nullptr) {
    if (0 == strcmp(attr.attribute_name, "*")) {
      return RC::SUCCESS;
    }
    if (tables->size() > 1) {
      return RC::SCHEMA_FIELD_NAME_ILLEGAL;
    }
    const FieldMeta * field =tables->at(0)->table_meta().field(attr.attribute_name);
    if (nullptr == field) {
      return RC::SCHEMA_FIELD_NOT_EXIST;
    }
    type = field->type();
  }
  // 有表名，表名必须出现在from后面的table列表中
  else {
    int exist = false;
    for (int j = 0; j < tables->size(); j++) {
      if (0 == strcmp(tables->at(j)->name(), attr.relation_name)) {
        if (0 == strcmp(attr.attribute_name, "*")) {
          // 
          return RC::SCHEMA_FIELD_REDUNDAN;
        }
        const FieldMeta * field = tables->at(j)->table_meta().field(attr.attribute_name);
        if (nullptr == field) {
          // 属性不存在
          return RC::SCHEMA_FIELD_NOT_EXIST;
        }
        type = field->type();
        exist = true;
        break;
      }
    }
    if (!exist) {
      // 表不存在
      return RC::SCHEMA_TABLE_NOT_EXIST;
    }
  }
  return RC::SUCCESS;
}


//select 中表达式的元数据校验
RC check_meta_select_expr(std::vector<Table *> *tables, CalExp *exp){
  RC rc = RC::SUCCESS;
  if(!exp)
    return rc;
  if(exp->right_exp){
    rc = check_meta_select_expr(tables, exp->left_exp);
    if(RC::SUCCESS == rc){
      rc = check_meta_select_expr(tables, exp->right_exp);
    }
  }else if(!exp->left_exp && !exp->right_exp){
    if(exp->is_attr){
      AttrType type;
      rc = check_attr_in_table(tables, exp->attr, type);
      if(RC::SUCCESS == rc){
        if(type != INTS && type != FLOATS){
          rc = RC::SCHEMA_FIELD_TYPE_MISMATCH;
        }
      }
    }
  }else{
    rc = RC::SCHEMA_FIELD_TYPE_MISMATCH;
  }
  return rc;
}

// select命令的元数据校验
RC check_meta_select(const char *db, Selects &selects, std::vector<Table *> &parent_tables, bool &is_complex) {
  std::vector<Table *> tables;

  // table校验，检查from后面的table是否在db中
  for (int i = selects.relation_num - 1; i >= 0; i--) {
    const char *table_name = selects.relations[i];
    Table * table = DefaultHandler::get_default().find_table(db, table_name);
    if (table == nullptr) {
      return RC::SCHEMA_TABLE_NOT_EXIST;
    }
    tables.push_back(table);
    parent_tables.push_back(table);
  }

  // attr校验，检查select后面的attr是否在from后面的table中
  for (int i = 0; i < selects.attr_num; i++) {
    const RelAttr &attr = selects.attributes[i];
    AttrType type;
    RC rc = check_attr_in_table(&tables, attr, type);
    if (rc != RC::SCHEMA_FIELD_REDUNDAN && rc != RC::SUCCESS) {
      return rc;
    }
  }

  for (int i = 0; i < selects.expr_num; i++) {
    if(selects.exprs[i]->is_attr){
      AttrType type;
      RC rc = check_attr_in_table(&tables, selects.exprs[i]->attr, type);
      if (rc != RC::SCHEMA_FIELD_REDUNDAN && rc != RC::SUCCESS) {
        return rc;
      }
    }else{
      RC rc = check_meta_select_expr(&tables, selects.exprs[i]);
      if(RC::SUCCESS != rc){
        return rc;
      }
    }
  }

  // condition校验，检查where后面的condition中的attr是否在from后面的table中
  int sub_num = 0;
  for (int i = 0; i < selects.condition_num; i++) {
    Condition &condition = selects.conditions[i];
    AttrType left_type = condition.left_value.type;
    AttrType right_type = condition.right_value.type;
    if (condition.left_exp){
      RC rc = check_meta_select_expr(&tables,condition.left_exp);
      if (rc != RC::SUCCESS) {
        return rc;
      }
      left_type = FLOATS;
    }
    else if (condition.left_is_attr) {
      RC rc = check_attr_in_table(&tables, condition.left_attr, left_type);
      if (rc != RC::SUCCESS) {
        rc = check_attr_in_table(&parent_tables, condition.left_attr, left_type);
        if (rc != RC::SUCCESS) {
          return rc;
        }
        is_complex = true;
      }
    }

    // 如果是子查询条件, 需要检查字段是否唯一且类型是否一致
    if (condition.sub_selects != nullptr) {
      condition.is_complex = false;
      if (condition.sub_selects_left != nullptr) {
        continue;
      }
      RC rc = check_meta_select(db, *condition.sub_selects, parent_tables, is_complex);
      if (rc != RC::SUCCESS) {
        return rc;
      }
      condition.is_complex = is_complex;
      if (is_complex) {
        sub_num ++;
        continue;
      }
      sub_num ++;
      continue;
    }

    if (condition.right_exp) {
      RC rc = check_meta_select_expr(&tables,condition.right_exp);
      if (rc != RC::SUCCESS) {
        return rc;
      }
      right_type = FLOATS;
    }else if (condition.right_is_attr) {
      RC rc = check_attr_in_table(&tables, condition.right_attr, right_type);
      if (rc != RC::SUCCESS) {
        rc = check_attr_in_table(&parent_tables, condition.right_attr, right_type);
        if (rc != RC::SUCCESS) {
          return rc;
        }
        is_complex = true;
      }
    }

    if (right_type != NULLS && left_type != NULLS && left_type != right_type && ((left_type != INTS && left_type != FLOATS) || (right_type != INTS && right_type != FLOATS))) {
      // 条件表达式两边类型不匹配，并且其中一个不是int或float类型，不能转换类型
      return RC::SCHEMA_FIELD_TYPE_MISMATCH;
    }
  }

  return RC::SUCCESS;
}

RC check_meta_sub_select(const char *db, const Selects &selects, std::vector<TupleSet> &sub_tuple_sets) {
  std::vector<Table *> tables;

  // table校验，检查from后面的table是否在db中
  for (int i = selects.relation_num - 1; i >= 0; i--) {
    const char *table_name = selects.relations[i];
    Table * table = DefaultHandler::get_default().find_table(db, table_name);
    if (table == nullptr) {
      return RC::SCHEMA_TABLE_NOT_EXIST;
    }
    tables.push_back(table);
  }

  int sub_num = 0;
  for (int i = 0; i < selects.condition_num; i++) {
    const Condition &condition = selects.conditions[i];
    if (condition.sub_selects_left != nullptr || condition.is_complex) {
      continue;
    }

    AttrType left_type = condition.left_value.type;
    AttrType right_type = condition.right_value.type;
    if (condition.left_exp){
      RC rc = check_meta_select_expr(&tables,condition.left_exp);
      if (rc != RC::SUCCESS) {
        return rc;
      }
      left_type = FLOATS;
    }
    else if (condition.left_is_attr) {
      RC rc = check_attr_in_table(&tables, condition.left_attr, left_type);
      if (rc != RC::SUCCESS) {
        return rc;
      }
    }

    // 如果是子查询条件, 需要检查字段是否唯一且类型是否一致
    if (condition.sub_selects != nullptr) {
      if (sub_num + 1 > sub_tuple_sets.size()) {
        return RC::SCHEMA_FIELD_NOT_EXIST;
      }
      TupleSet &tuple_set = sub_tuple_sets[sub_num];
      if (tuple_set.get_schema().fields().size() != 1) {
        return RC::SCHEMA_FIELD_TYPE_MISMATCH;
      }
      if (condition.comp >= EQUAL_TO && condition.comp <= GREAT_THAN && condition.sub_selects->aggr_num != 1) {
        return RC::SCHEMA_FIELD_TYPE_MISMATCH;
      }
      const TupleField &field = tuple_set.get_schema().fields()[0];
      right_type = field.type();
      if (left_type != right_type && ((left_type != INTS && left_type != FLOATS) || (right_type != INTS && right_type != FLOATS))) {
        return RC::SCHEMA_FIELD_TYPE_MISMATCH;
      }
      sub_num ++;
    }
  }

  return RC::SUCCESS;
}

RC double_sub_selects_compare(const char *db, SessionEvent *session_event, const Condition &condition) {
  if (condition.sub_selects_left->aggr_num != 1 || condition.sub_selects_left->attr_num != 0
      || condition.sub_selects->aggr_num != 1 || condition.sub_selects->attr_num != 0) {
    return RC::SCHEMA_FIELD_NOT_SUPPORT;
  }
  TupleSet tuple_set_left, tuple_set_right;
  RC rc = do_sub_select(db, *condition.sub_selects_left, session_event, tuple_set_left, false);
  if (rc != RC::SUCCESS) {
    return rc;
  }
  rc = do_sub_select(db, *condition.sub_selects, session_event, tuple_set_right, false);
  if (rc != RC::SUCCESS) {
    return rc;
  }
  if (tuple_set_left.size() == 0 || tuple_set_right.size() == 0) {
    return RC::SCHEMA_CONDITION_FILTER_ALL;
  }
  const TupleValue &value_left = tuple_set_left.tuples()[0].get(0);
  const TupleValue &value_right = tuple_set_right.tuples()[0].get(0);
  int cmp_result = value_left.compare(value_right);
  bool result = false;
  switch (condition.comp)
  {
    case EQUAL_TO:
      result = 0 == cmp_result;
      break;
    case LESS_EQUAL:
      result = cmp_result <= 0;
      break;
    case NOT_EQUAL:
      result = cmp_result != 0;
      break;
    case LESS_THAN:
      result = cmp_result < 0;
      break;
    case GREAT_EQUAL:
      result = cmp_result >= 0;
      break;
    case GREAT_THAN:
      result = cmp_result > 0;
      break;
    default:
      result = false;
  }
  if (result == false) {
    return RC::SCHEMA_CONDITION_FILTER_ALL;
  }
  return RC::SUCCESS;
}

bool compare_attr_tuples(Condition &condition, const TupleSchema &schema, const Tuple &tuple, TupleSet &tuple_set) {
  if (tuple_set.size() == 0 || schema.fields().size() == 0 || tuple.size() == 0) {
    return false;
  }

  TupleValue *left_tuple_value = nullptr;
  std::vector<const TupleField>::const_iterator field_iter;
  std::vector<const std::shared_ptr<TupleValue> >::const_iterator value_iter;
  
  field_iter = schema.fields().begin();
  value_iter = tuple.values().begin();
  for (; field_iter != schema.fields().end(); ++field_iter, ++value_iter) {
    if ((condition.left_attr.relation_name == nullptr || 0 == strcmp(field_iter->table_name(), condition.left_attr.relation_name))
        && 0 == strcmp(field_iter->field_name(), condition.left_attr.attribute_name)) {
      left_tuple_value = value_iter->get();
      break;
    }
  }
  if (left_tuple_value == nullptr) {
    return false;
  }

  const TupleValue &right_tuple_value = tuple_set.get(0).get(0);
  if (right_tuple_value.get_type() == FLOATS && left_tuple_value->get_type() == INTS) {
    IntValue *tuple_tmp_value = dynamic_cast<IntValue *>(left_tuple_value);
    int value = tuple_tmp_value->get_value();
    delete tuple_tmp_value;
    left_tuple_value = new FloatValue((float)value, FLOATS);
  }
  if (condition.comp >= EQUAL_TO && condition.comp <= GREAT_THAN) {
    int cmp_result = 0;
    cmp_result = left_tuple_value->compare(right_tuple_value);
    switch (condition.comp)
    {
    case EQUAL_TO:
      return 0 == cmp_result;
    case LESS_EQUAL:
      return cmp_result <= 0;
    case NOT_EQUAL:
      return cmp_result != 0;
    case LESS_THAN:
      return cmp_result < 0;
    case GREAT_EQUAL:
      return cmp_result >= 0;
    case GREAT_THAN:
      return cmp_result > 0;
    default:
      return false;
    }
  }
  // 判断是否在子查询集合中
  bool in = false;
  for (int i = 0; i < tuple_set.size(); i++) {
    const TupleValue &right_tuple_value = tuple_set.get(i).get(0);
    if (left_tuple_value->get_type() == right_tuple_value.get_type() && left_tuple_value->compare(right_tuple_value) == 0) {
      in = true;
      break;
    }
  }
  if ((condition.comp == WHERE_IN && in) || (condition.comp == NOT_IN && !in)) {
    return true;
  }
  return false;
}

RC transfer_tuple_to_value(const TupleSchema &schema, const Tuple &tuple, Value &value, RelAttr &attr) {
  TupleValue *tuple_value = nullptr;
  std::vector<const TupleField>::const_iterator field_iter;
  std::vector<const std::shared_ptr<TupleValue> >::const_iterator value_iter;
  
  field_iter = schema.fields().begin();
  value_iter = tuple.values().begin();
  for (; field_iter != schema.fields().end(); ++field_iter, ++value_iter) {
    if ((attr.relation_name == nullptr || 0 == strcmp(field_iter->table_name(), attr.relation_name))
        && 0 == strcmp(field_iter->field_name(), attr.attribute_name)) {
      tuple_value = value_iter->get();
      break;
    }
  }
  if (tuple_value == nullptr) {
    return RC::SCHEMA_FIELD_NOT_EXIST;
  }

  if (tuple_value->get_type() == INTS) {
    IntValue *tuple_tmp_value = dynamic_cast<IntValue *>(tuple_value);
    int v = tuple_tmp_value->get_value();
    value.type = INTS;
    value.data = malloc(sizeof(v));
    memcpy(value.data, &v, sizeof(v));
  }
  else if (tuple_value->get_type() == FLOATS) {
    FloatValue *tuple_tmp_value = dynamic_cast<FloatValue *>(tuple_value);
    float v = tuple_tmp_value->get_value();
    value.type = FLOATS;
    value.data = malloc(sizeof(v));
    memcpy(value.data, &v, sizeof(v));
  }
  else if (tuple_value->get_type() == DATES) {
    DateValue *tuple_tmp_value = dynamic_cast<DateValue *>(tuple_value);
    unsigned int v = tuple_tmp_value->get_value();
    value.type = DATES;
    value.data = malloc(sizeof(v));
    memcpy(value.data, &v, sizeof(v));
  }
  else if (tuple_value->get_type() == CHARS) {
    StringValue *tuple_tmp_value = dynamic_cast<StringValue *>(tuple_value);
    std::string v = tuple_tmp_value->get_value();
    value.type = CHARS;
    value.data = malloc(v.size());
    memcpy(value.data, v.c_str(), v.size());
  }
  else {
    return RC::SCHEMA_FIELD_NOT_SUPPORT;
  }

  return RC::SUCCESS;
}

RC do_complex_sub_select(const char *db, Selects &selects, SessionEvent *session_event, const TupleSchema &schema, const Tuple &tuple, TupleSet &tuple_set_out) {
  const char *table_name = selects.relations[0];
  Table * table = DefaultHandler::get_default().find_table(db, table_name);
  if (table == nullptr) {
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }
  
  // 如果子查询还有父查询的属性，先做一次转换
  for (int i = 0; i < selects.condition_num; i++) {
    Condition &condition = selects.conditions[i];
    if (condition.sub_selects) {
      continue;
    }
    if (0 != strcmp(table->name(), condition.left_attr.relation_name)) {
      transfer_tuple_to_value(schema, tuple, condition.left_value, condition.left_attr);
      condition.left_is_attr = 0;
    }
    else if(0 != strcmp(table->name(), condition.right_attr.relation_name)) {
      transfer_tuple_to_value(schema, tuple, condition.right_value, condition.right_attr);
      condition.right_is_attr = 0;
    }
  }

  // 检查是否含有复杂查询
  bool is_complex = false;
  for (int j = 0; j < selects.condition_num; j++) {
    Condition &condition = selects.conditions[j];
    if (condition.sub_selects != nullptr && condition.is_complex) {
      is_complex = true;
      break;
    }
  }
  if (!is_complex) {
    // 普通查询，直接执行即可
    return do_sub_select(db, selects, session_event, tuple_set_out, is_complex);
  }

  TupleSet tuple_set_tmp;
  RC rc = do_sub_select(db, selects, session_event, tuple_set_tmp, is_complex);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  TupleSchema new_schema;
  new_schema.append(schema);
  new_schema.append(tuple_set_tmp.get_schema());
  tuple_set_out.set_schema(tuple_set_tmp.get_schema());
  TupleSet tuple_set_sub_out;

  for (int i = 0; i < tuple_set_tmp.size(); i++) {
    bool flag = true;
    const Tuple &my_tuple = tuple_set_tmp.get(i);
    Tuple new_tuple = tuple;
    for (int j = 0; j < my_tuple.size(); j++) {
      new_tuple.add(my_tuple.get_pointer(j));
    }
    for (int j = 0; j < selects.condition_num; j++) {
      Condition &condition = selects.conditions[j];
      if (condition.is_complex) {
        rc = do_complex_sub_select(db, *condition.sub_selects, session_event, new_schema, new_tuple, tuple_set_sub_out);
        if (rc != RC::SUCCESS) {
          return rc;
        }
        if (!compare_attr_tuples(condition, tuple_set_tmp.get_schema(), my_tuple, tuple_set_sub_out)) {
          flag = false;
          break;
        }
      }
    }
    if (flag) {
      tuple_set_out.add(Tuple(my_tuple));
    }
  }

  return rc;
}

RC ExecuteStage::do_select(const char *db, Query *sql, SessionEvent *session_event) {
  RC rc = RC::SUCCESS;
  Session *session = session_event->get_client()->session;
  Trx *trx = session->current_trx();
  Selects &selects = sql->sstr.selection;

  // 元数据校验
  std::vector<Table *> tables;
  bool is_complex = false;
  rc = check_meta_select(db, selects, tables, is_complex);
  if (rc != RC::SUCCESS) {
    end_trx_if_need(session, trx, false);
    return rc;
  }

  // 如果是简单子查询, 先执行简单子查询
  bool filter_all = false;
  std::vector<TupleSet> sub_tuple_sets;
  std::vector<Condition> conditions;
  for (int i = 0; i < selects.condition_num; i++) {
    const Condition &condition = selects.conditions[i];
    if (condition.sub_selects != nullptr) {
      if (condition.is_complex) {
        continue;
      }
      if (condition.sub_selects_left != nullptr) {
        rc = double_sub_selects_compare(db, session_event, condition);
        if (rc == RC::SCHEMA_CONDITION_FILTER_ALL) {
          filter_all = true;
        }
        else if (rc != RC::SUCCESS) {
          return rc;
        }
        continue;
      }
      TupleSet tuple_set;
      rc = do_sub_select(db, *condition.sub_selects, session_event, tuple_set, false);
      if (rc != RC::SUCCESS) {
        end_trx_if_need(session, trx, false);
        return rc;
      }
      sub_tuple_sets.push_back(std::move(tuple_set));
    }
  }

  rc = check_meta_sub_select(db, selects, sub_tuple_sets);
  if (rc != RC::SUCCESS) {
    end_trx_if_need(session, trx, false);
    return rc;
  }

  // 把和某张表关联的condition都拿出来，生成该表最底层的select 执行节点, 一个执行节点包括需要返回的列、条件
  // 数组是用来存放不同表的select 执行节点
  std::vector<SelectExeNode *> select_nodes;
  for (int i = selects.relation_num - 1; i >= 0; i--) {
    const char *table_name = selects.relations[i];
    SelectExeNode *select_node = new SelectExeNode;
    int table_num = is_complex ? 2:selects.relation_num;
    rc = create_selection_executor(trx, selects, db, table_name, table_num, sub_tuple_sets, *select_node);
    if (rc != RC::SUCCESS) {
      for (SelectExeNode *& tmp_node: select_nodes) {
        delete tmp_node;
      }
      end_trx_if_need(session, trx, false);
      return rc;
    }
    select_nodes.push_back(select_node);
  }

  if (select_nodes.empty()) {
    LOG_ERROR("No table given");
    end_trx_if_need(session, trx, false);
    return RC::SQL_SYNTAX;
  }

  std::vector<TupleSet> tuple_sets;
  for (SelectExeNode *&node: select_nodes) {
    TupleSet tuple_set;
    rc = node->execute(tuple_set);
    if (rc != RC::SUCCESS) {
      for (SelectExeNode *& tmp_node: select_nodes) {
        delete tmp_node;
      }
      end_trx_if_need(session, trx, false);
      return rc;
    } else {
      // WARN 这里调用了TupleSet拷贝构造函数，导致tuple_set内的TupleSchema进行了fields的转移
      tuple_sets.push_back(std::move(tuple_set));
    }
  }
  if (filter_all) {
    tuple_sets.front().clear_tuples();
  }

  std::stringstream ss;
  // 复杂子查询
  if (is_complex) {
    TupleSet &tuple_set_tmp = tuple_sets[0];
    TupleSet tuple_set_in(tuple_set_tmp.get_schema());
    TupleSet tuple_set(tuple_set_tmp.get_schema());
    TupleSet tuple_set_out;
    for (int i = 0; i < tuple_set_tmp.size(); i++) {
      bool flag = true;
      const Tuple &tuple = tuple_set_tmp.get(i);
      for (int j = 0; j < selects.condition_num; j++) {
        Condition &condition = selects.conditions[j];
        if (condition.is_complex) {
          rc = do_complex_sub_select(db, *condition.sub_selects, session_event, tuple_set_in.get_schema(), tuple, tuple_set_out);
          if (rc != RC::SUCCESS) {
            return rc;
          }
          if (!compare_attr_tuples(condition, tuple_set_in.get_schema(), tuple, tuple_set_out)) {
            flag = false;
            break;
          }
        }
      }
      if (flag) {
        tuple_set.add(Tuple(tuple));
      }
    }

    tuple_set.print(ss);
  }
  else {
    TupleSet join_tuple_set;
    std::vector<JoinConditionFilter *> join_condition_filters;
    rc = create_join_condition_filter(db, selects, join_condition_filters);
    if (tuple_sets.size() > 1) {
      // 本次查询了多张表，需要做join操作
      if (rc != RC::SUCCESS) {
        return rc;
      }
      std::vector<TupleSet> join_tuple_sets;
      join_tuple_sets.push_back(std::move(tuple_sets[0]));
      join_tuple_sets.push_back(std::move(tuple_sets[1]));
      join_tables(join_tuple_sets, join_condition_filters, join_tuple_set);
      for(int i = 2; i < tuple_sets.size(); ++i){
        join_tuple_sets.clear();
        join_tuple_sets.push_back(std::move(join_tuple_set));
        join_tuple_sets.push_back(std::move(tuple_sets[i]));
        join_tables(join_tuple_sets, join_condition_filters, join_tuple_set);
      }
      if(selects.expr_num)
        join_tuple_set.print(&selects.exprs[0],selects.expr_num, true, ss);
      else
        join_tuple_set.print(ss);
    }
    else {
      if(!join_condition_filters.empty()){
        //存在表达式
        join_tables(tuple_sets, join_condition_filters, join_tuple_set);
        if(selects.expr_num)
          join_tuple_set.print(&selects.exprs[0],selects.expr_num, false, ss);
        else
          join_tuple_set.print(ss);
      }
      else {
        // 当前只查询一张表，直接返回结果即可
        if(selects.expr_num)
          tuple_sets.front().print(&selects.exprs[0],selects.expr_num,false, ss);
        else
          tuple_sets.front().print(ss);
      }
    }
  }

  for (SelectExeNode *& tmp_node: select_nodes) {
    delete tmp_node;
  }
  session_event->set_response(ss.str());
  end_trx_if_need(session, trx, true);
  return rc;
}

RC do_sub_select(const char *db, const Selects &selects, SessionEvent *session_event, TupleSet &tuple_set, bool is_complex) {
  RC rc = RC::SUCCESS;
  Session *session = session_event->get_client()->session;
  Trx *trx = session->current_trx();

  // 目前子查询只支持单字段单表
  if (selects.relation_num != 1 || selects.attr_num + selects.aggr_num != 1) {
    LOG_ERROR("select fail: subquery has more than one attribute/relation or the attribute is \"*\"");
    return RC::SCHEMA_FIELD_NOT_SUPPORT;
  }

  // 如果有子查询, 先执行子查询
  bool filter_all = false;
  std::vector<TupleSet> sub_tuple_sets;
  std::vector<Condition> conditions;
  for (int i = 0; i < selects.condition_num; i++) {
    const Condition &condition = selects.conditions[i];
    if (condition.sub_selects != nullptr) {
      if (condition.is_complex) {
        continue;
      }
      if (condition.sub_selects_left != nullptr) {
        rc = double_sub_selects_compare(db, session_event, condition);
        if (rc == RC::SCHEMA_CONDITION_FILTER_ALL) {
          filter_all = true;
        }
        else if (rc != RC::SUCCESS) {
          return rc;
        }
        continue;
      }
      TupleSet tuple_set;
      rc = do_sub_select(db, *condition.sub_selects, session_event, tuple_set, is_complex);
      if (rc != RC::SUCCESS) {
        return rc;
      }
      sub_tuple_sets.push_back(std::move(tuple_set));
    }
  }

  // 把和某张表关联的condition都拿出来，生成该表最底层的select 执行节点, 一个执行节点包括需要返回的列、条件
  // 数组是用来存放不同表的select 执行节点
  std::vector<SelectExeNode *> select_nodes;
  for (int i = selects.relation_num - 1; i >= 0; i--) {
    const char *table_name = selects.relations[i];
    SelectExeNode *select_node = new SelectExeNode;
    int table_num = is_complex ? 2:selects.relation_num;
    rc = create_selection_executor(trx, selects, db, table_name, table_num, sub_tuple_sets, *select_node);
    if (rc != RC::SUCCESS) {
      for (SelectExeNode *& tmp_node: select_nodes) {
        delete tmp_node;
      }
      return rc;
    }
    select_nodes.push_back(select_node);
  }

  if (select_nodes.empty()) {
    LOG_ERROR("No table given");
    return RC::SQL_SYNTAX;
  }

  SelectExeNode * node = select_nodes[0];
  rc = node->execute(tuple_set);
  if (rc != RC::SUCCESS) {
    delete node;
    return rc;
  }
  if (filter_all) {
    tuple_set.clear_tuples();
  }

  delete node;
  return rc;
}

bool match_table(const Selects &selects, const char *table_name_in_condition, const char *table_name_to_match) {
  if (table_name_in_condition != nullptr) {
    return 0 == strcmp(table_name_in_condition, table_name_to_match);
  }

  return selects.relation_num == 1;
}

static RC schema_add_field(Table *table, const char *field_name, TupleSchema &schema) {
  const FieldMeta *field_meta = table->table_meta().field(field_name);
  if (nullptr == field_meta) {
    LOG_WARN("No such field. %s.%s", table->name(), field_name);
    return RC::SCHEMA_FIELD_MISSING;
  }

  schema.add_if_not_exists(field_meta->type(), NOTHING_A, table->name(), field_meta->name());
  return RC::SUCCESS;
}

static RC schema_add_field_agg(Table *table, AggType atype, const char *field_name, TupleSchema &schema) {
  // */number 是count运算下的特殊属性名
  if(strcmp(field_name, "*") == 0 || (field_name && field_name[0] >= '0' && field_name[0] <= '9')) {
    if(atype != COUNT_A) return RC::SCHEMA_FIELD_MISSING;
    // 使用任意type不影响count(*)/count(number)，因为count(*)/count(number)这里不考虑具体列
    schema.add_if_not_exists(INTS, atype, table->name(), field_name);
    return RC::SUCCESS;
  } else {
    const FieldMeta *field_meta = table->table_meta().field(field_name);
    if (nullptr == field_meta) {
      LOG_WARN("No such field. %s.%s", table->name(), field_name);
      return RC::SCHEMA_FIELD_MISSING;
    }
    
    if(atype == AVG_A && (field_meta->type() != INTS && field_meta->type() != FLOATS && field_meta->type() != DATES)) {
      LOG_WARN("Field can not calculate AVG. %s.%s", table->name(), field_name);
      return RC::SCHEMA_FIELD_TYPE_MISMATCH;
    }

    schema.add_if_not_exists(field_meta->type(), atype, table->name(), field_meta->name());
    return RC::SUCCESS;
  }
}

// 把和某张表关联的condition都拿出来，生成该表最底层的select 执行节点, 一个执行节点包括需要返回的列、条件
RC create_selection_executor(Trx *trx, const Selects &selects, const char *db, const char *table_name, int table_num, std::vector<TupleSet> &sub_tuple_sets, SelectExeNode &select_node) {
  // 列出跟这张表关联的Attr
  TupleSchema schema;
  Table * table = DefaultHandler::get_default().find_table(db, table_name);
  if (nullptr == table) {
    LOG_WARN("No such table [%s] in db [%s]", table_name, db);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }
  
  // TODO 目前不支持同时有属性和聚合运算的查询
  if(selects.attr_num + selects.expr_num > 0 && selects.aggr_num > 0) {
    LOG_ERROR("Not support attribute and aggregation both exist now");
    return RC::SCHEMA_FIELD_NOT_SUPPORT;
  }

  if(table_num > 1){
    TupleSchema::from_table(table, schema);
  }else{
    //处理select后expr属性
    int index = 0;
    RelAttr attrs[MAX_NUM];
    ExprHandler h;
    for(int i = selects.expr_num - 1; i >= 0; i--){
      index = 0;
      RC rc = h.AppendAttrs(selects.exprs[i], &attrs[0], MAX_NUM, index);
      if(RC::SUCCESS != rc)
        return rc;
      for(int i = index - 1; i >= 0; i--){
        if(nullptr != attrs[i].relation_name){
          if(0 != strcmp(attrs[i].relation_name,table->name())){
            continue;
          }
        }
        RC rc = schema_add_field(table, attrs[i].attribute_name, schema);
        if(RC::SUCCESS != rc)
          return rc;
      }
    }

    // 处理查找属性
    for (int i = selects.attr_num - 1; i >= 0; i--) {
      const RelAttr &attr = selects.attributes[i];
      if (nullptr == attr.relation_name || 0 == strcmp(table_name, attr.relation_name)) {
        if (0 == strcmp("*", attr.attribute_name)) {
          // 列出这张表所有字段
          TupleSchema::from_table(table, schema);
          break; // TODO 没有校验，给出* 之后，再写字段的错误 // *,ID paser直接失败
        } else {
          // 列出这张表相关字段
          RC rc = schema_add_field(table, attr.attribute_name, schema);
          if (rc != RC::SUCCESS) {
            return rc;
          }
        }
      }
    }
  }

  //处理聚合运算符
  for (int i = selects.aggr_num - 1; i >= 0; i--) {
    const AggOp &aggr = selects.aggregations[i];
    if (nullptr == aggr.attribute.relation_name || 0 == strcmp(table_name, aggr.attribute.relation_name)) {
      // */number在这里同样当成属性处理，在底层逻辑里根据聚合运算另作处理
      // 列出这张表相关字段
      RC rc = schema_add_field_agg(table, aggr.type, aggr.attribute.attribute_name, schema);
      if (rc != RC::SUCCESS) {
        return rc;
      }
    } else {
      LOG_WARN("Table [%s] and table [%s] is not equal", table_name, aggr.attribute.relation_name);
      return RC::SCHEMA_TABLE_NOT_EQUAL;
    }
  }

  // 找出仅与此表相关的过滤条件, 或者都是值的过滤条件
  std::vector<DefaultConditionFilter *> condition_filters;
  int sub_idx = 0;
  for (int i = 0; i < selects.condition_num; i++) {
    RC rc = RC::SUCCESS;
    const Condition &condition = selects.conditions[i];
    DefaultConditionFilter *condition_filter = new DefaultConditionFilter(*table);
    // 如果是子查询条件
    if (condition.sub_selects != nullptr) {
      if (condition.sub_selects_left != nullptr) {
        continue;
      }
      if (condition.is_complex) {
        continue;
      }
      rc = condition_filter->init(*table, condition, &sub_tuple_sets[sub_idx]);
      sub_idx ++;
    }else if(condition.left_exp || condition.right_exp){
      continue;
    }else if ((condition.left_is_attr == 0 && condition.right_is_attr == 0) || // 两边都是值
        (condition.left_is_attr == 1 && condition.right_is_attr == 0 && match_table(selects, condition.left_attr.relation_name, table_name)) ||  // 左边是属性右边是值
        (condition.left_is_attr == 0 && condition.right_is_attr == 1 && match_table(selects, condition.right_attr.relation_name, table_name)) ||  // 左边是值，右边是属性名
        (condition.left_is_attr == 1 && condition.right_is_attr == 1 &&
            match_table(selects, condition.left_attr.relation_name, table_name) && match_table(selects, condition.right_attr.relation_name, table_name)) // 左右都是属性名，并且表名都符合
        ) {
      rc = condition_filter->init(*table, condition, NULL);
    }
    else {
      continue;
    }

    if (rc == RC::SCHEMA_CONDITION_INVALID) {
      delete condition_filter;
      continue;
    } else if (rc != RC::SUCCESS) {
      delete condition_filter; // free space
      return rc;
    }
    condition_filters.push_back(condition_filter);
  }

  return select_node.init(trx, table, std::move(schema), std::move(condition_filters));
}
