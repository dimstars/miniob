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


#include "storage/default/default_handler.h"

#include <string>

#include "common/os/path.h"
#include "common/log/log.h"
#include "common/lang/string.h"
#include "storage/common/record_manager.h"
#include "storage/common/bplus_tree.h"
#include "storage/common/table.h"
#include "storage/common/condition_filter.h"

DefaultHandler &DefaultHandler::get_default() {
  static DefaultHandler handler;
  return handler;
}

RC execute(char *sql) {
  return RC::GENERIC_ERROR;
  /*
          sqlstr *sql_str = NULL;
          RC rc;
          sql_str = getSqlstr();
          rc = parse(sql, sql_str);//只有两种返回结果SUCCESS和SQL_SYNTAX

          if (rc == SUCCESS) {
                  switch (sql_str->flag) {
                          //case 1:
                          //判断SQL语句为select语句
                          //break;

                          case 2:
                          //判断SQL语句为insert语句
                          break;

                          case 3:
                          //判断SQL语句为update语句
                          break;

                          case 4:
                          //判断SQL语句为delete语句
                          break;

                          case 5:
                          //判断SQL语句为createTable语句
                          break;

                          case 6:
                          //判断SQL语句为dropTable语句
                          break;

                          case 7:
                          //判断SQL语句为createIndex语句
                          break;

                          case 8:
                          //判断SQL语句为dropIndex语句
                          break;

                          case 9:
                          //判断为help语句，可以给出帮助提示
                          break;

                          case 10:
                          //判断为exit语句，可以由此进行退出操作
                          break;
          }
          else {
                  //fprintf(stderr, "SQL Errors: %s", sql_str->sstr.errors);
                  return rc;
          }
  */
}

bool match_table(const char *table_name_in_condition, const char *table_name_to_match) {
  if (table_name_in_condition != nullptr) {
    return 0 == strcmp(table_name_in_condition, table_name_to_match);
  }
  return true;
}

RC check_condition_table_equal(const char* table_name, int condition_num, const Condition *conditions) {
  for(int i = 0; i < condition_num; i++) {
    if ((conditions[i].left_is_attr == 0 && conditions[i].right_is_attr == 0) || // 两边都是值
        (conditions[i].left_is_attr == 1 && conditions[i].right_is_attr == 0 && match_table(conditions[i].left_attr.relation_name, table_name)) ||  // 左边是属性右边是值
        (conditions[i].left_is_attr == 0 && conditions[i].right_is_attr == 1 && match_table(conditions[i].right_attr.relation_name, table_name)) ||  // 左边是值，右边是属性名
        (conditions[i].left_is_attr == 1 && conditions[i].right_is_attr == 1 &&
            match_table(conditions[i].left_attr.relation_name, table_name) && match_table(conditions[i].right_attr.relation_name, table_name)) // 左右都是属性名，并且表名都符合
        ) {
      continue;
    } else {
      LOG_WARN("Table and table is not equal");
      return RC::SCHEMA_TABLE_NOT_EQUAL;
    }
  }
  return RC::SUCCESS;
}

DefaultHandler::DefaultHandler() {
}

DefaultHandler::~DefaultHandler() noexcept {
  destroy();
}

RC DefaultHandler::init(const char *base_dir) {
  // 检查目录是否存在，或者创建
  std::string tmp(base_dir);
  tmp += "/db";
  if (!common::check_directory(tmp)) {
    LOG_ERROR("Cannot access base dir: %s. msg=%d:%s", errno, strerror(errno));
    return RC::GENERIC_ERROR; // TODO error
  }

  base_dir_ = base_dir;
  db_dir_ = tmp + "/";

  LOG_INFO("Default handler init with %s success", base_dir);
  return RC::SUCCESS;
}

void DefaultHandler::destroy() {
  sync();

  for (const auto & iter : opened_dbs_) {
    delete iter.second;
  }
  opened_dbs_.clear();
}

RC DefaultHandler::create_db(const char *dbname) {
  if (nullptr == dbname || common::is_blank(dbname)) {
    LOG_WARN("Invalid db name");
    return RC::INVALID_ARGUMENT;
  }

  // 如果对应名录已经存在，返回错误
  std::string dbpath = db_dir_ + dbname;
  if (common::is_directory(dbpath.c_str())) {
    LOG_WARN("Db already exists: %s", dbname);
    return RC::SCHEMA_DB_EXIST;
  }

  if (!common::check_directory(dbpath)) {
    LOG_ERROR("Create db fail: %s", dbpath.c_str());
    return RC::GENERIC_ERROR; // TODO io error
  }
  return RC::SUCCESS;
}

RC DefaultHandler::drop_db(const char *dbname) {
  return RC::GENERIC_ERROR;
}

RC DefaultHandler::open_db(const char *dbname) {
  if (nullptr == dbname || common::is_blank(dbname)) {
    LOG_WARN("Invalid db name");
    return RC::INVALID_ARGUMENT;
  }

  if (opened_dbs_.find(dbname) != opened_dbs_.end()) {
    return RC::SUCCESS;
  }

  std::string dbpath = db_dir_ + dbname;
  if (!common::is_directory(dbpath.c_str())) {
    return RC::SCHEMA_DB_NOT_EXIST;
  }

  // TODO open db
  Db *db = new Db();
  RC ret = RC::SUCCESS;
  if ((ret = db->init(dbname, dbpath.c_str())) != RC::SUCCESS) {
    LOG_ERROR("Failed to open db: %s. error=%d", dbname, ret);
  }
  opened_dbs_[dbname] = db;
  return RC::SUCCESS;
}

RC DefaultHandler::close_db(const char *dbname) {
  return RC::GENERIC_ERROR;
}

RC DefaultHandler::execute(const char *sql) {
  return RC::GENERIC_ERROR;
}

RC DefaultHandler::create_table(const char *dbname, const char *relation_name, int attribute_count, const AttrInfo *attributes) {
  Db *db = find_db(dbname);
  if (db == nullptr) {
    return RC::SCHEMA_DB_NOT_OPENED;
  }
  return db->create_table(relation_name, attribute_count, attributes);
}

RC DefaultHandler::drop_table(const char *dbname, const char *relation_name) {
  Db *db = find_db(dbname);
  if (db == nullptr) {
    return RC::SUCCESS;
  }
  return db->drop_table(relation_name);
}

RC DefaultHandler::create_index(Trx *trx, const char *dbname, const char *relation_name, const char *index_name, const char *attribute_name) {
  Table *table = find_table(dbname, relation_name);
  if (nullptr == table) {
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }
  return table->create_index(trx, index_name, attribute_name);
}

RC DefaultHandler::create_unique_index(Trx *trx, const char *dbname, const char *relation_name, const char *index_name, const char *attribute_name) {
  Table *table = find_table(dbname, relation_name);
  if (nullptr == table) {
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }
  return table->create_index(trx, index_name, attribute_name, true);
}

RC DefaultHandler::drop_index(Trx *trx, const char *dbname, const char *relation_name, const char *index_name) {

  return RC::GENERIC_ERROR;
}

RC DefaultHandler::insert_record(Trx *trx, const char *dbname, const char *relation_name, int tuple_num, const int value_nums[], const Value values[][MAX_NUM]) {
  Table *table = find_table(dbname, relation_name);
  if (nullptr == table) {
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  return table->insert_records(trx, tuple_num, value_nums, values);
}

RC DefaultHandler::delete_record(Trx *trx, const char *dbname, const char *relation_name,
                                 int condition_num, const Condition *conditions, int *deleted_count) {
  Table *table = find_table(dbname, relation_name);
  if (nullptr == table) {
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  RC rc = check_condition_table_equal(relation_name, condition_num, conditions);
  if(rc != RC::SUCCESS) {
    return rc;
  }

  CompositeConditionFilter condition_filter;
  rc = condition_filter.init(*table, conditions, condition_num);
  if (rc != RC::SUCCESS) {
    return rc;
  }
  return table->delete_record(trx, &condition_filter, deleted_count);
}

RC DefaultHandler::update_record(Trx *trx, const char *dbname, const char *relation_name, const char *attribute_name, const Value *value,
                          int condition_num, const Condition *conditions, int *updated_count) {
  Table *table = find_table(dbname, relation_name);
  if (nullptr == table) {
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  const FieldMeta *field_meta = table->table_meta().field(attribute_name);
  if(nullptr == field_meta) {
    LOG_WARN("No such field. %s.%s", table->name(), attribute_name);
    return RC::SCHEMA_FIELD_MISSING;
  }

  // TODO int<->float
  if ((field_meta->type() == INTS && value->type == FLOATS) || (field_meta->type() == INTS && value->type == FLOATS)) {
      // do nothing
  } else if(field_meta->type() != value->type) {
    LOG_WARN("Field type mismatch.");
    return RC::SCHEMA_FIELD_TYPE_MISMATCH;
  } else if(field_meta->type() == DATES && !check_date_legality((char*)value->data)) {
    LOG_WARN("Field value is illeagl. %s.%s", table->name(), attribute_name);
    return RC::SCHEMA_FIELD_VALUE_ILLEGAL;
  }

  RC rc = check_condition_table_equal(relation_name, condition_num, conditions);
  if(rc != RC::SUCCESS) {
    return rc;
  }

  return table->update_record(trx, attribute_name, value, condition_num, conditions, updated_count);
}

Db *DefaultHandler::find_db(const char *dbname) const {
  std::map<std::string, Db*>::const_iterator iter = opened_dbs_.find(dbname);
  if (iter == opened_dbs_.end()) {
    return nullptr;
  }
  return iter->second;
}

Table *DefaultHandler::find_table(const char *dbname, const char *table_name) const {
  if (dbname == nullptr || table_name == nullptr) {
    LOG_WARN("Invalid argument. dbname=%p, table_name=%p", dbname, table_name);
    return nullptr;
  }
  Db *db = find_db(dbname);
  if (nullptr == db) {
    return nullptr;
  }

  return db->find_table(table_name);
}

RC DefaultHandler::sync() {
  RC rc = RC::SUCCESS;
  for (const auto & db_pair: opened_dbs_) {
    Db *db = db_pair.second;
    rc = db->sync();
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Failed to sync db. name=%s, rc=%d:%s", db->name(), rc, strrc(rc));
      return rc;
    }
  }
  return rc;
}