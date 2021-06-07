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

#include <string.h>
#include <string>

#include "storage/default/default_storage_stage.h"

#include "common/conf/ini.h"
#include "common/io/io.h"
#include "common/lang/string.h"
#include "common/log/log.h"
#include "common/seda/timer_stage.h"
#include "common/metrics/metrics_registry.h"
#include "rc.h"
#include "storage/default/default_handler.h"
#include "storage/common/condition_filter.h"
#include "storage/common/table.h"
#include "storage/common/table_meta.h"
#include "storage/trx/trx.h"
#include "event/execution_plan_event.h"
#include "event/session_event.h"
#include "event/sql_event.h"
#include "event/storage_event.h"
#include "session/session.h"

using namespace common;

const std::string DefaultStorageStage::QUERY_METRIC_TAG = "DefaultStorageStage.query";
const char * CONF_BASE_DIR = "BaseDir";
const char * CONF_SYSTEM_DB = "SystemDb";

const char * DEFAULT_SYSTEM_DB = "sys";

//! Constructor
DefaultStorageStage::DefaultStorageStage(const char *tag) : Stage(tag), handler_(nullptr) {
}

//! Destructor
DefaultStorageStage::~DefaultStorageStage() {
  delete handler_;
  handler_ = nullptr;
}

//! Parse properties, instantiate a stage object
Stage *DefaultStorageStage::makeStage(const std::string &tag) {
  DefaultStorageStage *stage = new (std::nothrow) DefaultStorageStage(tag.c_str());
  if (stage == nullptr) {
    LOG_ERROR("new DefaultStorageStage failed");
    return nullptr;
  }
  stage->setProperties();
  return stage;
}

//! Set properties for this object set in stage specific properties
bool DefaultStorageStage::setProperties() {
  std::string stageNameStr(stageName);
  std::map<std::string, std::string> section = 
      the_global_properties()->get(stageNameStr);
  
  std::map<std::string, std::string>::iterator iter = section.find(CONF_BASE_DIR);
  if (iter == section.end()) {
    LOG_ERROR("Config cannot be empty: %s", CONF_BASE_DIR);
    return false;
  }

  const char *base_dir = iter->second.c_str();
  
  const char *sys_db = DEFAULT_SYSTEM_DB;
  iter = section.find(CONF_SYSTEM_DB);
  if (iter != section.end()) {
    sys_db = iter->second.c_str();
    LOG_INFO("Use %s as system db", sys_db);
  }

  handler_ = &DefaultHandler::get_default();
  if (RC::SUCCESS != handler_->init(base_dir)) {
    LOG_ERROR("Failed to init default handler");
    return false;
  }

  RC ret = handler_->create_db(sys_db);
  if (ret != RC::SUCCESS && ret != RC::SCHEMA_DB_EXIST) {
    LOG_ERROR("Failed to create system db");
    return false;
  }

  ret = handler_->open_db(sys_db);
  if (ret != RC::SUCCESS) {
    LOG_ERROR("Failed to open system db");
    return false;
  }

  Session &default_session = Session::default_session();
  default_session.set_current_db(sys_db);

  LOG_INFO("Open system db success: %s", sys_db);
  return true;
}

//! Initialize stage params and validate outputs
bool DefaultStorageStage::initialize() {
  LOG_TRACE("Enter");

  MetricsRegistry &metricsRegistry = theGlobalMetricsRegistry();
  queryMetric =  new SimpleTimer();
  metricsRegistry.registerMetric(QUERY_METRIC_TAG, queryMetric);

  LOG_TRACE("Exit");
  return true;
}

//! Cleanup after disconnection
void DefaultStorageStage::cleanup() {
  LOG_TRACE("Enter");

  if (handler_) {
    handler_->destroy();
    handler_ = nullptr;
  }
  LOG_TRACE("Exit");
}

void DefaultStorageStage::handleEvent(StageEvent *event) {
  LOG_TRACE("Enter\n");
  TimerStat timerStat(*queryMetric);

  StorageEvent *storage_event = static_cast<StorageEvent *>(event);
  sqlstr *sql = storage_event->exe_event()->sqls();

  SessionEvent *session_event = storage_event->exe_event()->sql_event()->session_event();

  Session *session = session_event->get_client()->session;
  const char *current_db = session->current_db().c_str();

  Trx *current_trx = session->current_trx();

  RC rc = RC::SUCCESS;

  char response[256];
  switch (sql->flag)
  {
  case SCF_INSERT: { // insert into
      const Inserts &inserts = sql->sstr.ins;
      const char *table_name = inserts.relName;
      rc = handler_->insert_record(current_trx, current_db, table_name, inserts.nValues, inserts.values);
      snprintf(response, sizeof(response), "%s\n", strrc(rc));
    }
    break;
  case SCF_UPDATE: {
      const Updates &updates = sql->sstr.upd;
      const char *table_name = updates.relName;
      const char *field_name = updates.attrName;
      int updated_count = 0;
      rc = handler_->update_record(current_trx, current_db, table_name, field_name, &updates.value,
                                   updates.nConditions, updates.conditions, &updated_count);
      snprintf(response, sizeof(response), "%s. %d record(s) updated\n", strrc(rc), updated_count);
    }
    break;
  case SCF_DELETE: {
      const Deletes &deletes = sql->sstr.del;
      const char *table_name = deletes.relName;
      int deleted_count = 0;
      rc = handler_->delete_record(current_trx, current_db, table_name, deletes.nConditions, deletes.conditions, &deleted_count);
      snprintf(response, sizeof(response), "%s. %d record(s) deleted\n", strrc(rc), deleted_count);
    }
    break;
  case SCF_CREATE_TABLE: { // create table
      const CreateTable &create_table = sql->sstr.cret;
      rc = handler_->create_table(current_db, create_table.relName, 
              create_table.attrCount, create_table.attributes);
      snprintf(response, sizeof(response), "%s\n", strrc(rc));
    }
    break;
  case SCF_CREATE_INDEX: {
      const CreateIndex &create_index = sql->sstr.crei;
      rc = handler_->create_index(current_trx, current_db, create_index.relName, create_index.indexName, create_index.attrName);
      snprintf(response, sizeof(response), "%s\n", strrc(rc));
    }
    break;

  case SCF_SHOW_TABLES: {
      Db *db = handler_->find_db(current_db);
      if (nullptr == db) {
        snprintf(response, sizeof(response), "No such database: %s\n", current_db);
      } else {
        std::vector<std::string> all_tables;
        db->all_tables(all_tables);
        if (all_tables.empty()) {
          snprintf(response, sizeof(response), "No table\n");
        } else {
          std::stringstream ss;
          for (const auto &table: all_tables) {
            ss << table << std::endl;
          }
          snprintf(response, sizeof(response), "%s\n", ss.str().c_str()); // TODO
        }
      }
    }
    break;
  case SCF_DESC_TABLE: {
      const char *table_name = sql->sstr.desc_table.table_name;
      Table *table = handler_->find_table(current_db, table_name);
      std::stringstream ss;
      if (table != nullptr) {
        table->table_meta().desc(ss);
      } else {
        ss << "No such table: " << table_name << std::endl;
      }
      snprintf(response, sizeof(response), "%s", ss.str().c_str());
    }
    break;
  default:
      snprintf(response, sizeof(response), "Unsupported sql: %d\n", sql->flag);
      break;
  }

  if (rc == RC::SUCCESS && !session->is_trx_multi_operation_mode()) {
    rc = current_trx->commit();
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Failed to commit trx. rc=%d:%s", rc, strrc(rc));// TODO
    }
  }

  session_event->set_response(response);
  event->doneImmediate();

  LOG_TRACE("Exit\n");
}

void DefaultStorageStage::callbackEvent(StageEvent *event,
                                        CallbackContext *context) {
  LOG_TRACE("Enter\n");
  event->doneImmediate();
  LOG_TRACE("Exit\n");
  return;
}
