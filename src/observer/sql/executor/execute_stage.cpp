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
RC create_join_executor(const Selects &selects, const char *db, const std::vector<TupleSet> &tuple_set_list, JoinExeNode &node);

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
      do_select(current_db, sql, exe_event->sql_event()->session_event());
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
    case SCF_DROP_INDEX: {
      StorageEvent *storage_event = new (std::nothrow) StorageEvent(exe_event);
      if (storage_event == nullptr) {
        LOG_ERROR("Failed to new StorageEvent");
        event->done_immediate();
        return;
      }

      CompletionCallback *cb = new (std::nothrow) CompletionCallback(this, nullptr);
      if (cb == nullptr) {
        LOG_ERROR("Failed to new callback for SessionEvent");
        exe_event->done_immediate();
        return;
      }

      exe_event->push_callback(cb);
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

// TODO 这里没有对输入的某些信息做合法性校验，比如查询的列名、where条件中的列名等，没有做必要的合法性校验
// 需要补充上这一部分
RC ExecuteStage::do_select(const char *db, Query *sql, SessionEvent *session_event) {

  RC rc = RC::SUCCESS;
  Session *session = session_event->get_client()->session;
  Trx *trx = session->current_trx();
  const Selects &selects = sql->sstr.selection;
  // 把所有的表和只跟这张表关联的condition都拿出来，生成最底层的select 执行节点
  std::vector<SelectExeNode> select_nodes;
  for (int i = 0; i < selects.relation_num; i++) {
    const char *table_name = selects.relations[i];
    SelectExeNode select_node;
    rc = create_selection_executor(trx, selects, db, table_name, select_node);
    if (rc != RC::SUCCESS) {
      end_trx_if_need(session, trx, false);
      return rc;
    }
    select_nodes.emplace_back(std::move(select_node));
  }

  if (select_nodes.empty()) {
    LOG_ERROR("No table given");
    end_trx_if_need(session, trx, false);
    return RC::SQL_SYNTAX;
  }

  std::vector<TupleSet> tuple_sets;
  for (SelectExeNode &node: select_nodes) {
    TupleSet tuple_set;
    rc = node.execute(tuple_set);
    if (rc != RC::SUCCESS) {
      end_trx_if_need(session, trx, false);
      return rc;
    } else {
      tuple_sets.push_back(std::move(tuple_set));
    }
  }

  std::stringstream ss;
  if (tuple_sets.size() > 1) {
    // 本次查询了多张表，需要做join操作
    JoinExeNode join_exe_node;
    rc = create_join_executor(selects, db, tuple_sets, join_exe_node);
    if (rc != RC::SUCCESS) {
      end_trx_if_need(session, trx, false);
      return rc;
    }

    TupleSet tuple_set;
    rc = join_exe_node.execute(tuple_set);
    if (rc != RC::SUCCESS) {
      end_trx_if_need(session, trx, false);
      return rc;
    }
    tuple_set.print(ss);
  } else {
    // 当前只查询一张表，直接返回结果即可
    tuple_sets.front().print(ss);
  }

  session_event->set_response(ss.str());
  end_trx_if_need(session, trx, true);
  return rc;
}

bool match_table(const Selects &selects, const char *table_name_in_condition, const char *table_name_to_match) {
  if (table_name_in_condition != nullptr) {
    return 0 == strcmp(table_name_in_condition, table_name_to_match);
  }

  return selects.condition_num == 1;
}

static RC schema_add_field(Table *table, const char *field_name, TupleSchema &schema) {
  const FieldMeta *field_meta = table->table_meta().field(field_name);
  if (nullptr == field_meta) {
    // TODO 返回客户端
    LOG_WARN("No such field. %s.%s", table->name(), field_name);
    return RC::SCHEMA_FIELD_MISSING;
  }

  schema.add_if_not_exists(field_meta->type(), table->name(), field_meta->name());
  return RC::SUCCESS;
}

// 把所有的表和只跟这张表关联的condition都拿出来，生成最底层的select 执行节点
RC create_selection_executor(Trx *trx, const Selects &selects, const char *db, const char *table_name, SelectExeNode &select_node) {
  // 列出跟这张表关联的Attr
  TupleSchema schema;
  Table * table = DefaultHandler::get_default().find_table(db, table_name);
  if (nullptr == table) {
    LOG_WARN("No such table [%s] in db [%s]", table_name, db);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  for (int i = selects.attr_num - 1; i >= 0; i--) {
    const RelAttr &attr = selects.attributes[i];
    if (nullptr == attr.relation_name || 0 == strcmp(table_name, attr.relation_name)) {
      if (0 == strcmp("*", attr.attribute_name)) {
        // 列出这张表所有字段
        TupleSchema::from_table(table, schema);
        break; // TODO 没有校验，给出* 之后，再写字段的错误
      } else {
        // 列出这张表相关字段
        RC rc = schema_add_field(table, attr.attribute_name, schema);
        if (rc != RC::SUCCESS) {
          return rc;
        }
      }
    }
  }

  // 把所有condition中的属性也加入进来
  for (int i = 0; i < selects.condition_num; i++) {
    const Condition &condition = selects.conditions[i];
    if (condition.left_is_attr == 1 && match_table(selects, condition.left_attr.relation_name, table_name)) {
      RC rc = schema_add_field(table, condition.left_attr.attribute_name, schema);
      if (rc != RC::SUCCESS) {
        return rc;
      }
    }
    if (condition.right_is_attr == 1 && match_table(selects, condition.right_attr.relation_name, table_name)) {
      RC rc = schema_add_field(table, condition.right_attr.attribute_name, schema);
      if (rc != RC::SUCCESS) {
        return rc;
      }
    }
  }

  // 找出仅与此表相关的过滤条件, 或者都是值的过滤条件
  std::vector<DefaultConditionFilter> condition_filters;
  for (int i = 0; i < selects.condition_num; i++) {
    const Condition &condition = selects.conditions[i];
    if ((condition.left_is_attr == 0 && condition.right_is_attr == 0) || // 两边都是值
        (condition.left_is_attr == 1 && condition.right_is_attr == 0 && match_table(selects, condition.left_attr.relation_name, table_name)) ||  // 左边是属性右边是值
        (condition.left_is_attr == 0 && condition.right_is_attr == 1 && match_table(selects, condition.right_attr.relation_name, table_name)) ||  // 左边是值，右边是属性名
        (condition.left_is_attr == 1 && condition.right_is_attr == 1 &&
            match_table(selects, condition.left_attr.relation_name, table_name) && match_table(selects, condition.right_attr.relation_name, table_name)) // 左右都是属性名，并且表名都符合
        ) {
      DefaultConditionFilter condition_filter;
      RC rc = condition_filter.init(*table, condition);
      if (rc != RC::SUCCESS) {
        return rc;
      }
      condition_filters.push_back(condition_filter);
    }
  }

  return select_node.init(trx, table, std::move(schema), std::move(condition_filters));
}

RC create_join_executor(const Selects &selects, const char *db, const std::vector<TupleSet> &tuple_set_list, JoinExeNode &node) {
  if (selects.relation_num <= 1) {
    LOG_ERROR("Select only one table do not need join");
    return RC::GENERIC_ERROR;
  }

  // 把最终想要展示的schema计算出来
  TupleSchema schema;
  TupleSchema schema_tmp;
  for (int i = selects.attr_num - 1; i >= 0; i--) {
    const RelAttr & select_attr = selects.attributes[i];

    if (select_attr.relation_name == nullptr || common::is_blank(select_attr.relation_name)) {
      // 出现了一个不指定表名的属性，那么它的attr name一定是 *
      if ( 0 != strcmp(select_attr.attribute_name, "*")) {
        LOG_TRACE("AttrName must be '*' while rel name is blank");
        return RC::SQL_SYNTAX; // TODO 传递错误
      }

      // 把所有表的都捞出来
      for (int j = selects.relation_num - 1; j >= 0; j--) {
        const char *table_name = selects.relations[j];
        Table *table = DefaultHandler::get_default().find_table(db, table_name);
        if (table == nullptr) {
          LOG_ERROR("Cannot find such table: %s.%s", db, table_name);
          return RC::SCHEMA_TABLE_NOT_EXIST;
        }

        schema_tmp.clear();
        TupleSchema::from_table(table, schema_tmp);
        schema.append(schema_tmp);
      }
    } else {
      const char *table_name = select_attr.relation_name;
      Table *table = DefaultHandler::get_default().find_table(db, table_name);
      if (table == nullptr) {
        LOG_ERROR("Cannot find such table: %s.%s", db, table_name);
        return RC::SCHEMA_TABLE_NOT_EXIST;
      }

      // 表名不是空的
      if (0 == strcmp(select_attr.attribute_name, "*")) {
        // 字段名字是*，就把所有的字段捞出来

        schema_tmp.clear();
        TupleSchema::from_table(table, schema_tmp);
        schema.append(schema_tmp);
      } else {
        const FieldMeta *field_meta = table->table_meta().field(select_attr.attribute_name);
        if (field_meta == nullptr) {
          LOG_ERROR("No such field (%s) in table %s.%s", select_attr.attribute_name, db, table_name);
          return RC::SCHEMA_FIELD_MISSING;
        }
        schema.add(field_meta->type(), table_name, field_meta->name());
      }
    }
  }

  // 把所有同时与多个表相关的condition找出来
  std::vector<const Condition *> condition_filters;
  for (int i = 0; i < selects.condition_num; i++) {
    const Condition &condition = selects.conditions[i];
    if (condition.left_is_attr == 1 && condition.right_is_attr == 1) {
      // 两边都是属性
      // 判断是否不同的表
      if (condition.left_attr.relation_name != nullptr && !common::is_blank(condition.left_attr.relation_name) &&
          condition.right_attr.relation_name != nullptr && !common::is_blank(condition.right_attr.relation_name) &&
          0 != strcmp(condition.left_attr.relation_name, condition.right_attr.relation_name)) {
        // 两个表名都不是空的，并且不同
        condition_filters.push_back(&condition);
      }
    }
  }

  return node.init(tuple_set_list, std::move(schema), std::move(condition_filters));
}
