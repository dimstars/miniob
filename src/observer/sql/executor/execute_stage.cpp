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

RC create_selection_executor(Trx *trx, const Selects &selects, const char *db, const char *table_name, SelectExeNode &select_node);

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
        session_event->set_response(response);
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
  // 若任意TupleSet为空，那么笛卡尔积为空
  for (TupleSet &tuple_set: tuple_sets) {
    if (tuple_set.size() == 0) {
      return RC::SUCCESS;
    }
  }

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
      RC rc = join_condition_filter->init(*left_table, *right_table, condition);
      if (rc != RC::SUCCESS) {
        delete join_condition_filter;
        return rc;
      }
      join_condition_filters.push_back(join_condition_filter);
    }
  }
  return RC::SUCCESS;
}

RC check_attr_in_table(std::vector<Table *> tables, const RelAttr &attr, AttrType &type) {
  // 无表名，要么是“*”，要么是单表查询
  if (attr.relation_name == nullptr) {
    if (0 == strcmp(attr.attribute_name, "*")) {
      return RC::SUCCESS;
    }
    if (tables.size() > 1) {
      return RC::SCHEMA_FIELD_NAME_ILLEGAL;
    }
    const FieldMeta * field =tables[0]->table_meta().field(attr.attribute_name);
    if (nullptr == field) {
      return RC::SCHEMA_FIELD_NOT_EXIST;
    }
    type = field->type();
  }
  // 有表名，表名必须出现在from后面的table列表中
  else {
    int exist = false;
    for (int j = 0; j < tables.size(); j++) {
      if (0 == strcmp(tables[j]->name(), attr.relation_name)) {
        const FieldMeta * field = tables[j]->table_meta().field(attr.attribute_name);
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

// select命令的元数据校验
RC check_meta_select(const char *db, const Selects &selects) {
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

  // attr校验，检查select后面的attr是否在from后面的table中
  for (int i = 0; i < selects.attr_num; i++) {
    const RelAttr &attr = selects.attributes[i];
    AttrType type;
    RC rc = check_attr_in_table(tables, attr, type);
    if (rc != RC::SUCCESS) {
      return rc;
    }
  }

  // condition校验，检查where后面的condition中的attr是否在from后面的table中
  for (int i = 0; i < selects.condition_num; i++) {
    const Condition &condition = selects.conditions[i];
    AttrType left_type = condition.left_value.type;
    AttrType right_type = condition.right_value.type;
    if (condition.left_is_attr) {
      RC rc = check_attr_in_table(tables, condition.left_attr, left_type);
      if (rc != RC::SUCCESS) {
        return rc;
      }
    }
    if (condition.right_is_attr) {
      RC rc = check_attr_in_table(tables, condition.right_attr, right_type);
      if (rc != RC::SUCCESS) {
        return rc;
      }
    }
    if (left_type != right_type && ((left_type != INTS && left_type != FLOATS) || (right_type != INTS && right_type != FLOATS))) {
      // 条件表达式两边类型不匹配，并且其中一个不是int或float类型，不能转换类型
      return RC::SCHEMA_FIELD_TYPE_MISMATCH;
    }
  }

  return RC::SUCCESS;
}

// TODO 这里没有对输入的某些信息做合法性校验，比如查询的列名、where条件中的列名等，没有做必要的合法性校验
// 需要补充上这一部分. 校验部分也可以放在resolve，不过跟execution放一起也没有关系
RC ExecuteStage::do_select(const char *db, Query *sql, SessionEvent *session_event) {
  RC rc = RC::SUCCESS;
  Session *session = session_event->get_client()->session;
  Trx *trx = session->current_trx();
  const Selects &selects = sql->sstr.selection;

  // 元数据校验
  rc = check_meta_select(db, selects);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  // 把和某张表关联的condition都拿出来，生成该表最底层的select 执行节点, 一个执行节点包括需要返回的列、条件
  // 数组是用来存放不同表的select 执行节点
  std::vector<SelectExeNode *> select_nodes;
  for (int i = selects.relation_num - 1; i >= 0; i--) {
    const char *table_name = selects.relations[i];
    SelectExeNode *select_node = new SelectExeNode;
    rc = create_selection_executor(trx, selects, db, table_name, *select_node);
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

  std::stringstream ss;
  if (tuple_sets.size() > 1) {
    // 本次查询了多张表，需要做join操作
    TupleSet join_tuple_set;
    std::vector<JoinConditionFilter *> join_condition_filters;
    rc = create_join_condition_filter(db, selects, join_condition_filters);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    join_tables(tuple_sets, join_condition_filters, join_tuple_set);
    join_tuple_set.print(ss);
  } else {
    // 当前只查询一张表，直接返回结果即可
    tuple_sets.front().print(ss);
  }

  for (SelectExeNode *& tmp_node: select_nodes) {
    delete tmp_node;
  }
  session_event->set_response(ss.str());
  end_trx_if_need(session, trx, true);
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
    assert(atype == COUNT_A);
    // 使用任意type不影响count(*)/count(number)，因为count(*)/count(number)这里不考虑具体列
    schema.add_if_not_exists(INTS, atype, table->name(), field_name);
    return RC::SUCCESS;
  } else {
    const FieldMeta *field_meta = table->table_meta().field(field_name);
    if (nullptr == field_meta) {
      LOG_WARN("No such field. %s.%s", table->name(), field_name);
      return RC::SCHEMA_FIELD_MISSING;
    }
    
    if(atype == AVG_A && (field_meta->type() < INTS || field_meta->type() > FLOATS)) {
      LOG_WARN("Field can not calculate AVG. %s.%s", table->name(), field_name);
      return RC::SCHEMA_FIELD_TYPE_MISMATCH;
    }

    schema.add_if_not_exists(field_meta->type(), atype, table->name(), field_meta->name());
    return RC::SUCCESS;
  }
}

// 把和某张表关联的condition都拿出来，生成该表最底层的select 执行节点, 一个执行节点包括需要返回的列、条件
RC create_selection_executor(Trx *trx, const Selects &selects, const char *db, const char *table_name, SelectExeNode &select_node) {
  // 列出跟这张表关联的Attr
  TupleSchema schema;
  Table * table = DefaultHandler::get_default().find_table(db, table_name);
  if (nullptr == table) {
    LOG_WARN("No such table [%s] in db [%s]", table_name, db);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }
  
  // TODO 目前不支持同时有属性和聚合运算的查询
  if(selects.attr_num > 0 && selects.aggr_num > 0) {
    LOG_ERROR("Not support attribute and aggregation both exist now");
    return RC::SCHEMA_FIELD_NOT_SUPPORT;
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
  for (int i = 0; i < selects.condition_num; i++) {
    const Condition &condition = selects.conditions[i];
    // TODO 这里需要支持多表操作, 如 t1.id == t2.id
    if ((condition.left_is_attr == 0 && condition.right_is_attr == 0) || // 两边都是值
        (condition.left_is_attr == 1 && condition.right_is_attr == 0 && match_table(selects, condition.left_attr.relation_name, table_name)) ||  // 左边是属性右边是值
        (condition.left_is_attr == 0 && condition.right_is_attr == 1 && match_table(selects, condition.right_attr.relation_name, table_name)) ||  // 左边是值，右边是属性名
        (condition.left_is_attr == 1 && condition.right_is_attr == 1 &&
            match_table(selects, condition.left_attr.relation_name, table_name) && match_table(selects, condition.right_attr.relation_name, table_name)) // 左右都是属性名，并且表名都符合
        ) {
      DefaultConditionFilter *condition_filter = new DefaultConditionFilter();
      RC rc = condition_filter->init(*table, condition);
      if (rc != RC::SUCCESS) {
        delete condition_filter; // free space
        return rc;
      }
      condition_filters.push_back(condition_filter);
    }
  }

  return select_node.init(trx, table, std::move(schema), std::move(condition_filters));
}
